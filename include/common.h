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
        string sectionId;
        int lectures_per_week;
        int duration;
        bool lab;

    public:
        Course(string id, string name, string teacher_id, string sectionId, 
            int lectures_per_week, int duration, bool lab) {
            this->id = id;
            this->name = name;
            this->teacher_id = teacher_id;
            this->sectionId = sectionId;
            this->lectures_per_week = lectures_per_week;
            this->duration = duration;
            this->lab = lab;
        }
        string getID() {
            return id;
        }
        string getName() {
            return name;
        }
        string getTeacherID() {
            return teacher_id;
        }
        string getSectionID() {
            return sectionId;
        }
        int getLecturesPerWeek() {
            return lectures_per_week;
        }
        int getDuration() {
            return duration;
        }
        bool isLab() {
            return lab;
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
        string getID() {
            return id;
        }
        string getName() {
            return name;
        }
};

// Room Structure
class Room {
    private:
        string RoomID;
        string name;
        int capacity;
        string roomType;

    public:
        Room(string RoomID, string name, int capacity, string roomType) {
            this->RoomID = RoomID;
            this->name = name;
            this->capacity = capacity;
            this->roomType = roomType;
        }
        string getRoomID() {
            return RoomID;
        }
        string getRoomName() {
            return name;
        }
        int getCapacity() {
            return capacity;
        }
        string getRoomType() {
            return roomType;
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
        string getSectionID() const{
            return sectionID;
        }
        string getSubjectName() const{
            return subjectName;
        }
        string getTeacherID() const{
            return teacher_ID;
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
        TimetableCell() {}
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

// Score of a Timetable
class TimetableScore {
    private:
        int totalScore;             // Overall quality score (higher = better)
        int conflicts;              // Hard constraint violations detected
        int idleGaps;               // Idle/free periods between classes
        int labPlacementScore;      // Bonus for correct lab placement
        int distributionScore;      // Bonus for balanced spread across week
        string explanation;         // Human-readable reason this timetable was chosen/rejected

    public:
        TimetableScore() {
            totalScore = 0;
            conflicts = 0;
            idleGaps = 0;
            labPlacementScore = 0;
            distributionScore = 0;
        }

        void incrementIdleGaps(int score) {
            idleGaps += score;
        }
        void incrementLabPlaceScore(int score) {
            labPlacementScore += score;
        }
        void incrementDistributionScore(int score) {
            distributionScore += score;
        }

        void setTotalScore(int score) {
            totalScore = score;
        }
        void setConflict(int conflict) {
            conflicts = conflict;
        }
        void setExplanation(string explain) {
            explanation = explain;
        }

        int getConflicts() {
            return conflicts;
        }
        int getTotalScore() {
            return totalScore;
        }
        int getIdleGaps() {
            return idleGaps;
        }
        int getLabPlaceScore() {
            return labPlacementScore;
        }
        int getDistributionScore() {
            return distributionScore;
        }
        string getExplanation() {
            return explanation;
        }
};

#endif