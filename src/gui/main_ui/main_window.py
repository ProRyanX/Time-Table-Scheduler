import customtkinter as ctk
from main_ui.theme import setup_theme
from main_ui.sidebar import Sidebar
from main_ui.dashboard_page import DashboardPage
from main_ui.teachers_page import TeachersPage
from main_ui.rooms_page import RoomsPage
from main_ui.events_page import EventsPage
from main_ui.generate_page import GeneratePage
from main_ui.results_page import ResultsPage
from services.backend_api import BackendAPI
from services.cpp_bridge import init

class TimetableApp(ctk.CTk):
    def __init__(self):
        super().__init__()
        setup_theme()

        self.title("Timetable Scheduler")
        self.geometry("1400x820")
        self.minsize(1200, 700)

        self.api = BackendAPI()
        init(
            "data/db.sqlite",
            "data/courses.csv",
            "data/teachers.csv",
            "data/rooms.csv"
        )

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
