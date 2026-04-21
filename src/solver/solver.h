#ifndef SOLVER_H
#define SOLVER_H

#include "../../include/common.h"
#include "../../include/constants.h"
#include <vector>
#include <unordered_map>

bool solveTimeTable(std::vector<ClassEvent>& events, 
                    int index,
                    const std::unordered_map<std::string, int>& teacherMap,
                    const std::unordered_map<std::string, int>& sectionMap,
                    const std::unordered_map<std::string, int>& subjectMap,
                    int numRooms
                );

#endif