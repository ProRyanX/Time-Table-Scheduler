#include "solver.h"
#include "../../include/constants.h"
#include "../constraints/constraint_manager.h"
#include <iostream>

using namespace std;

bool solveTimeTable(vector<ClassEvent>& events, int index){
    if(index>=(int)events.size()){
        return true;
    }

    ClassEvent& currentEvent = events[index];

    for(int day = 0; day<MAX_DAYS; day++){
        for(int slot = 0; slot<MAX_SLOTS; slot ++){
            if(slot == LUNCH_SLOT) continue;
            for(int r = 0; r<MAX_ROOMS; r++){
                if(isSafe(day,slot,r,currentEvent)){
                    markBusy(day, slot,r,currentEvent, true);

                    if(solveTimeTable(events, index+1)){
                        return true;
                    }

                    markBusy(day,slot,r,currentEvent, false);
                }
            
            }
        }
    }
    return false;
}