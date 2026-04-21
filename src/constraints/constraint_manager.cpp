#include "constraint_manager.h"
#include <cstring>  // for memset

// Actual definitions of the global arrays (declared as extern in globals.h)
bool roomBusy[MAX_ROOMS][MAX_DAYS][MAX_SLOTS];
bool teacherBusy[MAX_TEACHERS][MAX_DAYS][MAX_SLOTS];
bool sectionBusy[MAX_SECTIONS][MAX_DAYS][MAX_SLOTS];
bool subjectOnDay[MAX_SUBJECTS][MAX_DAYS];

int timetable[MAX_ROOMS][MAX_DAYS][MAX_SLOTS];

void initializeConstraintArrays() {
    // Zero out all constraint arrays
    std::memset(roomBusy, 0, sizeof(roomBusy));
    std::memset(teacherBusy, 0, sizeof(teacherBusy));
    std::memset(sectionBusy, 0, sizeof(sectionBusy));
    std::memset(subjectOnDay, 0, sizeof(subjectOnDay));

    // Initialize timetable to empty (-1)
    for (int r = 0; r < MAX_ROOMS; ++r) {
        for (int d = 0; d < MAX_DAYS; ++d) {
            for (int s = 0; s < MAX_SLOTS; ++s) {
                timetable[r][d][s] = -1;
            }
        }
    }

    // ================== LUNCH SLOT BLOCKING ==================
    // Block lunch slot (index 4) for all rooms, teachers and sections
    for (int r = 0; r < MAX_ROOMS; ++r) {
        for (int d = 0; d < MAX_DAYS; ++d) {
            roomBusy[r][d][LUNCH_SLOT] = true;
        }
    }
    for (int t = 0; t < MAX_TEACHERS; ++t) {
        for (int d = 0; d < MAX_DAYS; ++d) {
            teacherBusy[t][d][LUNCH_SLOT] = true;
        }
    }
    for (int sec = 0; sec < MAX_SECTIONS; ++sec) {
        for (int d = 0; d < MAX_DAYS; ++d) {
            sectionBusy[sec][d][LUNCH_SLOT] = true;
        }
    }
    // subjectOnDay does not need lunch blocking
}

bool isSafe(int roomIdx, int day, int slot, int teacherIdx, int sectionIdx, int subjectIdx) {
    // Already blocked by initializeConstraintArrays() for lunch
    if (roomBusy[roomIdx][day][slot])     return false;
    if (teacherBusy[teacherIdx][day][slot]) return false;
    if (sectionBusy[sectionIdx][day][slot]) return false;
    if (subjectOnDay[subjectIdx][day])     return false;

    return true;
}

void assignEvent(int roomIdx, int day, int slot, int teacherIdx, int sectionIdx, int subjectIdx, int eventIndex) {
    roomBusy[roomIdx][day][slot]     = true;
    teacherBusy[teacherIdx][day][slot] = true;
    sectionBusy[sectionIdx][day][slot] = true;
    subjectOnDay[subjectIdx][day]     = true;
    timetable[roomIdx][day][slot] = eventIndex;
}

void unassignEvent(int roomIdx, int day, int slot, int teacherIdx, int sectionIdx, int subjectIdx) {
    roomBusy[roomIdx][day][slot]     = false;
    teacherBusy[teacherIdx][day][slot] = false;
    sectionBusy[sectionIdx][day][slot] = false;
    subjectOnDay[subjectIdx][day]     = false;

    timetable[roomIdx][day][slot] = -1;
}