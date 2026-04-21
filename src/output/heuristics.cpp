#include "heuristics.h"
#include "../../include/constants.h"

#include <algorithm>
#include <vector>
#include <string>
#include <numeric>      // iota
#include <random>       // shuffle
#include <sstream>
#include <iostream>
#include <iomanip>

TimetableScore g_best_timetable_score;

// -----------------------------------------------------------------------
// constraintScore()
// 
// Multi-factor score for a single ClassEvent.
// Higher score = more constrained = place earlier in DFS (MRV).
//
// Factors:
//   - isLab()          : labs need specific room types   (+20 base)
//   - getFrequency()   : more sessions = harder to fit   (x10)
//   - getDuration()    : longer slots need contiguous    ((d-1)x5)
//   - lab_duration     : long labs are hardest of all    (+10 bonus)
// -----------------------------------------------------------------------
int constraintScore(const ClassEvent& e) {
    int score = 0;

    // Factor 1: Lab penalty - labs need dedicated lab rooms
    if (e.isLab()) {
        score += 20;
    }

    // Factor 2: Frequency - more sessions per week means fewer
    // valid day combinations remain as scheduling fills up
    score += e.getFrequency() * 10;

    // Factor 3: Duration - long sessions need consecutive open slots
    if (e.getDuration() > 1) {
        score += (e.getDuration() - 1) * 5;
    }

    // Factor 4: Combined lab+duration bonus - hardest to place
    if (e.isLab() && e.getDuration() > 1) {
        score += 10;
    }

    return score;
}

// -----------------------------------------------------------------------
// sortMCV()

// Implements MRV (Most Restricted Variable).
// Places hardest-to-schedule events first so the DFS fails
// fast on bad branches. Labs go first (fewer valid rooms),
// then tie-break by constraintScore() (frequency + duration).
// -----------------------------------------------------------------------
void sortMCV(std::vector<ClassEvent>& allEvents) {
    std::sort(allEvents.begin(), allEvents.end(), 
        [](const ClassEvent& a, const ClassEvent& b) {
            // Primary: lab events before theory events
            if (a.isLab() != b.isLab()) {
                return a.isLab() > b.isLab();       // true (1) > false (0)
            }

            // Secondary: among events of the same type,
            // place the more constrained  one first
            return constraintScore(a) > constraintScore(b);
        }
    );
}

// -----------------------------------------------------------------------
// countIdleGaps()
//
// Counts idle gaps for a given room: empty slots that are
// sandwiched between two occupied slots on the same day.
// This penalises fragmented schedules.
// -----------------------------------------------------------------------
int countIdleGaps(const int timetable[][5][8], int roomIndex, int numSlots) {
    int gaps = 0;
    for (int day = 0; day < 5; ++day) {
        // Find first and last occupied slot in this room on this day
        int first = -1, last = -1;
        for (int slot = 0; slot < numSlots; ++slot) {
            if (timetable[roomIndex][day][slot] != -1) {
                if (first == -1) { first = slot; }
                last = slot;
            }
        }
        // Count empty slots strictly between first and last
        if (first != -1 && last > first) {
            for (int slot = first + 1; slot < last; ++slot) {
                if (timetable[roomIndex][day][slot] == -1) {
                    gaps++;
                }
            }
        }
    }
    return gaps;
}

// -----------------------------------------------------------------------
// countConflicts()
//
// Counts hard conflicts; slots in the timetable where the same
// index appears more than once across rooms on the same day/slot
// (i.e., double-booked events).
// -----------------------------------------------------------------------
int countConflicts(const int timetable[][5][8], int numRooms, int numSlots) {
    int conflicts = 0;
    for (int day = 0; day < 5; ++day) {
        for (int slot = 0; slot < numSlots; ++slot) {
            // Check for duplicate event indices across rooms
            vector<int> seen;
            for (int room = 0; room < numRooms; ++room) {
                int idx = timetable[room][day][slot];
                if (idx == -1) { continue; }
                if (find(seen.begin(), seen.end(), idx) != seen.end()) {
                    conflicts++;
                }
                else {
                    seen.push_back(idx);
                }
            }
        }
    }
    return conflicts;
}

// -----------------------------------------------------------------------
// evaluateTimetable()
//
// Produces a TimetableScore for a completed timetable.
//
// Scoring breakdown:
//    +50 per event with no conflicts
//    -20 per conflict
//    -5 per idle gap
//    +15 per lab event placed (confirms lab room assignment)
//    +10 distribution bonus if no day has more than 2x the events
//        of the least-loaded day (balanced week)
// -----------------------------------------------------------------------
TimetableScore evaluateTimetable(
    const int timetable[][5][8],
    const vector<ClassEvent>& events,
    int numRooms
) {
    TimetableScore s;

    int conflicts = countConflicts(timetable, numRooms, MAX_SLOTS);
    s.setConflict(conflicts);

    // Idle gaps across all rooms
    for (int r = 0; r < numRooms; ++r) {
        s.incrementIdleGaps(countIdleGaps(timetable, r, 8));
    }

    // Lab placement score: count lab events appear in the grid
    for (const auto& e: events) {
        if (e.isLab()) { s.incrementLabPlaceScore(15);}
    }

    // Distribution score: events per day 
    int eventsPerDay[5] = {0, 0, 0, 0, 0};
    for (int r = 0; r < numRooms; ++r) {
        for (int d = 0; d < 5; ++d) {
            for (int sl = 0; sl < 8; ++sl) {
                if (timetable[r][d][sl] != -1) { eventsPerDay[d]++; }
            }
        }
    }
    int minDay = *min_element(eventsPerDay, eventsPerDay + 5);
    int maxDay = *max_element(eventsPerDay, eventsPerDay + 5);
    // Reward balanced distribution
    if (minDay > 0 && maxDay <= 2 * minDay) {
        s.incrementDistributionScore(10);
    }

    // Aggregate total score
    int base = (int)events.size() * 50;
    s.setTotalScore(
        base
        - (s.getConflicts() * 20)
        - (s.getIdleGaps() * 5)
        + s.getLabPlaceScore()
        + s.getDistributionScore()
    );

    // Build explanation string
    ostringstream oss;
    oss << "Score=" << s.getTotalScore()
        << " | Conflicts=" << s.getConflicts()
        << " | IdleGaps=" << s.getIdleGaps()
        << " | LabScore=" << s.getLabPlaceScore()
        << " | Distribution=" << s.getDistributionScore();
    s.setExplanation(oss.str());

    return s;
}

// -----------------------------------------------------------------------
// generateCandidateOrdering()
// 
// Produces 'numCandidates' event orderings for multirun solver.
// Each ordering:
//    1. Keeps all lab events at the front (MRV invariant)
//    2. Randomly shuffles theory events behind them
// This explores different branches of the solution space.
// -----------------------------------------------------------------------
vector<vector<ClassEvent>> generateCandidateOrderings(
    const vector<ClassEvent>& events,
    int numCandidates
) {
    // Split into labs and theory
    vector<ClassEvent> labs, theory;
    for (const auto& e : events) {
        if (e.isLab()) { labs.push_back(e); }
        else           { theory.push_back(e); }
    }

    // Sort labs by constraint score (deterministic)
    sort(labs.begin(), labs.end(),
        [](const ClassEvent& a, const ClassEvent& b) {
            return constraintScore(a) > constraintScore(b);
        }
    );

    vector<vector<ClassEvent>> orderings;
    mt19937 rng(42);        // Fixed seed for reproducibility

    for (int i = 0; i < numCandidates; ++i) {
        vector<ClassEvent> theoryShuffled = theory;
        shuffle(theoryShuffled.begin(), theoryShuffled.end(), rng);

        // Combine: labs first, then shuffled theory
        vector<ClassEvent> ordering = labs;
        ordering.insert(ordering.end(), theoryShuffled.begin(), theoryShuffled.end());
        orderings.push_back(ordering);
    }

    return orderings;
}

// -----------------------------------------------------------------------
// selectBestTimetable()
//
// Compares all candidate TimetableScores and returns the index
// of the best one. Prioritises:
//   1. Fewest conflicts (hard constraint - non-negotiable)
//   2. Highest totalScore (captures gaps + distribution)
// Populates 'explanation' with human-readable message
// -----------------------------------------------------------------------
int selectBestTimetable(
    vector<TimetableScore>& scores,
    string& explanation
) {
    if (scores.empty()) {
        explanation = "No candidates to evaluate.";
        return -1;
    }

    int bestIdx = 0;
    for(int i = 1; i < scores.size(); ++i) {
        TimetableScore curr = scores[i];
        TimetableScore best = scores[bestIdx];

        // Rule 1: fewer conflicts wins unconditionally
        if (curr.getConflicts() < best.getConflicts()) {
            bestIdx = i;
            continue;
        }
        // Rule 2: same conflicts - higher total score wins
        if (curr.getConflicts() == best.getConflicts() &&
            curr.getTotalScore() > best.getTotalScore()
        ) {
            bestIdx = i;
        }
    }

    TimetableScore& winner = scores[bestIdx];
    ostringstream oss;
    oss << "Candidate #" << (bestIdx + 1) << " selected.\n"
        << "Reason: ";
    if (winner.getConflicts() == 0) {
        oss << "Zero conflicts achieved. ";
    }
    else {
        oss << "Fewest conflicts (" << winner.getConflicts() << ").";
    }
    oss << "Best overall score of " << winner.getTotalScore() << " "
        << "(IdleGaps=" << winner.getIdleGaps() 
        << ", LabScore=" << winner.getLabPlaceScore()
        << ", Distribution=" << winner.getDistributionScore() << ").";

    explanation = oss.str();
    return bestIdx;
}

// -----------------------------------------------------------------------
// printCandidateComparison()
//
// Prints a formatted table comparin all candidate timetables.
// Useful for DAA demonstration and analysis.
//-----------------------------------------------------------------------
void printCandidateComparison(const vector<TimetableScore>& scores) {
    cout << "\n";
    cout << "╔════════════╦═══════════╦═══════════╦══════════╦══════════╦══════════╗\n";
    cout << "║ Candidate  ║   Score   ║ Conflicts ║   Gaps   ║ LabScore ║  Distrib ║\n";
    cout << "╠════════════╬═══════════╬═══════════╬══════════╬══════════╬══════════╣\n";

    for (int i = 0; i < scores.size(); ++i) {
        TimetableScore s = scores[i];
        cout << "║ "
            << setw(8) << left << ("#" + to_string(i + 1))
            << " ║ "
            << setw(9) << right << s.getTotalScore()
            << " ║ "
            << setw(9) << s.getConflicts()
            << " ║ "
            << setw(8) << s.getIdleGaps()
            << " ║ "
            << setw(8) << s.getLabPlaceScore()
            << " ║ "
            << setw(8) << s.getDistributionScore()
            << " ║\n";
    }
    cout << "╚════════════╩═══════════╩═══════════╩══════════╩══════════╩══════════╝\n";
}

// -----------------------------------------------------------------------
// printSelectionExplanation()
//
// Prints the explanability layer - why the best timetable won.
// -----------------------------------------------------------------------
void printSelectionExplanation (TimetableScore& best, int candidateIndex) {
    cout << "\n=== SELECTED TIMETABLE: Candidate #" << (candidateIndex + 1) << " ===\n";
    cout << " Total Score     : " << best.getTotalScore()         << "\n";
    cout << " Conflicts       : " << best.getConflicts()          << "\n";
    cout << " Idle Gaps       : " << best.getIdleGaps()           << "\n";
    cout << " Lab Placement   : " << best.getLabPlaceScore()      << "\n";
    cout << " Distribution    : " << best.getDistributionScore()  << "\n";
    cout << " Explanation     : " << best.getExplanation()        << "\n";
    cout << "==========================================\n\n";
}