#include <iostream>
#include "../include/scheduler_api.h"

int main() {
    if (!init_scheduler("data/db.sqlite", "data/courses.csv", "data/teachers.csv", "data/rooms.csv")) {
        std::cerr << get_last_error() << "\n";
        return 1;
    }

    int score = generate_timetable();
    if (score < 0) {
        std::cerr << get_last_error() << "\n";
        return 1;
    }

    if (!export_timetable("outputs")) {
        std::cerr << get_last_error() << "\n";
        return 1;
    }

    std::cout << "Success, score = " << score << "\n";
    return 0;
}
