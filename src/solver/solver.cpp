#include "solver.h"
#include "../../include/constants.h"
#include "../constraints/constraint_manager.h"
#include <iostream>

using namespace std;

bool solveTimeTable(vector<ClassEvent>& events, 
                    int index,
                    const std::unordered_map<std::string, int>& teacherMap,
                    const std::unordered_map<std::string, int>& sectionMap,
                    const std::unordered_map<std::string, int>& subjectMap,
                    int numRooms
                ){
    if(index>=(int)events.size()){
        return true;
    }

    const ClassEvent& currentEvent = events[index];
    int teacherIdx = teacherMap.at(currentEvent.getTeacherID());
    int sectionIdx = sectionMap.at(currentEvent.getSectionID());
    int subjectIdx = subjectMap.at(currentEvent.getSubjectName());

    for(int day = 0; day < MAX_DAYS; day++) {
        for(int slot = 0; slot < MAX_SLOTS; slot ++) {
            if(slot == LUNCH_SLOT) continue;
            for(int room = 0; room < numRooms; room++) {
                if(!isSafe(room, day, slot, teacherIdx, sectionIdx, subjectIdx)) { continue; }
                    
                assignEvent(room, day, slot, teacherIdx, sectionIdx, subjectIdx, index);
                if(solveTimeTable(events, index + 1, teacherMap, sectionMap, subjectMap, numRooms)) { return true; }
                unassignEvent(room, day, slot, teacherIdx, sectionIdx, subjectIdx);
            }
        }
    }
    return false;
}