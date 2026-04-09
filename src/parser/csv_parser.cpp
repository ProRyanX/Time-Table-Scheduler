#include "csv_parser.h"
#include <fstream>
#include <sstream>

// Read Courses CSV
// Read Courses CSV
vector<Course> readCourses(string filename) {
    vector<Course> courses;
    ifstream file(filename);
    string line;

    // Optional: uncomment if your CSV has a header row to skip it
    // getline(file, line); 

    while (getline(file, line)) {
        if (line.empty()) continue; // Skip empty lines so the program doesn't crash
        stringstream ss(line);      

        string id, name, teacher_id, temp;
        int lectures_per_week;

        // 1. Get ID (Read directly into the string variable)
        getline(ss, id, ',');       

        // 2. Get Name (Read directly into the string variable)
        getline(ss, name, ',');       

        // 3. Get Teacher ID (Read directly into the string variable)
        getline(ss, teacher_id, ',');

        // 4. Get Lectures Per Week 
        // This one IS a number, so we use temp and stoi() here
        getline(ss, temp, '\n');    
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

    // Optional: uncomment if your CSV has a header row to skip it
    // getline(file, line); 

    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);

        string temp;
        string id;
        string name;

        getline(ss, id, ',');       // DIRECTLY get string ID (No stoi needed!)
        getline(ss, name, '\n');    

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

    // Optional: uncomment if your CSV has a header row
    // getline(file, line); 

    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);

        string temp;
        string id;
        string name;
        int capacity;

        getline(ss, id, ',');       // DIRECTLY get string ID
        getline(ss, name, ',');     

        getline(ss, temp, '\n');    
        capacity = stoi(temp);      // Capacity is still a pure number, so stoi() is correct here!

        Room R(id, name, capacity);
        rooms.push_back(R);
    }
    return rooms;
}
// Read Requirements CSV and "Explode" based on frequency
vector<ClassEvent> readRequirements(string filename) {
    vector<ClassEvent> allEvents;
    ifstream file(filename);
    string line;

    // Optional: If your CSV has a header row like "Section,Subject...", 
    // uncomment the next line to skip the first row:
    // getline(file, line); 

    while (getline(file, line)) {
        if (line.empty()) continue; // Skip empty lines

        stringstream ss(line);
        string temp;
        
        string sectionID, subjectName, teacher_ID;
        int duration, frequency;
        bool isLab;

        // 1. Section
        getline(ss, sectionID, ',');
        
        // 2. Subject
        getline(ss, subjectName, ',');
        
        // 3. Teacher ID
        getline(ss, teacher_ID, ',');
        
        // 4. Duration
        getline(ss, temp, ',');
        duration = stoi(temp);
        
        // 5. Frequency
        getline(ss, temp, ',');
        frequency = stoi(temp);
        
        // 6. IsLab (0 or 1)
        getline(ss, temp, '\n');
        isLab = (stoi(temp) == 1);

        // -------------------------------------------------------------
        // THE EXPLOSION LOGIC: 
        // If frequency is 4, loop 4 times and create 4 separate objects
        // -------------------------------------------------------------
        for (int i = 0; i < frequency; i++) {
            ClassEvent newEvent(sectionID, subjectName, teacher_ID, duration, frequency, isLab);
            allEvents.push_back(newEvent);
        }
    }
    
    return allEvents;
}