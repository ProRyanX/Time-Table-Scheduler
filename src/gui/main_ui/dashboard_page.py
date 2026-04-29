import customtkinter as ctk
from src.gui.main_ui.widgets import SectionTitle, StatCard

class DashboardPage(ctk.CTkFrame):
    def __init__(self, master, api):
        super().__init__(master)
        self.api = api

        SectionTitle(self, "Dashboard").grid(row=0, column=0, padx=20, pady=(20, 10), sticky="w")

        self.card_frame = ctk.CTkFrame(self, fg_color="transparent")
        self.card_frame.grid(row=1, column=0, padx=20, pady=10, sticky="ew")
        for i in range(4):
            self.card_frame.grid_columnconfigure(i, weight=1)

        self.cards = [
            StatCard(self.card_frame, "Teachers", 0),
            StatCard(self.card_frame, "Rooms", 0),
            StatCard(self.card_frame, "Events", 0),
            StatCard(self.card_frame, "Sections", 0),
        ]
        for i, card in enumerate(self.cards):
            card.grid(row=0, column=i, padx=8, pady=8, sticky="ew")

        self.info_box = ctk.CTkTextbox(self, height=260)
        self.info_box.grid(row=2, column=0, padx=20, pady=10, sticky="nsew")
        self.grid_rowconfigure(2, weight=1)
        self.grid_columnconfigure(0, weight=1)

        self.refresh()

    def refresh(self):
        teachers = self.api.get_teachers()
        rooms = self.api.get_rooms()
        events = self.api.get_events()
        sections = len(set(e["section_id"] for e in events))

        values = [len(teachers), len(rooms), len(events), sections]
        titles = ["Teachers", "Rooms", "Events", "Sections"]
        for card, title, value in zip(self.cards, titles, values):
            for widget in card.winfo_children():
                widget.destroy()
            ctk.CTkLabel(card, text=title, font=ctk.CTkFont(size=13)).grid(
                row=0, column=0, padx=16, pady=(14, 4), sticky="w"
            )
            ctk.CTkLabel(card, text=str(value), font=ctk.CTkFont(size=26, weight="bold")).grid(
                row=1, column=0, padx=16, pady=(0, 14), sticky="w"
            )

        self.info_box.delete("1.0", "end")
        self.info_box.insert("end",
            "Welcome to the Timetable Scheduler UI.\n\n"
            "Use the sidebar to manage teachers, rooms, and events.\n"
            "Then open the Generate page to run the scheduling engine.\n"
            "Finally, check Results for scores, timetable preview, and export status."
        )
