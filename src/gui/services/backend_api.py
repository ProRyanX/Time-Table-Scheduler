import time
import json
import sqlite3
from services.cpp_bridge import (
    generate,
    get_error,
    get_timetable,
    get_score
)

class BackendAPI:
    def __init__(self):
        self.db_path = "data/db.sqlite"
        self.teachers = [
            {"teacher_id": "T001", "name": "Dr. Sharma"},
            {"teacher_id": "T002", "name": "Prof. Singh"},
        ]
        self.rooms = [
            {"room_id": "R101", "name": "Classroom 101", "capacity": 60},
            {"room_id": "LAB1", "name": "Computer Lab", "capacity": 40},
        ]
        self.events = [
            {
                "section_id": "SEC-A",
                "subject_name": "DSA",
                "teacher_id": "T001",
                "duration": 1,
                "frequency": 3,
                "is_lab": False,
            },
            {
                "section_id": "SEC-A",
                "subject_name": "OS Lab",
                "teacher_id": "T002",
                "duration": 2,
                "frequency": 1,
                "is_lab": True,
            },
        ]

    def get_teachers(self):
        conn = sqlite3.connect(self.db_path)
        cursor = conn.cursor()

        cursor.execute("SELECT id, name FROM teachers")
        rows = cursor.fetchall()

        conn.close()

        return [
            {"teacher_id": r[0], "name": r[1]}
            for r in rows
        ]

    def add_teacher(self, teacher_id, name):
        self.teachers.append({"teacher_id": teacher_id, "name": name})
        return True, "Teacher added successfully"

    def get_rooms(self):
        conn = sqlite3.connect(self.db_path)
        cursor = conn.cursor()

        cursor.execute("SELECT id, name, capacity, room_type FROM rooms")
        rows = cursor.fetchall()

        conn.close()

        return [
            {
                "room_id": r[0],
                "name": r[1],
                "capacity": r[2],
                "room type": r[3]
            }
            for r in rows
        ]

    def add_room(self, room_id, name, capacity):
        self.rooms.append({"room_id": room_id, "name": name, "capacity": int(capacity)})
        return True, "Room added successfully"

    def get_events(self):
        conn = sqlite3.connect(self.db_path)
        cursor = conn.cursor()

        cursor.execute("""
            SELECT name, teacher_id, section_id,
            freq, duration, is_lab
            FROM courses
        """)

        rows = cursor.fetchall()
        conn.close()

        return [
            {
                "subject_name": r[0],
                "teacher_id": r[1],
                "section_id": r[2],
                "frequency": r[3],
                "duration": r[4],
                "is_lab": bool(r[5])
            }
            for r in rows
        ]

    def add_event(self, section_id, subject_name, teacher_id, duration, frequency, is_lab):
        self.events.append({
            "section_id": section_id,
            "subject_name": subject_name,
            "teacher_id": teacher_id,
            "duration": int(duration),
            "frequency": int(frequency),
            "is_lab": bool(is_lab),
        })
        return True, "Class event added successfully"


    def generate_timetable(self, progress_callback=None):
        try:
            if progress_callback:
                progress_callback(10)

            print("Calling C++ generate...")
            res = generate()
            print("Return code:", res)

            if progress_callback:
                progress_callback(70)

            err = get_error()
            print("C++ Error:", err)

            raw_tt = get_timetable()
            raw_score = get_score()

            print("Raw Timetable:", raw_tt)
            print("Raw Score:", raw_score)

            if res < 0:
                return {
                    "success": False,
                    "error": err
                }

            timetable = json.loads(raw_tt)
            score = json.loads(raw_score)

            if progress_callback:
                progress_callback(100)

            return {
                "success": True,
                "timetable": timetable,
                "score": score,
                "output_files": []
            }

        except Exception as e:
            print("PYTHON EXCEPTION:", str(e))
            return {
                "success": False,
                "error": str(e)
            }