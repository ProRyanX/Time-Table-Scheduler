import ctypes
import os

BASE_DIR = os.path.dirname(os.path.abspath(__file__))

lib_path = os.path.join(BASE_DIR, "..", "..", "..", "libscheduler.dylib")
lib_path = os.path.abspath(lib_path)

print("Loading dylib from:", lib_path)  # optional debug

scheduler = ctypes.CDLL(lib_path)

scheduler.init_scheduler.argtypes = [
    ctypes.c_char_p,
    ctypes.c_char_p,
    ctypes.c_char_p,
    ctypes.c_char_p
]
scheduler.init_scheduler.restype = ctypes.c_bool

scheduler.generate_timetable.restype = ctypes.c_int

scheduler.get_timetable_json.restype = ctypes.c_char_p
scheduler.get_score_json.restype = ctypes.c_char_p
scheduler.get_last_error.restype = ctypes.c_char_p


def init(db, courses, teachers, rooms):
    return scheduler.init_scheduler(
        db.encode(),
        courses.encode(),
        teachers.encode(),
        rooms.encode()
    )

def generate():
    return scheduler.generate_timetable()

def get_timetable():
    res = scheduler.get_timetable_json()
    return res.decode() if res else "{}"

def get_score():
    res = scheduler.get_score_json()
    return res.decode() if res else "{}"

def get_error():
    err = scheduler.get_last_error()
    return err.decode() if err else "Unknown error"