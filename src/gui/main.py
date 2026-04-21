from main_ui.main_window import TimetableApp
from services.cpp_bridge import scheduler

def init_backend():
    ok = scheduler.init_scheduler(
        b"db.sqlite",
        b"courses.csv",
        b"teachers.csv",
        b"room.csv"
    )

    if not ok:
        raise RuntimeError("Failed to initialize C++ backend.")


if __name__ == "__main__":
    init_backend()

    app = TimetableApp()
    app.mainloop()