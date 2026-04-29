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
        """Temporary wrapper to fulfill UI required methods until db interface is written"""
        class StubAPI:
            def __init__(self, client):
                self.client = client
                self.teachers = []
                self.rooms = []
                self.events = []
                
            def get_teachers(self): return self.teachers
            def get_rooms(self): return self.rooms
            def get_events(self): return self.events
            
            def add_teacher(self, tid, name):
                self.teachers.append({"teacher_id": tid, "name": name})
                return True, "Teacher added"
                
            def add_room(self, rid, name, cap):
                self.rooms.append({"room_id": rid, "name": name, "capacity": cap})
                return True, "Room added"
                
            def add_event(self, **kwargs):
                self.events.append(kwargs)
                return True, "Event added"
                
            def generate_timetable(self, progress_callback=None):
                return self.client.generate_timetable()
                
        return StubAPI(self.client)