#include "../../include/constants.h"
#include "constraint_manager.h"

#include <cstring>

bool roomBusy[MAX_ROOMS][MAX_DAYS][MAX_SLOTS];
bool teacherBusy[MAX_TEACHERS][MAX_DAYS][MAX_SLOTS];
bool sectionBusy[MAX_SECTIONS][MAX_DAYS][MAX_SLOTS];
bool subjectOnDay[MAX_SECTIONS][MAX_DAYS][MAX_SUBJECTS];

int timetable[MAX_ROOMS][MAX_DAYS][MAX_SLOTS];

void initializeConstraintArrays() {
    std::memset(roomBusy, 0, sizeof(roomBusy));
    std::memset(teacherBusy, 0, sizeof(teacherBusy));
    std::memset(sectionBusy, 0, sizeof(sectionBusy));
    std::memset(subjectOnDay, 0, sizeof(subjectOnDay));

    for (int r = 0; r < MAX_ROOMS; ++r) {
        for (int d = 0; d < MAX_DAYS; ++d) {
            for (int s = 0; s < MAX_SLOTS; ++s) {
                timetable[r][d][s] = -1;
            }
        }
    }

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
}

bool isSafe(int roomIdx, int day, int startSlot, int duration, int teacherIdx, int sectionIdx, int subjectIdx) {
    if (startSlot + duration > MAX_SLOTS) return false;

    for (int k = 0; k < duration; ++k) {
        int currentSlot = startSlot + k;
        if (currentSlot == LUNCH_SLOT) return false; // Cannot span across lunch
        if (roomBusy[roomIdx][day][currentSlot]) return false;
        if (teacherBusy[teacherIdx][day][currentSlot]) return false;
        if (sectionBusy[sectionIdx][day][currentSlot]) return false;
    }
    // Daily subject limit only needs to be checked once per day
    if (subjectOnDay[sectionIdx][day][subjectIdx]) return false;

    return true;
}

void assignEvent(int roomIdx, int day, int startSlot, int duration, int teacherIdx, int sectionIdx, int subjectIdx) {
    for (int k = 0; k < duration; ++k) {
        int currentSlot = startSlot + k;
        roomBusy[roomIdx][day][currentSlot] = true;
        teacherBusy[teacherIdx][day][currentSlot] = true;
        sectionBusy[sectionIdx][day][currentSlot] = true;
    }
    subjectOnDay[sectionIdx][day][subjectIdx] = true;
}
void unassignEvent(int roomIdx, int day, int startSlot, int duration, int teacherIdx, int sectionIdx, int subjectIdx) {
    for (int k = 0; k < duration; ++k) {
        int currentSlot = startSlot + k;
        roomBusy[roomIdx][day][currentSlot] = false;
        teacherBusy[teacherIdx][day][currentSlot] = false;
        sectionBusy[sectionIdx][day][currentSlot] = false;
        
        timetable[roomIdx][day][currentSlot] = -1;
    }
    subjectOnDay[sectionIdx][day][subjectIdx] = false;
}