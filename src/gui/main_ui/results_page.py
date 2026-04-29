import customtkinter as ctk
from src.gui.main_ui.widgets import SectionTitle, StatCard

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
        
        # Handle case where score is an integer instead of a dict
        score = result["score"]
        if isinstance(score, int):
            vals = [score, "N/A", "N/A", "N/A", "N/A"]
        else:
            vals = [
                score.get("total_score", "N/A"),
                score.get("conflicts", "N/A"),
                score.get("idle_gaps", "N/A"),
                score.get("lab_score", "N/A"),
                score.get("distribution_score", "N/A"),
            ]
            
        titles = ["Total Score", "Conflicts", "Idle Gaps", "Lab Score", "Distribution"]
        for card, title, value in zip(self.score_cards, titles, vals):
            card.update_val(title, str(value))

        sections = []
        # If timetable data is available, populate section selector
        if "timetable" in result and isinstance(result["timetable"], dict):
            sections = list(result["timetable"].keys())
            
        if sections:
            self.section_selector.configure(values=sections)
            self.section_selector.set(sections[0])
            self.render_section(sections[0])
        else:
            self.section_selector.configure(values=["No Data Options"])
            self.section_selector.set("No Data Options")
            self.timetable_box.delete("1.0", "end")
            self.timetable_box.insert("end", "Timetable generated via C++.\nCheck the 'outputs/' folder for your excel/csv files!")

        self.export_box.delete("1.0", "end")
        if "export" in result:
            for k, v in result["export"].items():
                self.export_box.insert("end", f"{k}: {v}\n")

    def on_section_change(self, section):
        if section != "No Data Options":
            self.render_section(section)

    def render_section(self, section):
        if not self.result_data or "timetable" not in self.result_data:
            return
            
        data = self.result_data["timetable"].get(section, {})
        self.timetable_box.delete("1.0", "end")
        for day, slots in data.items():
            self.timetable_box.insert("end", f"\n=== {day} ===\n")
            for t, ev in slots.items():
                self.timetable_box.insert("end", f"  {t}: {ev}\n")