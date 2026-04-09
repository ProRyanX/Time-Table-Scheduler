#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <vector>

using namespace std;

// Course Structure
class Course {
    private:
        string id;
        string name;
        string teacher_id;
        int lectures_per_week;

    public:
        Course(string id, string name, string teacher_id, int lectures_per_week) {
            this->id = id;
            this->name = name;
            this->teacher_id = teacher_id;
            this->lectures_per_week = lectures_per_week;
        }
};

// Teacher Structure
class Teacher {
    private:
        string id;
        string name;

    public:
        Teacher(string id, string name) {
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
        string getRoomID() const {
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
        string getSectionID() const {
    return sectionID;
}

string getSubjectName() const {
    return subjectName;
}

string getTeacherID() const {
    return teacher_ID;
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
        TimetableCell() {
            this->occupied = false;
            this->subjectName = "";
            this->sectionID = "";
            this->teacher_ID = "";
            this->Lab = false;
        }
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