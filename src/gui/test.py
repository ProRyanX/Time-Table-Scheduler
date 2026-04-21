import json
from services.cpp_bridge import scheduler
import os

base = os.path.abspath("data")  # 👈 point to data folder

ok = scheduler.init_scheduler(
    os.path.join(base, "db.sqlite").encode(),
    os.path.join(base, "courses.csv").encode(),
    os.path.join(base, "teachers.csv").encode(),
    os.path.join(base, "rooms.csv").encode()
)

if not ok:
    print("Init failed:", scheduler.get_last_error().decode())
    exit()

# generate
res = scheduler.generate_timetable()

if res < 0:
    print("Error:", scheduler.get_last_error().decode())
else:
    timetable = json.loads(scheduler.get_timetable_json().decode())
    score = json.loads(scheduler.get_score_json().decode())

    print("Timetable:", timetable)
    print("Score:", score)