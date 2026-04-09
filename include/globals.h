#ifndef GLOBALS_H
#define GLOBALS_H

#include "constants.h"
#include "common.h"

// Global 3D boolean arrays for O(1) constraint checking
extern bool roomBusy[MAX_ROOMS][MAX_DAYS][MAX_SLOTS];
extern bool teacherBusy[MAX_TEACHERS][MAX_DAYS][MAX_SLOTS];
extern bool sectionBusy[MAX_SECTIONS][MAX_DAYS][MAX_SLOTS];
extern bool subjectOnDay[MAX_SECTIONS][MAX_DAYS][MAX_SUBJECTS];

// Timetable (Room x Day x Slot)
// Stores the index of the ClassEvent assigned to that slot
// -1 = empty slot
extern int timetable[MAX_ROOMS][MAX_DAYS][MAX_SLOTS];

#endif