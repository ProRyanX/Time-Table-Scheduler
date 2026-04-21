#include <string>
#include <vector>
#include <cstring>
#include <exception>
#include <iostream>
#include <sstream>
#include <unordered_set>
#include "../include/common.h"
#include "../include/constants.h"
#include "../include/globals.h"
#include "../include/scheduler_api.h"
#include "constraints/constraint_manager.h"
#include "output/heuristics.h"
#include "output/formatter.h"
#include "output/excel_exporter.h"
#include "parser/db_interface.h"
#include "solver/solver.h"

using namespace std;

static string g_db_path;
static string g_courses_csv;
static string g_teachers_csv;
static string g_rooms_csv;
static string g_last_error;
static int g_best_score = -9999;

static const int NUM_CANDIDATES = 3;

struct CandidateSolution {
    int grid[MAX_ROOMS][MAX_DAYS][MAX_SLOTS];
    bool solved;
};

static vector<ClassEvent> g_best_events;
static vector<Room> g_rooms;
extern TimetableScore g_best_timetable_score;
static bool g_has_solution = false;

static string timetable_json;
static string score_json;
static string last_error;

extern "C" {

bool init_scheduler(const char* db_path,
                    const char* courses_csv,
                    const char* teachers_csv,
                    const char* rooms_csv) {
    try {
        g_db_path = db_path ? db_path : "";
        g_courses_csv = courses_csv ? courses_csv : "";
        g_teachers_csv = teachers_csv ? teachers_csv : "";
        g_rooms_csv = rooms_csv ? rooms_csv : "";
        g_last_error.clear();
        g_has_solution = false;

        SchedulerDB db(g_db_path);
        if (!db.open()) {
            g_last_error = "Cannot open database";
            return false;
        }

        db.seedFromCSV(g_courses_csv, g_teachers_csv, g_rooms_csv);
        db.close();
        return true;
    } catch (const exception& e) {
        g_last_error = e.what();
        return false;
    }
}

int generate_timetable() {
    try {
        g_last_error.clear();
        g_has_solution = false;
        g_best_score = -9999;

        SchedulerDB db(g_db_path);
        if (!db.open()) {
            g_last_error = "Cannot open database";
            return -1;
        }

        vector<ClassEvent> baseEvents = db.loadCourses();
        g_rooms = db.loadRooms();

        if (baseEvents.empty()) {
            g_last_error = "No courses found in database";
            db.close();
            return -1;
        }

        if (g_rooms.empty()) {
            g_last_error = "No rooms found in database";
            db.close();
            return -1;
        }

        auto orderings = generateCandidateOrderings(baseEvents, NUM_CANDIDATES);
        vector<CandidateSolution> solutions(NUM_CANDIDATES);
        vector<TimetableScore> scores;

        std::unordered_map<std::string, int> teacherMap, sectionMap, subjectMap;
        int t = 0, s = 0, sub = 0;
        for (const auto& e : baseEvents) {
            if (!teacherMap.count(e.getTeacherID()))
                teacherMap[e.getTeacherID()] = t++;
            if (!sectionMap.count(e.getSectionID()))
                sectionMap[e.getSectionID()] = s++;
            if (!subjectMap.count(e.getSubjectName()))
                subjectMap[e.getSubjectName()] = sub++;
        }

        for (int c = 0; c < NUM_CANDIDATES; ++c) {
            initializeConstraintArrays();

            solutions[c].solved = solveTimeTable(
                orderings[c], 
                0,
                teacherMap,
                sectionMap,
                subjectMap,
                static_cast<int>(g_rooms.size())
            );

            if (solutions[c].solved) {
                for (int r = 0; r < MAX_ROOMS; ++r)
                    for (int d = 0; d < MAX_DAYS; ++d)
                        for (int s = 0; s < MAX_SLOTS; ++s)
                            solutions[c].grid[r][d][s] = timetable[r][d][s];

                TimetableScore sc = evaluateTimetable(
                    solutions[c].grid, orderings[c], g_rooms.size());
                scores.push_back(sc);
            } else {
                TimetableScore sc;
                sc.setConflict(999);
                sc.setTotalScore(-9999);
                sc.setExplanation("UNSOLVED");
                scores.push_back(sc);
            }
        }

        string selectionReason;
        int bestIdx = selectBestTimetable(scores, selectionReason);

        if (bestIdx < 0 || !solutions[bestIdx].solved) {
            g_last_error = "No valid timetable could be generated";
            db.close();
            return -1;
        }

        for (int r = 0; r < MAX_ROOMS; ++r)
            for (int d = 0; d < MAX_DAYS; ++d)
                for (int s = 0; s < MAX_SLOTS; ++s)
                    timetable[r][d][s] = solutions[bestIdx].grid[r][d][s];

        g_best_events = orderings[bestIdx];
        g_best_timetable_score = TimetableScore(scores[bestIdx]);
        g_best_score = scores[bestIdx].getTotalScore();
        g_has_solution = true;

        db.close();
        return (g_best_score >= 0) ? g_best_score : 0;
    } catch (const exception& e) {
        g_last_error = e.what();
        return -1;
    }
}

bool export_timetable(const char* output_dir) {
    try {
        if (!g_has_solution) {
            g_last_error = "No generated timetable available to export";
            return false;
        }

        string out = output_dir ? output_dir : "outputs";
        ExcelExporter exporter;
        exporter.exportAll(timetable, g_best_events, g_rooms, g_best_timetable_score, out);
        return true;
    } catch (const exception& e) {
        g_last_error = e.what();
        return false;
    }
}

const char* get_last_error() {
    return g_last_error.c_str();
}

int get_best_score() {
    return g_best_score;
}

const char* get_timetable_json() {
    std::ostringstream oss;

    // safety check
    if (!g_has_solution) {
        timetable_json = "{}";
        return timetable_json.c_str();
    }

    oss << "{";

    const char* days[MAX_DAYS] = {
        "Monday","Tuesday","Wednesday","Thursday","Friday"
    };

    // 🔹 Step 1: collect all sections
    unordered_set<string> sections;
    for (const auto& e : g_best_events) {
        sections.insert(e.getSectionID());
    }

    int secCount = 0;

    // 🔹 Step 2: iterate each section
    for (const auto& sec : sections) {

        oss << "\"" << sec << "\": {";

        // 🔹 Step 3: iterate days
        for (int d = 0; d < MAX_DAYS; d++) {

            oss << "\"" << days[d] << "\": {";

            // 🔹 Step 4: iterate slots
            for (int s = 0; s < MAX_SLOTS; s++) {

                oss << "\"slot" << s << "\": ";

                std::string value = "";

                // 🔹 Step 5: scan all rooms
                for (int r = 0; r < (int)g_rooms.size(); r++) {

                    int eventIdx = timetable[r][d][s];

                    if (eventIdx == -1) continue;

                    const ClassEvent& ev = g_best_events[eventIdx];

                    // match section
                    if (ev.getSectionID() == sec) {
                        value = ev.getSubjectName() + " (" + ev.getTeacherID() + ")";
                        break; // found for this slot
                    }
                }

                oss << "\"" << value << "\"";

                if (s != MAX_SLOTS - 1) oss << ",";
            }

            oss << "}";
            if (d != MAX_DAYS - 1) oss << ",";
        }

        oss << "}";

        if (++secCount < sections.size()) oss << ",";
    }

    oss << "}";

    timetable_json = oss.str();
    return timetable_json.c_str();
}

const char* get_score_json() {
    std::ostringstream oss;

    oss << "{";
    oss << "\"total_score\": " << g_best_timetable_score.getTotalScore() << ",";
    oss << "\"conflicts\": " << g_best_timetable_score.getConflicts() << ",";
    oss << "\"idle_gaps\": " << g_best_timetable_score.getIdleGaps() << ",";
    oss << "\"lab_score\": " << g_best_timetable_score.getLabPlaceScore() << ",";
    oss << "\"distribution_score\": " << g_best_timetable_score.getDistributionScore();
    oss << "}";

    score_json = oss.str();
    return score_json.c_str();
}

}
