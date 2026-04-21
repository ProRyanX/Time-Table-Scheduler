#include "csv_parser.h"
#include <fstream>
#include <sstream>
#include <iostream>

string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    size_t last = str.find_last_not_of(" \t\r\n");
    if (first == string::npos) return "";
    return str.substr(first, last - first + 1);
}

// Read Courses CSV
vector<Course> readCourses(string filename) {
    vector<Course> courses;
    ifstream file(filename);
    string line;

    getline(file, line);

    while (getline(file, line)) {
        stringstream ss(line);      // A stringstream object created for 'line'. Here, ("ID,Name,Teacher_ID,Lectures_Per_Week")

        string temp, id, teacher_id, section_id;
        int lectures_per_week, duration;
        string name;
        int islab;

        getline(ss, id, ',');     // Gets the substring until the character ','

        getline(ss,name,',');       // Stores the next substring until the character ','

        getline(ss, teacher_id, ',');

        getline(ss, section_id, ',');

        getline(ss, temp, ',');   
        temp = trim(temp);
        if(temp.empty()) { continue; }
        lectures_per_week = stoi(temp);

        getline(ss, temp, ',');
        temp = trim(temp);
        if (temp.empty()) { continue; }
        duration = stoi(temp);

        getline(ss, temp, '\n');
        temp = trim(temp);
        if (temp.empty()) { continue; }
        islab = stoi(temp);

        Course C(id, name, teacher_id, section_id, lectures_per_week, duration, islab);
        courses.push_back(C);
    }
    return courses;
}

// Read Teachers CSV
vector<Teacher> readTeachers(string filename) {
    vector<Teacher> teachers;
    ifstream file(filename);
    string line;

    getline(file, line);

    while (getline(file, line)) {
        stringstream ss(line);      // A stringstream object created for 'line'. Here, ("ID,Name")

        string name, id;

        getline(ss, id, ',');     // Gets the substring until the character ','

        getline(ss, name, '\n');    // Gets the final substring in the string stream

        Teacher T(id, name);
        teachers.push_back(T);
    }
    return teachers;
}

// Read Rooms CSV
vector<Room> readRooms(string filename) {
    vector<Room> rooms;
    ifstream file(filename);
    string line;

    getline(file, line); // skip header

    while (getline(file, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string id, name, temp, roomType;

        getline(ss, id, ',');
        getline(ss, name, ',');
        getline(ss, temp, ',');
        getline(ss, roomType, '\n');

        temp = trim(temp);

        if (temp.empty()) {
            std::cout << "Skipping invalid room line: " << line << std::endl;
            continue;
        }

        int capacity = stoi(temp);

        rooms.emplace_back(id, name, capacity, trim(roomType));
    }

    return rooms;
}
