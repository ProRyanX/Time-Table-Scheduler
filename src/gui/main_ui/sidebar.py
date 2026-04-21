import customtkinter as ctk

class Sidebar(ctk.CTkFrame):
    def __init__(self, master, on_nav):
        super().__init__(master, width=220, corner_radius=0)
        self.grid_rowconfigure(8, weight=1)

        ctk.CTkLabel(
            self,
            text="Timetable Scheduler",
            font=ctk.CTkFont(size=22, weight="bold")
        ).grid(row=0, column=0, padx=20, pady=(24, 20), sticky="w")

        buttons = [
            ("Dashboard", "dashboard"),
            ("Teachers", "teachers"),
            ("Rooms", "rooms"),
            ("Events", "events"),
            ("Generate", "generate"),
            ("Results", "results"),
        ]

        self.nav_buttons = {}
        for i, (label, key) in enumerate(buttons, start=1):
            btn = ctk.CTkButton(
                self,
                text=label,
                height=40,
                corner_radius=10,
                anchor="w",
                command=lambda k=key: on_nav(k)
            )
            btn.grid(row=i, column=0, padx=16, pady=6, sticky="ew")
            self.nav_buttons[key] = btn

    def set_active(self, active_key):
        for key, btn in self.nav_buttons.items():
            btn.configure(fg_color=("gray75", "gray25") if key != active_key else ("#2563eb", "#1d4ed8"))
