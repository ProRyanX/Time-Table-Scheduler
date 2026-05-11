import customtkinter as ctk
from src.gui.main_ui.theme import setup_theme
from src.gui.main_ui.sidebar import Sidebar
from src.gui.main_ui.dashboard_page import DashboardPage
from src.gui.main_ui.teachers_page import TeachersPage
from src.gui.main_ui.rooms_page import RoomsPage
from src.gui.main_ui.events_page import EventsPage
from src.gui.main_ui.generate_page import GeneratePage
from src.gui.main_ui.results_page import ResultsPage
from src.gui.services.cpp_bridge import SchedulerClient
class TimetableApp(ctk.CTk):
    def __init__(self):
        super().__init__()
        setup_theme()

        self.title("Timetable Scheduler")
        self.geometry("1400x820")
        self.minsize(1200, 700)

        self.client = SchedulerClient()
        self.client.init_scheduler(data_dir="data")
        
        self.api = self._create_mock_api() # Temporary adapter
        self.grid_columnconfigure(1, weight=1)
        self.grid_rowconfigure(0, weight=1)

        self.sidebar = Sidebar(self, self.show_page)
        self.sidebar.grid(row=0, column=0, sticky="nsew")

        self.container = ctk.CTkFrame(self, corner_radius=0)
        self.container.grid(row=0, column=1, sticky="nsew")
        self.container.grid_rowconfigure(0, weight=1)
        self.container.grid_columnconfigure(0, weight=1)

        self.pages = {}
        self.pages["dashboard"] = DashboardPage(self.container, self.api)
        self.pages["teachers"] = TeachersPage(self.container, self.api, self.refresh_dashboard)
        self.pages["rooms"] = RoomsPage(self.container, self.api, self.refresh_dashboard)
        self.pages["events"] = EventsPage(self.container, self.api, self.refresh_dashboard)
        self.pages["generate"] = GeneratePage(self.container, self.api, self.on_generated)
        self.pages["results"] = ResultsPage(self.container)

        for page in self.pages.values():
            page.grid(row=0, column=0, sticky="nsew")

        self.show_page("dashboard")

    def show_page(self, key):
        self.pages[key].tkraise()
        self.sidebar.set_active(key)
        if key == "dashboard":
            self.refresh_dashboard()
        elif key == "events":
            self.pages["events"].refresh()

    def refresh_dashboard(self):
        self.pages["dashboard"].refresh()

    def on_generated(self, result):
        self.pages["results"].load_result(result)
        self.show_page("results")


    def _create_mock_api(self):
        """Adapter that reads and writes directly to the CSV files so C++ can see them"""
        import os
        import csv
        
        class DirectCSVAPI:
            def __init__(self, client):
                self.client = client
                self.data_dir = "data"
                self.teachers_file = os.path.join(self.data_dir, "teachers.csv")
                self.rooms_file = os.path.join(self.data_dir, "rooms.csv")
                self.courses_file = os.path.join(self.data_dir, "courses.csv")
                
            def get_teachers(self):
                if not os.path.exists(self.teachers_file): return []
                with open(self.teachers_file, mode='r', newline='', encoding='utf-8') as f:
                    reader = csv.DictReader(f)
                    return [{"teacher_id": row.get("id", ""), "name": row.get("name", "")} for row in reader]
                    
            def get_rooms(self):
                if not os.path.exists(self.rooms_file): return []
                with open(self.rooms_file, mode='r', newline='', encoding='utf-8') as f:
                    reader = csv.DictReader(f)
                    return [{"room_id": row.get("id", ""), "name": row.get("name", ""), "capacity": row.get("capacity", "")} for row in reader]
                    
            def get_events(self):
                if not os.path.exists(self.courses_file): return []
                with open(self.courses_file, mode='r', newline='', encoding='utf-8') as f:
                    reader = csv.DictReader(f)
                    return [
                        {
                            "section_id": row.get("section_id", ""),
                            "subject_name": row.get("name", ""),
                            "teacher_id": row.get("teacher_id", ""),
                            "duration": int(row.get("duration", 1)),
                            "frequency": int(row.get("lectures_per_week", 1)),
                            "is_lab": int(row.get("is_lab", 0))
                        } for row in reader
                    ]
            
            def add_teacher(self, tid, name):
                is_new = not os.path.exists(self.teachers_file)
                with open(self.teachers_file, mode='a', newline='', encoding='utf-8') as f:
                    writer = csv.writer(f)
                    if is_new: writer.writerow(["id", "name"])
                    writer.writerow([tid, name])
                return True, "Teacher added to CSV"
                
            def add_room(self, rid, name, cap):
                is_new = not os.path.exists(self.rooms_file)
                with open(self.rooms_file, mode='a', newline='', encoding='utf-8') as f:
                    writer = csv.writer(f)
                    if is_new: writer.writerow(["id", "name", "capacity", "roomType"])
                    writer.writerow([rid, name, cap, "Classroom"]) # Defaulting to Classroom
                return True, "Room added to CSV"
                
            def add_event(self, section_id, subject, teacher_id, duration, frequency, is_lab):
                is_new = not os.path.exists(self.courses_file)
                
                section_count = 1
                if not is_new:
                    with open(self.courses_file, 'r', encoding='utf-8') as f:
                        reader = csv.DictReader(f)
                        for row in reader:
                            if row.get("section_id") == section_id:
                                section_count += 1
                
                clean_section = str(section_id).replace(" ", "").upper()
                c_id = f"C_{clean_section}_{section_count}"
                
                with open(self.courses_file, mode='a', newline='', encoding='utf-8') as f:
                    writer = csv.writer(f)
                    if is_new: writer.writerow(["id", "name", "teacher_id", "section_id", "lectures_per_week", "duration", "is_lab"])
                    
                    try: dur = int(duration)
                    except: dur = 1
                    try: freq = int(frequency)
                    except: freq = 1
                    
                    writer.writerow([c_id, str(subject), str(teacher_id), str(section_id), freq, dur, 1 if is_lab else 0])
                    
                return True, f"Event {c_id} added to CSV"
                            
            def generate_timetable(self, progress_callback=None):
                import shutil
                # 1. Delete the sqlite database so C++ is FORCED to rebuild it from the CSVs it sees
                db_path = os.path.join(self.data_dir, "db.sqlite")
                
                try:
                    if os.path.exists(db_path):
                        os.remove(db_path)
                except Exception as e:
                    print(f"Warning could not delete db.sqlite: {e}")
                
                # 2. Re-initialize the Flask Backend context
                self.client.init_scheduler(data_dir=self.data_dir)
                
                # 3. Generate Timetable using the fresh data
                return self.client.generate_timetable()
                
        return DirectCSVAPI(self.client)