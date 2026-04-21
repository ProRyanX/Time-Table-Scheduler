#ifndef SCHEDULERENGINE_H
#define SCHEDULERENGINE_H

#include <string>
#include <vector>
#include "../include/common.h"
#include "../include/constants.h"
#include "parser/db_interface.h"
#include "output/heuristics.h"

class SchedulerEngine {
public:
    SchedulerEngine();

    bool initialize();
    bool generateBestTimetable();
    void printBestTimetable() const;
    void exportBestTimetable(const std::string& outputDir) const;

    const TimetableScore& getBestScore() const;
    int getBestCandidateIndex() const;

private:
    static const int NUM_CANDIDATES = 3;
    static const char* DB_PATH;
    static const char* COURSES_CSV;
    static const char* TEACHERS_CSV;
    static const char* ROOMS_CSV;

    struct CandidateSolution {
        int grid[MAX_ROOMS][MAX_DAYS][MAX_SLOTS];
        bool solved = false;
    };

    SchedulerDB db;
    std::vector<ClassEvent> baseEvents;
    std::vector<Room> rooms;
    std::vector<std::vector<ClassEvent>> orderings;
    std::vector<CandidateSolution> solutions;
    std::vector<TimetableScore> scores;

    int bestIdx;
    std::string selectionReason;

    void copyGlobalTimetableTo(int dest[MAX_ROOMS][MAX_DAYS][MAX_SLOTS]) const;
    void copyGridToGlobal(const int src[MAX_ROOMS][MAX_DAYS][MAX_SLOTS]) const;
    void buildCellGrid(
        const int src[MAX_ROOMS][MAX_DAYS][MAX_SLOTS],
        const std::vector<ClassEvent>& events,
        TimetableCell dest[MAX_ROOMS][MAX_DAYS][MAX_SLOTS]
    ) const;
};

#endif
