#pragma once
#include "../../include/common.h"
#include "../../include/constants.h"
#include <vector>

// ------------------------------------------------------------------------
// MRV Heuristics
// ------------------------------------------------------------------------
// - Labs come first (harder to place due to room type constraints)
// - Tie-break: higher frequency events before lower frequency
void sortMCV(std::vector<ClassEvent>& allEvents);


// ------------------------------------------------------------------------
// Scoring and Evaluation
// ------------------------------------------------------------------------
// Computes multi-factor constraint score for a single ClassEvent.
// Higher score = harder to place = should be scheduled earlier.
int constraintScore(const ClassEvent& e);

// Evaluate a completed timetable and return its TimetableScore.
// Pass the full timetable grid and the event list used to build it.
TimetableScore evaluateTimetable(
    const int timetable[MAX_ROOMS][MAX_DAYS][MAX_SLOTS],        // [rooms][days][slots]
    const vector<ClassEvent>& events,
    int numRooms
);


// ------------------------------------------------------------------------
// Multi-Candidate Generation & Selection
// ------------------------------------------------------------------------
// Generate 'numCandidates' shuffled orderings of events.
// Each ordering is a permutation that preserves lab-first invariant
// but varies theory event order - used to explore the solution space.
vector<vector<ClassEvent>> generateCandidateOrderings(
    const vector<ClassEvent>& events,
    int numCandidates
);

// Given a set of scored timetables, return the index of the best one
// and populate 'explanation' with a human-readable message.
int selectBestTimetable(
    vector<TimetableScore>& scores,
    string& explanation
);


// ------------------------------------------------------------------------
// Metric Helpers
// (support evaluateTimetable; can also be called from constraints)
// ------------------------------------------------------------------------
// Count idle gaps (empty slots sandwiched between occupied slots)
// for a single room across all days.
int countIdleGaps(const int timetable[MAX_ROOMS][MAX_DAYS][MAX_SLOTS], int roomIndex, int numSlots);

// Count hard conflicts in a timetable grid (double-bookings, etc.)
int countConflicts(const int timetable[MAX_ROOMS][MAX_DAYS][MAX_SLOTS], int numRooms, int numSlots);

// Prints a formatted comparison table of all candidate TimetableScores.
void printCandidateComparison(const vector<TimetableScore>& scores);

// Print the explanation for why a particular timetable was selected.
void printSelectionExplanation(const TimetableScore& best, int candidateIndex);