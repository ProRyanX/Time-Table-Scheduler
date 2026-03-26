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
        int id;
        string name;
        int capacity;

    public:
        Room(int id, string name, int capacity) {
            this->id = id;
            this->name = name;
            this->capacity = capacity;
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

#endif