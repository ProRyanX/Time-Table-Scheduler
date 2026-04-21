import ctypes
from ctypes import c_char_p, c_int, c_bool

lib = ctypes.CDLL("./libscheduler.so")

lib.init_scheduler.argtypes = [c_char_p, c_char_p, c_char_p, c_char_p]
lib.init_scheduler.restype = c_bool

lib.generate_timetable.argtypes = []
lib.generate_timetable.restype = c_int

lib.export_timetable.argtypes = [c_char_p]
lib.export_timetable.restype = c_bool

lib.get_last_error.argtypes = []
lib.get_last_error.restype = c_char_p

lib.get_best_score.argtypes = []
lib.get_best_score.restype = c_int

ok = lib.init_scheduler(
    b"data/scheduler.db",
    b"data/courses.csv",
    b"data/teachers.csv",
    b"data/rooms.csv"
)

if not ok:
    print("INIT ERROR:", lib.get_last_error().decode())
    raise SystemExit(1)

score = lib.generate_timetable()
if score < 0:
    print("GENERATE ERROR:", lib.get_last_error().decode())
    raise SystemExit(1)

print("Best score:", score)

ok = lib.export_timetable(b"outputs")
if not ok:
    print("EXPORT ERROR:", lib.get_last_error().decode())
    raise SystemExit(1)

print("Export completed.")
