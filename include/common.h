#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <vector>

using namespace std;

// Course Structure
class Course {
    private:
        int id;
        string name;
        int teacher_id;
        int lectures_per_week;

    public:
        Course(int id, string name, int teacher_id, int lectures_per_week) {
            this->id = id;
            this->name = name;
            this->teacher_id = teacher_id;
            this->lectures_per_week = lectures_per_week;
        }
};

// Teacher Structure
class Teacher {
    private:
        int id;
        string name;

    public:
        Teacher(int id, string name) {
            this->id = id;
            this->name = name;
        }
};

// Room Structure
class Room {
    private:
        string RoomID;
        string name;
        int capacity;

    public:
        Room(string RoomID, string name, int capacity) {
            this->RoomID = RoomID;
            this->name = name;
            this->capacity = capacity;
        }
        string getRoomID() {
            return RoomID;
        }

};

// Time Slot
class TimeSlot {
    private:
        int day;    // 0-4 (Mon-Fri)
        int slot;   // 0-7 (Periods)

    public:
        TimeSlot(int day, int slot) {
            this->day = day;
            this->slot = slot;
        }
};

// Class Events
class ClassEvent {
    private:
        string sectionID;
        string subjectName;
        string teacher_ID;
        int duration;
        int frequency;
        bool Lab;

    public:
        ClassEvent(string sectionID, string subjectName, string teacher_ID, int duration, int frequency, bool Lab) {
            this->sectionID = sectionID;
            this->subjectName = subjectName;
            this->teacher_ID = teacher_ID;
            this->duration = duration;
            this->frequency = frequency;
            this->Lab = Lab;

        }

        int getDuration() const{
            return duration;
        }
        int getFrequency() const{
            return frequency;
        }
        bool isLab() const{
            return Lab;
        }
};

// Time Table
class TimetableCell {
    private:
        bool occupied;
        string subjectName;
        string sectionID;
        string teacher_ID;
        bool Lab;

    public:
        TimetableCell(
            bool occupied,
            string subjectName,
            string sectionID,
            string teacher_ID,
            bool Lab
        ) {
            this->occupied = occupied;       this->subjectName = subjectName;        
            this->sectionID = sectionID;     this->teacher_ID = teacher_ID;
            this->Lab = Lab;
        }

        bool isOccupied()       const   { return occupied; }
        string getSubjectName() const   { return subjectName; }
        string getSectionID()   const   { return sectionID; }
        string getTeacherID()   const   { return teacher_ID; }
        bool isLab()            const   { return Lab; }
};

#endif