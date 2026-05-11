#include "db_interface.h"
#include "csv_parser.h"
#include <sqlite3.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <random>
#include <chrono>
#include <cstring>

using namespace std;

// -----------------------------------------------------------------------
// SchedulerDB - Constructor/Destructor
// -----------------------------------------------------------------------
SchedulerDB::SchedulerDB(const string& dbPath) : m_path(dbPath), m_db(nullptr) {}

SchedulerDB::~SchedulerDB() { close(); }

// -----------------------------------------------------------------------
// open() - connect + create schema
// -----------------------------------------------------------------------
bool SchedulerDB::open() {
    if (m_db) { return true; }  // already open

    int rc = sqlite3_open(m_path.c_str(), &m_db);
    if (rc != SQLITE_OK) {
        cerr << "[DB] Cannot open database: " << sqlite3_errmsg(m_db) << "\n";
        m_db = nullptr;
        return false;
    }
    // Enable WAL for better concurrency
    exec("PRAGMA journam_mode=WAL;");
    exec("PRAGMA foreign_keys=ON;");

    if (!createTables()) {
        cerr << "[DB] Failed to create tables.\n";
        return false;
    }
    cout << "[DB] Opened: " << m_path << "\n";
    return true;
}

void SchedulerDB::close() {
    if (m_db) {
        sqlite3_close(m_db);
        m_db = nullptr;
    }
}

bool SchedulerDB::isOpen() const { return m_db != nullptr; }

string SchedulerDB::lastError() const {
    return m_db ? sqlite3_errmsg(m_db) : "database not open";
}

// -----------------------------------------------------------------------
// createTables()
// -----------------------------------------------------------------------
bool SchedulerDB::createTables() {
    const string sql = R"(
        CREATE TABLE IF NOT EXISTS teachers (
            id      TEXT PRIMARY KEY,
            name    TEXT NOT NULL
        );
        CREATE TABLE IF NOT EXISTS rooms (
            id          TEXT PRIMARY KEY,
            name        TEXT NOT NULL,
            capacity    INTEGER NOT NULL DEFAULT 30,
            room_type   TEXT NOT NULL DEFAULT 'CLASSROOM'
        );
        CREATE TABLE IF NOT EXISTS courses (
            id          TEXT PRIMARY KEY,
            name        TEXT NOT NULL,
            teacher_id  TEXT NOT NULL,
            section_id  TEXT NOT NULL,
            freq        INTEGER NOT NULL DEFAULT 1,
            duration    INTEGER NOT NULL DEFAULT 1,
            is_lab      INTEGER NOT NULL DEFAULT 0,
            FOREIGN KEY(teacher_id) REFERENCES teachers(id)
        );
    )";
    return exec(sql);
}

// -----------------------------------------------------------------------
// exec() - runs a SQL statement with no result set
// -----------------------------------------------------------------------
bool SchedulerDB::exec(const string& sql) const {
    if(!m_db) { return false; }

    char* errMsg = nullptr;
    int rc = sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "[DB] SQL error: " << (errMsg ? errMsg : "unknown") << "\n";
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

// -----------------------------------------------------------------------
// genID() - simple unique id generator
// -----------------------------------------------------------------------
string SchedulerDB::genID(const string& prefix) const {
    auto now = chrono::high_resolution_clock::now().time_since_epoch().count();
    mt19937 rng(static_cast<unsigned>(now));
    uniform_int_distribution<int> dist(1000, 9999);
    return prefix + to_string(dist(rng));
}

// ================================================================
//  COURSES
// ================================================================
vector<ClassEvent> SchedulerDB::loadCourses() const {
    vector<ClassEvent> result;
    if(!m_db) { return result; }

    const char* sql = "SELECT section_id, name, teacher_id, freq, duration, is_lab FROM courses;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "[DB] loadCourses prepare failed: " << lastError() << "\n";
        return result;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        string section_id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        string name       = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        string teachId    = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        int    freq       = sqlite3_column_int(stmt, 3);
        int    duration   = sqlite3_column_int(stmt, 4);
        bool   isLab      = sqlite3_column_int(stmt, 5) != 0;

        // ClassEvent(sectionID, subjectName, teacher_ID, duration, frequency, Lab)
        result.emplace_back(section_id, name, teachId, duration, freq, isLab);
    }
    sqlite3_finalize(stmt);
    cout << "[DB] Loaded " << result.size() << " courses.\n";
    return result;
}

bool SchedulerDB::insertCourse(const string& idIn, const string& name,
                                const string& teacher_id, const string& section_id,
                                int freq, int duration, bool isLab) {
    if(!m_db) { return false; }
    
    string id = idIn.empty() ? genID("C") : idIn;
    const char* sql = 
        "INSERT INTO courses (id, name, teacher_id, section_id, freq, duration, is_lab) "
        "VALUES (?, ?, ?, ?, ?, ?, ?);";
    
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) { return false; }

    sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, teacher_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, section_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 5, freq);
    sqlite3_bind_int(stmt, 6, duration);
    sqlite3_bind_int(stmt, 7, isLab ? 1 : 0);

    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return ok;
}

bool SchedulerDB::updateCourse(const string& id, const string& name,
                                const string& teacher_id, const string& section_id,
                                int freq, int duration, bool isLab) {
    if (!m_db) { return false; }
    
    const char* sql = 
        "UPDATE courses "
        "SET name = ?, teacher_id = ?, section_id = ?, freq = ?, duration = ?, is_lab = ? "
        "WHERE id = ?;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) { return false; }

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, teacher_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, section_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, freq);
    sqlite3_bind_int(stmt, 5, duration);
    sqlite3_bind_int(stmt, 6, isLab ? 1 : 0);
    sqlite3_bind_text(stmt, 7, id.c_str(), -1, SQLITE_TRANSIENT);

    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return ok;
}

bool SchedulerDB::deleteCourse(const string& id) {
    if (!m_db) { return false; }

    const char* sql = "DELETE FROM courses WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) { return false; }

    sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);

    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return ok;
}

// ================================================================
//  TEACHERS
// ================================================================
vector<Teacher> SchedulerDB::loadTeachers() const {
    vector<Teacher> result;
    if (!m_db) { return result; }

    const char* sql = "SELECT id, name FROM teachers";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "DB loadTeachers prepare failed: " << lastError() << "\n";
        return result;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        string id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        result.emplace_back(id, name);
    }

    sqlite3_finalize(stmt);
    return result;
}

bool SchedulerDB::insertTeacher(const string& idIn, const string& name) {
    if (!m_db) return false;

    string id = idIn.empty() ? genID("T") : idIn;
    const char* sql = "INSERT INTO teachers (id, name) VALUES (?, ?);";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_TRANSIENT);

    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return ok;
}

bool SchedulerDB::updateTeacher(const string& id, const string& name) {
    if (!m_db) return false;

    const char* sql = "UPDATE teachers SET name = ? WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, id.c_str(), -1, SQLITE_TRANSIENT);

    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return ok;
}

bool SchedulerDB::deleteTeacher(const string& id) {
    if (!m_db) return false;

    const char* sql = "DELETE FROM teachers WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);
    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return ok;
}

// ================================================================
//  ROOMS
// ================================================================
vector<Room> SchedulerDB::loadRooms() const {
    vector<Room> result;
    if (!m_db) return result;

    const char* sql = "SELECT id, name, capacity, room_type FROM rooms";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "DB loadRooms prepare failed: " << lastError() << "\n";
        return result;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        string id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        int capacity = sqlite3_column_int(stmt, 2);
        string roomType = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        result.emplace_back(id, name, capacity, roomType);
    }

    sqlite3_finalize(stmt);
    cout << "DB: Loaded " << result.size() << " rooms.\n";
    return result;
}

bool SchedulerDB::insertRoom(const string& idIn, const string& name,
                            int capacity, const string& roomType) {
    if (!m_db) return false;

    string id = idIn.empty() ? genID("R") : idIn;
    const char* sql =
        "INSERT INTO rooms (id, name, capacity, room_type) VALUES (?, ?, ?, ?);";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, capacity);
    sqlite3_bind_text(stmt, 4, roomType.c_str(), -1, SQLITE_TRANSIENT);

    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return ok;
}

bool SchedulerDB::updateRoom(const string& id, const string& name,
                            int capacity, const string& roomType) {
    if (!m_db) return false;

    const char* sql =
        "UPDATE rooms SET name = ?, capacity = ?, room_type = ? WHERE id = ?;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, capacity);
    sqlite3_bind_text(stmt, 3, roomType.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, id.c_str(), -1, SQLITE_TRANSIENT);

    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return ok;
}

bool SchedulerDB::deleteRoom(const string& id) {
    if (!m_db) return false;

    const char* sql = "DELETE FROM rooms WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);
    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return ok;
}

// ================================================================
//  seedFromCSV() — one-time migration from old CSV files
// ================================================================
void SchedulerDB::seedFromCSV(
    const string& courses_csv,
    const string& teachers_csv,
    const string& rooms_csv
) {
    vector<Course> courses = readCourses(courses_csv);
    vector<Teacher> teachers = readTeachers(teachers_csv);
    vector<Room> rooms = readRooms(rooms_csv);

    // ❌ DO NOT read CSV again manually here

    for (auto& t : teachers) {
        insertTeacher(t.getID(), t.getName());
    }

    for (auto& r : rooms) {
        insertRoom(r.getRoomID(), r.getRoomName(), r.getCapacity(), r.getRoomType());
    }

    for (auto& c : courses) {
        insertCourse(
            c.getID(),
            c.getName(),
            c.getTeacherID(),
            c.getSectionID(),
            c.getLecturesPerWeek(),
            c.getDuration(),
            c.isLab()
        );
    }
}
