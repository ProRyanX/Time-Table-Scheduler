#ifndef DB_INTERFACE_H
#define DB_INTERFACE_H

// -----------------------------------------------------------------------
// db_interface.h
// Requires: sqlite3.h (link with -lsqlite3)
// Database file: data/scheduler.db (auto-gcreated on first run)

// Tables created automatically:
//    cources  (id TEXT, name TEXT, teacher_id TEXT,
//              freq INT, duration INT, is_lab INT)
//    teachers (id TEXT, name TEXT)
//    rooms    (id TEXT, name TEXT, capacity INT, room_type TEXT)
//
// Usage :
//    SchedulerDB db("data/scheduler.db");
//    db.open();
//    auto courses = db.loadCourses();
//    db.close();
// -----------------------------------------------------------------------

#include "../../include/common.h"
#include <vector>
#include <string>

using namespace std;

// Forward-declare sqlite3 so callers don't need to include sqlite3.h
struct sqlite3;

class SchedulerDB {
    public:
        // ----------------Lifecycle----------------
        explicit SchedulerDB(const string& dbPath = "data/db.sqlite");
        ~SchedulerDB();

        // Open connection and create tables if they don't exist.
        // Returns true on success.
        bool open();
        void close();
        bool isOpen() const;

        // ----------------Seed helpers----------------
        // Call once after open() if the DB is empty.
        void seedFromCSV(const string& coursesCSV,
                        const string& teachersCSV,
                        const string& roomCSV
        );

        // ----------------Courses----------------
        vector<ClassEvent> loadCourses() const;

        // Insert a new course. id auto-generated if empty.
        bool insertCourse(const string& id,
                        const string& name,
                        const string& teacher_id,
                        const string& section_id,
                        int freq, int duration, bool isLab
        );
        bool updateCourse(const string& id,
                        const string& name,
                        const string& teacher_id,
                        const string& section_id,
                        int freq, int duration, bool isLab
        );
        bool deleteCourse(const string& id);

        // ----------------Teachers----------------
        vector<Teacher> loadTeachers() const;
        bool insertTeacher(const string& id, const string& name);
        bool updateTeacher(const string& id, const string& name);
        bool deleteTeacher(const string& id);

        // ----------------Rooms----------------
        vector<Room> loadRooms() const;
        bool insertRoom(const string& id, const string& name,
                        int capacity, const string& roomType
        );
        bool updateRoom(const string& id, const string& name,
                        int capacity, const string& roomType
        );
        bool deleteRoom(const string& id);

        // ----------------Utility----------------
        // Returns the last SQLite error message.
        string lastError() const;

    private:
        string    m_path;
        sqlite3*  m_db = nullptr;

        bool createTables();
        bool exec(const string& sql) const;
        string genID(const string& prefix) const;
};

#endif    // DB_INTERFACE_H