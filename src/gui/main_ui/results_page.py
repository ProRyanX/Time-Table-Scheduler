import customtkinter as ctk
from main_ui.widgets import SectionTitle, StatCard

class ResultsPage(ctk.CTkFrame):
    def __init__(self, master):
        super().__init__(master)
        self.result_data = None

        SectionTitle(self, "Results").grid(row=0, column=0, padx=20, pady=(20, 10), sticky="w")

        self.score_frame = ctk.CTkFrame(self, fg_color="transparent")
        self.score_frame.grid(row=1, column=0, padx=20, pady=10, sticky="ew")
        for i in range(5):
            self.score_frame.grid_columnconfigure(i, weight=1)

        self.score_cards = [
            StatCard(self.score_frame, "Total Score", "-"),
            StatCard(self.score_frame, "Conflicts", "-"),
            StatCard(self.score_frame, "Idle Gaps", "-"),
            StatCard(self.score_frame, "Lab Score", "-"),
            StatCard(self.score_frame, "Distribution", "-"),
        ]
        for i, card in enumerate(self.score_cards):
            card.grid(row=0, column=i, padx=8, pady=8, sticky="ew")

        self.section_selector = ctk.CTkOptionMenu(self, values=["No Data"], command=self.on_section_change)
        self.section_selector.grid(row=2, column=0, padx=20, pady=(10, 5), sticky="w")

        self.timetable_box = ctk.CTkTextbox(self, height=300)
        self.timetable_box.grid(row=3, column=0, padx=20, pady=10, sticky="nsew")

        self.export_box = ctk.CTkTextbox(self, height=100)
        self.export_box.grid(row=4, column=0, padx=20, pady=(0, 20), sticky="ew")

        self.grid_rowconfigure(3, weight=1)
        self.grid_columnconfigure(0, weight=1)

    def load_result(self, result):
        self.result_data = result
        score = result["score"]

        vals = [
            score["total_score"],
            score["conflicts"],
            score["idle_gaps"],
            score["lab_score"],
            score["distribution_score"],
        ]
        titles = ["Total Score", "Conflicts", "Idle Gaps", "Lab Score", "Distribution"]
        for card, title, value in zip(self.score_cards, titles, vals):
            for widget in card.winfo_children():
                widget.destroy()
            ctk.CTkLabel(card, text=title, font=ctk.CTkFont(size=13)).grid(
                row=0, column=0, padx=16, pady=(14, 4), sticky="w"
            )
            ctk.CTkLabel(card, text=str(value), font=ctk.CTkFont(size=26, weight="bold")).grid(
                row=1, column=0, padx=16, pady=(0, 14), sticky="w"
            )

        sections = list(result["timetable"].keys()) or ["No Data"]
        self.section_selector.configure(values=sections)
        self.section_selector.set(sections[0])
        self.render_section(sections[0])

        self.export_box.delete("1.0", "end")
        self.export_box.insert("end", "Generated export files:\n")
        for path in result["output_files"]:
            self.export_box.insert("end", f"- {path}\n")

    def on_section_change(self, section):
        self.render_section(section)

    def render_section(self, section):
        self.timetable_box.delete("1.0", "end")
        if not self.result_data:
            return

        data = self.result_data["timetable"].get(section, {})
        for day, slots in data.items():
            self.timetable_box.insert("end", f"{day}\n")
            self.timetable_box.insert("end", "-" * 42 + "\n")
            slot_keys = [f"slot{i}" for i in range(8)]
            for i, ts in enumerate(slot_keys):
                value = slots.get(ts, "")
                label = f"Slot {i+1}"
                self.timetable_box.insert("end", f"{label}: {value if value else 'Free'}\n")
            self.timetable_box.insert("end", "\n")
