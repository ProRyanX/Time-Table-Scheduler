#ifndef CONSTRAINT_MANAGER_H
#define CONSTRAINT_MANAGER_H
// Declare the initialization function
void initializeConstraintArrays();

// Update these 3 signatures to include 'int duration' 
// so they perfectly match constraints.cpp
bool isSafe(int roomIdx, int day, int startSlot, int duration, int teacherIdx, int sectionIdx, int subjectIdx);

void assignEvent(int roomIdx, int day, int startSlot, int duration, int teacherIdx, int sectionIdx, int subjectIdx);

void unassignEvent(int roomIdx, int day, int startSlot, int duration, int teacherIdx, int sectionIdx, int subjectIdx);

#endif