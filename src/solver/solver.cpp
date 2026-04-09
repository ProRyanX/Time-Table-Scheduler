#include "solver.h"
#include "../../include/constants.h"
#include "../constraints/constraint_manager.h"
#include "../../include/globals.h"
#include <unordered_map>
#include <string>

using namespace std;

static int getOrCreateIndex(
    unordered_map<string, int>& indexMap,
    const string& key,
    int maxSize
) {
    auto it = indexMap.find(key);
    if (it != indexMap.end()) {
        return it->second;
    }

    int newIndex = static_cast<int>(indexMap.size());
    if (newIndex >= maxSize) {
        return -1;
    }

    indexMap[key] = newIndex;
    return newIndex;
}

static int getTeacherIndex(const string& teacherID) {
    static unordered_map<string, int> teacherIndexMap;
    return getOrCreateIndex(teacherIndexMap, teacherID, MAX_TEACHERS);
}

static int getSectionIndex(const string& sectionID) {
    static unordered_map<string, int> sectionIndexMap;
    return getOrCreateIndex(sectionIndexMap, sectionID, MAX_SECTIONS);
}

static int getSubjectIndex(const string& subjectName) {
    static unordered_map<string, int> subjectIndexMap;
    return getOrCreateIndex(subjectIndexMap, subjectName, MAX_SUBJECTS);
}

bool solveTimeTable(vector<ClassEvent>& events, int index) {
    if (index >= static_cast<int>(events.size())) {
        return true;
    }

    ClassEvent& currentEvent = events[index];

    int teacherIdx = getTeacherIndex(currentEvent.getTeacherID());
    int sectionIdx = getSectionIndex(currentEvent.getSectionID());
    int subjectIdx = getSubjectIndex(currentEvent.getSubjectName());

    if (teacherIdx < 0 || sectionIdx < 0 || subjectIdx < 0) {
        return false;
    }

    for (int day = 0; day < MAX_DAYS; ++day) {
        for (int slot = 0; slot < MAX_SLOTS; ++slot) {
            if (slot == LUNCH_SLOT) {
                continue;
            }

            for (int roomIdx = 0; roomIdx < MAX_ROOMS; ++roomIdx) {
                int duration = currentEvent.getDuration();
                if (isSafe(roomIdx, day, slot, duration, teacherIdx, sectionIdx, subjectIdx)) {
                    assignEvent(roomIdx, day, slot, duration, teacherIdx, sectionIdx, subjectIdx);  

                    for(int k=0; k < duration; k++) timetable[roomIdx][day][slot+k] = index;
                    if (solveTimeTable(events, index + 1)) {
                        return true;
                    }

                    unassignEvent(roomIdx, day, slot, duration, teacherIdx, sectionIdx, subjectIdx);
                }
            }
        }
    }

    return false;
}