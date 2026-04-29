import customtkinter as ctk
from tkinter import messagebox
from src.gui.main_ui.widgets import SectionTitle, DataTable
from src.gui.services.validation import validate_required

class EventsPage(ctk.CTkFrame):
    def __init__(self, master, api, on_refresh_dashboard):
        super().__init__(master)
        self.api = api
        self.on_refresh_dashboard = on_refresh_dashboard

        SectionTitle(self, "Manage Class Events").grid(row=0, column=0, padx=20, pady=(20, 10), sticky="w")

        form = ctk.CTkFrame(self)
        form.grid(row=1, column=0, padx=20, pady=10, sticky="ew")
        for i in range(7):
            form.grid_columnconfigure(i, weight=1)

        self.section_id = ctk.CTkEntry(form, placeholder_text="Section ID")
        self.subject_name = ctk.CTkEntry(form, placeholder_text="Subject Name")
        self.teacher_option = ctk.CTkOptionMenu(form, values=self.teacher_values())
        self.duration = ctk.CTkEntry(form, placeholder_text="Duration")
        self.frequency = ctk.CTkEntry(form, placeholder_text="Frequency")
        self.lab_switch = ctk.CTkSwitch(form, text="Lab")

        self.section_id.grid(row=0, column=0, padx=8, pady=12, sticky="ew")
        self.subject_name.grid(row=0, column=1, padx=8, pady=12, sticky="ew")
        self.teacher_option.grid(row=0, column=2, padx=8, pady=12, sticky="ew")
        self.duration.grid(row=0, column=3, padx=8, pady=12, sticky="ew")
        self.frequency.grid(row=0, column=4, padx=8, pady=12, sticky="ew")
        self.lab_switch.grid(row=0, column=5, padx=8, pady=12, sticky="w")

        ctk.CTkButton(form, text="Add Event", command=self.add_event).grid(row=0, column=6, padx=8, pady=12, sticky="ew")

        self.table = DataTable(self, ["Section", "Subject", "Teacher", "Duration", "Frequency", "Lab"])
        self.table.grid(row=2, column=0, padx=20, pady=(0, 20), sticky="nsew")

        self.grid_rowconfigure(2, weight=1)
        self.grid_columnconfigure(0, weight=1)
        self.refresh()

    def teacher_values(self):
        teachers = self.api.get_teachers()
        if not teachers:
            return ["No Teachers"]
        return [t["teacher_id"] for t in teachers]

    def refresh_teachers(self):
        self.teacher_option.configure(values=self.teacher_values())
        vals = self.teacher_values()
        if vals:
            self.teacher_option.set(vals[0])

    def add_event(self):
        fields = {
            "Section ID": self.section_id.get(),
            "Subject Name": self.subject_name.get(),
            "Teacher ID": self.teacher_option.get(),
            "Duration": self.duration.get(),
            "Frequency": self.frequency.get(),
        }
        ok, msg = validate_required(fields)
        if not ok:
            messagebox.showerror("Validation Error", msg)
            return

        ok, msg = self.api.add_event(
            self.section_id.get(),
            self.subject_name.get(),
            self.teacher_option.get(),
            self.duration.get(),
            self.frequency.get(),
            self.lab_switch.get() == 1
        )
        if ok:
            messagebox.showinfo("Success", msg)
            self.section_id.delete(0, "end")
            self.subject_name.delete(0, "end")
            self.duration.delete(0, "end")
            self.frequency.delete(0, "end")
            self.refresh()
            self.on_refresh_dashboard()

    def refresh(self):
        self.refresh_teachers()
        rows = [
            (
                e["section_id"],
                e["subject_name"],
                e["teacher_id"],
                e["duration"],
                e["frequency"],
                "Yes" if e["is_lab"] else "No",
            )
            for e in self.api.get_events()
        ]
        self.table.set_rows(rows)
