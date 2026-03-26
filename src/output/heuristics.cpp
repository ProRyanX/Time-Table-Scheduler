#include "heuristics.h"
#include <algorithm>
#include <vector>

// -----------------------------------------------------------
// constraintScore()
// Higher score = more constrained = place earlier in DFS
// Labs score higher because they need specific room types
// Higher frequency = harder to fit = score higher
// -----------------------------------------------------------
int constraintScore(const ClassEvent& e) {
    int score = 0;

    // More sessions per week = fewer valid dday combinations = harder
    score += e.getFrequency()*10;

    // Longer classes need consecutive/longer open slots = harder to fit
    if(e.getDuration() > 1) {
        score += (e.getDuration()-1)*5;
    }

    return score;
}

// -----------------------------------------------------------
// sortMCV()
// Implements MRV (Most Restricted Variable).
// Places hardest-to-schedule events first so the DFS fails
// fast on bad branches. Labs go first (fewer valid rooms),
// then tie-break by constraintScore() (frequency + duration).
// -----------------------------------------------------------
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