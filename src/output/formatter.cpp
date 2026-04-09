#include "formatter.h"
#include "../../include/constants.h"
#include <iostream>
#include <iomanip>
#include <string>

using namespace std;

// ----------------------------------------------------------------
// Day name lookup
// ----------------------------------------------------------------
static const string DAY_NAMES[] = {
    "Monday", "Tuesday", "Wednesday", "Thursday", "Friday"
};

// ----------------------------------------------------------------
// slotToTime()
// Assumes classes start at 08:00, each slot is 1 hour.
// Adjust START_HOUR if your schedule differs.
// ----------------------------------------------------------------
string slotToTime(int slot) {
    const int START_HOUR = 8;
    int startH = START_HOUR + slot;
    int endH = startH + 1;

    // Format as "HH:00 - HH:00"
    string start = (startH < 10 ? "0" : "") + to_string(startH) + ":00";
    string end = (endH < 10 ? "0" : "") + to_string(endH) + ":00";
    return start + "-" + end;
}

// ----------------------------------------------------------------
// printHeader()
// Prints a simple banner around a title string.
// ----------------------------------------------------------------
void printHeader(const string& title) {
    string border(60, '=');
    cout << "\n" << border << "\n";
    // Center the title
    int padding = (60 - (int)title.size())/2;
    cout << string(padding, ' ') << title << "\n";
    cout << border << "\n";
}

// ----------------------------------------------------------------
// printDay()
// Prints all slots for one day across every room
// ----------------------------------------------------------------
void printDay(
    int day,
    const TimetableCell timetable[MAX_ROOMS][MAX_DAYS][MAX_SLOTS],
    const vector<Room>& rooms
) {
    string dayName = (day < 5) ? DAY_NAMES[day] : "Day " + to_string(day);
    printHeader(dayName);

    for (int slot = 0; slot < MAX_SLOTS; slot++) {
        cout << "\n [" << slotToTime(slot) << "]\n";
        cout << " " << string(56, '-') << "\n";
        
        bool anyOccupied = false;

        for (int room = 0; room < MAX_ROOMS; room++) {
            const TimetableCell& cell = timetable[room][day][slot];

            if (cell.isOccupied()) {
                anyOccupied = true;

                // Room info
                string roomLabel = "Room " + to_string(room);
                if (room < (int)rooms.size()) {
                    roomLabel = rooms[room].getRoomID();
                }

                // Print one row per occupied cell
                cout << "  | " << left << setw(10) << roomLabel
                    << "  | Subject: " << setw(12) << cell.getSubjectName()
                    << "  | Section: " << setw(8)  << cell.getSectionID()
                    << "  | Teacher: " << setw(10) << cell.getTeacherID();

                if (cell.isLab()) {
                    cout << "[LAB]";
                }

                cout << "\n";
            }
        }

        if (!anyOccupied) {
            cout << "  | (All rooms free this slot)\n";
        }
    }

    cout << "\n";
}

// ----------------------------------------------------------------
// printTimetable()
// Top-level printer - iterates over all days.
// ----------------------------------------------------------------
void printTimetable(
    const TimetableCell timetable[MAX_ROOMS][MAX_DAYS][MAX_SLOTS],
    const vector<Room>& rooms
) {
    printHeader("TIMETABLE SCHEDULER - FINAL SCHEDULE");

    for (int day = 0; day < MAX_DAYS; day++) {
        printDay(day, timetable, rooms);
    }

    cout << string(60, '=') << "\n";
    cout << "  Schedule complete.\n";
    cout << string(60, '=') << "\n\n";
}