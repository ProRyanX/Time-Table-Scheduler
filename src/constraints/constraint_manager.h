#ifndef CONSTRAINT_MANAGER_H
#define CONSTRAINT_MANAGER_H

#include "../../include/globals.h"

// Initialize all global arrays to false/empty and block lunch slots
void initializeConstraintArrays();

// Central O(1) safety check used by DFS solver
// Returns true if the slot is safe for this event
// NOTE: Caller (solver) must pass pre-mapped integer indices
//       (parser should provide teacherIdx, sectionIdx, subjectIdx)
bool isSafe(int roomIdx, int day, int slot, int teacherIdx, int sectionIdx, int subjectIdx);

// Mark slot as occupied (called when placing an event)
void assignEvent(int roomIdx, int day, int slot, int teacherIdx, int sectionIdx, int subjectIdx, int eventIndex);

// Unmark slot (called during backtracking)
void unassignEvent(int roomIdx, int day, int slot, int teacherIdx, int sectionIdx, int subjectIdx);

#endif