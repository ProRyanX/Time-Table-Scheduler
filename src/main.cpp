#include <iostream>
#include <vector>

#include "../include/common.h"
#include "../include/constants.h"
#include "../include/globals.h"

#include "parser/csv_parser.h"
#include "constraints/constraint_manager.h"
#include "output/heuristics.h"
#include "solver/solver.h"
#include "output/formatter.h"

using namespace std;

int main() {
    vector<ClassEvent> allEvents = readRequirements("data/class_config.csv");

    vector<Course> courses = readCourses("data/class_config.csv");
    vector<Teacher> teachers = readTeachers("data/teacher_config.csv");
    vector<Room> rooms = readRooms("data/room_config.csv");

    // TODO:
    // Build allEvents from the requirements CSV model described in text.txt:
    // Section, Subject, Teacher_ID, Duration, Frequency, IsLab
    // For each row, push Frequency copies of ClassEvent into allEvents.

    // Step 3 from text.txt:
    // Initialize master constraint tables and block lunch slots.
    initializeConstraintArrays();

    // Step 3 from text.txt:
    // Place harder events first, labs before theory.
    sortMCV(allEvents);

    // Step 4 from text.txt:
    // Recursive DFS backtracking solver.
    bool success = solveTimeTable(allEvents, 0);

    if (success) {
    TimetableCell formattedTimetable[MAX_ROOMS][MAX_DAYS][MAX_SLOTS];

    // Initialize everything as empty
    for (int r = 0; r < MAX_ROOMS; ++r) {
        for (int d = 0; d < MAX_DAYS; ++d) {
            for (int s = 0; s < MAX_SLOTS; ++s) {
                formattedTimetable[r][d][s] = TimetableCell(false, "", "", "", false);
            }
        }
    }

    // Fill the formatted table using the integer IDs from your solver
    for (int r = 0; r < MAX_ROOMS; ++r) {
        for (int d = 0; d < MAX_DAYS; ++d) {
            for (int s = 0; s < MAX_SLOTS; ++s) {
                int eventIndex = timetable[r][d][s];
                if (eventIndex != -1) {
                    ClassEvent& ev = allEvents[eventIndex];
                    formattedTimetable[r][d][s] = TimetableCell(
                        true, ev.getSubjectName(), ev.getSectionID(), ev.getTeacherID(), ev.isLab()
                    );
                }
            }
        }
    }

    printTimetable(formattedTimetable, rooms);
}
    if (!success) {
        cout << "No feasible timetable could be generated.\n";
        return 1;
    }

    // Step 4 and Step 5 from text.txt:
    // Convert the internal timetable state into TimetableCell[][][]
    // before calling printTimetable().
    // This conversion step is still needed because the solver currently stores
    // an int timetable, while the formatter expects TimetableCell objects.
    //
    // printTimetable(...);

    return 0;
}