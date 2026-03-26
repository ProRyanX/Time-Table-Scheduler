#include "csv_parser.h"
#include <fstream>
#include <sstream>

// Read Courses CSV
vector<Course> readCourses(string filename) {
    vector<Course> courses;
    ifstream file(filename);
    string line;

    while (getline(file, line)) {
        stringstream ss(line);      // A stringstream object created for 'line'. Here, ("ID,Name,Teacher_ID,Lectures_Per_Week")

        string temp;
        int id, teacher_id, lectures_per_week;
        string name;

        getline(ss, temp, ',');     // Gets the substring until the character ','
        id = stoi(temp);            // Converts the string number into an integer number. "98" -> 98

        getline(ss,name,',');       // Stores the next substring until the character ','

        getline(ss, temp, ',');
        teacher_id = stoi(temp);

        getline(ss, temp, '\n');    // Gets the final substring in the string stream
        lectures_per_week = stoi(temp);

        Course C(id, name, teacher_id, lectures_per_week);
        courses.push_back(C);
    }
    return courses;
}

// Read Teachers CSV
vector<Teacher> readTeachers(string filename) {
    vector<Teacher> teachers;
    ifstream file(filename);
    string line;

    while (getline(file, line)) {
        stringstream ss(line);      // A stringstream object created for 'line'. Here, ("ID,Name")

        string temp;
        int id;
        string name;

        getline(ss, temp, ',');     // Gets the substring until the character ','
        id = stoi(temp);            // Converts the string number into an integer number. "98" -> 98

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
    // while (getline(file, line)) {
    //     stringstream ss(line);
    //     string temp;
    //     Room r;
    //     getline(ss, temp, ',');
    //     r.id = stoi(temp);
    //     getline(ss, r.name, ',');
    //     getline(ss, temp, ',');
    //     r.capacity = stoi(temp);
    //     rooms.push_back(r);
    // }

    while (getline(file, line)) {
        stringstream ss(line);      // A stringstream object created for 'line'. Here, ("ID,Name,Teacher_ID,Lectures_Per_Week")

        string temp;
        int id, capacity;
        string name;

        getline(ss, temp, ',');     // Gets the substring until the character ','
        id = stoi(temp);            // Converts the string number into an integer number. "98" -> 98

        getline(ss, name, ',');     // Stores the next substring until the character ','

        getline(ss, temp, '\n');    // Gets the final substring in the string stream
        capacity = stoi(temp);

        Room R(id, name, capacity);
        rooms.push_back(R);
    }
    return rooms;
}
