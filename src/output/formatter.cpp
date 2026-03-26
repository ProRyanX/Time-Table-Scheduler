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
