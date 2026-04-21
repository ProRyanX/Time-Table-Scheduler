#pragma once
#include "../../include/common.h"
#include "../../include/constants.h"
#include <vector>
#include <string>

// Prints the full timetable to console, day by day, slot by slot.
// For each occupied cell: shows Subject, Section, Teacher, Room.
void printTimetable(
    const TimetableCell timetable[MAX_ROOMS][MAX_DAYS][MAX_SLOTS],
    std::vector<Room>& rooms
);

// Prints a single day's schedule across all rooms.
void printDay(
    int day,
    const TimetableCell timetable[MAX_ROOMS][MAX_DAYS][MAX_SLOTS],
    std::vector<Room>& rooms
);

// Prints a formatted header banner.
void printHeader(const std::string& title);

// Converts a slot index to a human-readable time string.
std::string slotToTime(int slot);