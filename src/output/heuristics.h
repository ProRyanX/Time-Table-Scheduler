#pragma once
#include "../../include/common.h"
#include <vector>

// Sorts allEvents using MCV heuristic:
// - Labs come first (harder to place due to room type constraints)
// - Tie-break: higher frequency events before lower frequency
void sortMCV(std::vector<ClassEvent>& allEvents);
