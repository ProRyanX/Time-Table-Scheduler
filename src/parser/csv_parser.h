#ifndef CSV_PARSER_H
#define CSV_PARSER_H

#include "../../include/common.h"

vector<Course> readCourses(string filename);
vector<Teacher> readTeachers(string filename);
vector<Room> readRooms(string filename);

#endif
