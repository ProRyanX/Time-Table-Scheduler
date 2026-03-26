#ifndef CONSTRAINT_MANAGER
#define CONSTRAINT_MANAGER

#include "../../include/common.h"
#include "../../include/constants.h"


bool isSafe(int day, int slot, int roomIndex, const ClassEvent& event);
void markBusy(int day, int slot, int roomIndex, const ClassEvent& event, bool status);

#endif