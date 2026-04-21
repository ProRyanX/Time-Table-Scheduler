#include "SchedulerEngine.h"

#include <iostream>
#include <cstring>

#include "../include/globals.h"
#include "constraints/constraint_manager.h"
#include "solver/solver.h"
#include "output/formatter.h"
#include "output/excel_exporter.h"

const char* SchedulerEngine::DB_PATH = "data/scheduler.db";
const char* SchedulerEngine::COURSES_CSV = "data/courses.csv";
const char* SchedulerEngine::TEACHERS_CSV = "data/teachers.csv";
const char* SchedulerEngine::ROOMS_CSV = "data/rooms.csv";

SchedulerEngine::SchedulerEngine()
    : db(DB_PATH), bestIdx(-1) {
}

bool SchedulerEngine::initialize() {
    if (!db.open()) {
        std::cerr << "FATAL: Cannot open database.\n";
        return false;
    }

    db.seedFromCSV(COURSES_CSV, TEACHERS_CSV, ROOMS_CSV);

    baseEvents = db.loadCourses();
    rooms = db.loadRooms();

    if (baseEvents.empty()) {
        std::cerr << "FATAL: No courses found in database.\n";
        return false;
    }

    if (rooms.empty()) {
        std::cerr << "FATAL: No rooms found in database.\n";
        return false;
    }

    orderings = generateCandidateOrderings(baseEvents, NUM_CANDIDATES);
    solutions.assign(NUM_CANDIDATES, CandidateSolution{});
    scores.clear();
    bestIdx = -1;
    selectionReason.clear();

    return true;
}

bool SchedulerEngine::generateBestTimetable() {
    if (baseEvents.empty() || rooms.empty() || orderings.empty()) {
        std::cerr << "ERROR: Engine not initialized properly.\n";
        return false;
    }

    scores.clear();
    bestIdx = -1;
    selectionReason.clear();

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
            static_cast<int>(rooms.size())
        );

        if (solutions[c].solved) {
            copyGlobalTimetableTo(solutions[c].grid);
            TimetableScore sc = evaluateTimetable(solutions[c].grid, orderings[c], static_cast<int>(rooms.size()));
            scores.push_back(sc);
        } else {
            TimetableScore sc;
            sc.setConflict(999);
            sc.setTotalScore(-9999);
            sc.setExplanation("UNSOLVED");
            scores.push_back(sc);
        }
    }

    bestIdx = selectBestTimetable(scores, selectionReason);

    if (bestIdx < 0 || bestIdx >= static_cast<int>(solutions.size()) || !solutions[bestIdx].solved) {
        std::cerr << "FATAL: No valid timetable could be generated.\n";
        return false;
    }

    copyGridToGlobal(solutions[bestIdx].grid);
    scores[bestIdx].setExplanation(selectionReason);

    return true;
}

void SchedulerEngine::printBestTimetable() const {
    if (bestIdx < 0 || bestIdx >= static_cast<int>(solutions.size()) || !solutions[bestIdx].solved) {
        std::cerr << "No best timetable available to print.\n";
        return;
    }

    TimetableCell cellGrid[MAX_ROOMS][MAX_DAYS][MAX_SLOTS];
    buildCellGrid(solutions[bestIdx].grid, orderings[bestIdx], cellGrid);
    std::vector<Room> printRooms = rooms;
    printTimetable(cellGrid, printRooms);
}

void SchedulerEngine::exportBestTimetable(const std::string& outputDir) const {
    if (bestIdx < 0 || bestIdx >= static_cast<int>(solutions.size()) || !solutions[bestIdx].solved) {
        std::cerr << "No best timetable available to export.\n";
        return;
    }

    ExcelExporter exporter;
    // exporter.exportAll(solutions[bestIdx].grid, orderings[bestIdx], rooms, scores[bestIdx], outputDir);

    int exportGrid[MAX_ROOMS][MAX_DAYS][MAX_SLOTS];
    for (int r = 0; r < MAX_ROOMS; ++r) {
        for (int d = 0; d < MAX_DAYS; ++d) {
            for (int s = 0; s < MAX_SLOTS; ++s) {
                exportGrid[r][d][s] = solutions[bestIdx].grid[r][d][s];
            }
        }
    }

    std::vector<ClassEvent> exportEvents = orderings[bestIdx];
    std::vector<Room> exportRooms = rooms;
    TimetableScore exportScore = scores[bestIdx];

    exporter.exportAll(exportGrid, exportEvents, exportRooms, exportScore, outputDir);

}

const TimetableScore& SchedulerEngine::getBestScore() const {
    return scores[bestIdx];
}

int SchedulerEngine::getBestCandidateIndex() const {
    return bestIdx;
}

void SchedulerEngine::copyGlobalTimetableTo(int dest[MAX_ROOMS][MAX_DAYS][MAX_SLOTS]) const {
    for (int r = 0; r < MAX_ROOMS; ++r) {
        for (int d = 0; d < MAX_DAYS; ++d) {
            for (int s = 0; s < MAX_SLOTS; ++s) {
                dest[r][d][s] = timetable[r][d][s];
            }
        }
    }
}

void SchedulerEngine::copyGridToGlobal(const int src[MAX_ROOMS][MAX_DAYS][MAX_SLOTS]) const {
    for (int r = 0; r < MAX_ROOMS; ++r) {
        for (int d = 0; d < MAX_DAYS; ++d) {
            for (int s = 0; s < MAX_SLOTS; ++s) {
                timetable[r][d][s] = src[r][d][s];
            }
        }
    }
}

void SchedulerEngine::buildCellGrid(
    const int src[MAX_ROOMS][MAX_DAYS][MAX_SLOTS],
    const std::vector<ClassEvent>& events,
    TimetableCell dest[MAX_ROOMS][MAX_DAYS][MAX_SLOTS]
) const {
    for (int r = 0; r < MAX_ROOMS; ++r) {
        for (int d = 0; d < MAX_DAYS; ++d) {
            for (int s = 0; s < MAX_SLOTS; ++s) {
                int idx = src[r][d][s];

                if (idx >= 0 && idx < static_cast<int>(events.size())) {
                    const ClassEvent& e = events[idx];
                    dest[r][d][s] = TimetableCell(
                        true,
                        e.getSubjectName(),
                        e.getSectionID(),
                        e.getTeacherID(),
                        e.isLab()
                    );
                } else {
                    dest[r][d][s] = TimetableCell(false, "", "", "", false);
                }
            }
        }
    }
}
