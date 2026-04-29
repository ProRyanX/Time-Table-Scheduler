import customtkinter as ctk
from tkinter import messagebox
from src.gui.main_ui.widgets import SectionTitle, DataTable
from src.gui.services.validation import validate_required

class TeachersPage(ctk.CTkFrame):
    def __init__(self, master, api, on_refresh_dashboard):
        super().__init__(master)
        self.api = api
        self.on_refresh_dashboard = on_refresh_dashboard

        SectionTitle(self, "Manage Teachers").grid(row=0, column=0, padx=20, pady=(20, 10), sticky="w")

        top = ctk.CTkFrame(self)
        top.grid(row=1, column=0, padx=20, pady=10, sticky="ew")
        top.grid_columnconfigure((0, 1, 2, 3), weight=1)

        self.teacher_id = ctk.CTkEntry(top, placeholder_text="Teacher ID")
        self.teacher_name = ctk.CTkEntry(top, placeholder_text="Teacher Name")
        self.teacher_id.grid(row=0, column=0, padx=8, pady=12, sticky="ew")
        self.teacher_name.grid(row=0, column=1, padx=8, pady=12, sticky="ew")

        ctk.CTkButton(top, text="Add Teacher", command=self.add_teacher).grid(
            row=0, column=2, padx=8, pady=12, sticky="ew"
        )
        ctk.CTkButton(top, text="Refresh", command=self.refresh).grid(
            row=0, column=3, padx=8, pady=12, sticky="ew"
        )

        self.table = DataTable(self, ["Teacher ID", "Name"])
        self.table.grid(row=2, column=0, padx=20, pady=(0, 20), sticky="nsew")

        self.grid_rowconfigure(2, weight=1)
        self.grid_columnconfigure(0, weight=1)

        self.refresh()

    def add_teacher(self):
        fields = {
            "Teacher ID": self.teacher_id.get(),
            "Teacher Name": self.teacher_name.get(),
        }
        ok, msg = validate_required(fields)
        if not ok:
            messagebox.showerror("Validation Error", msg)
            return

        ok, msg = self.api.add_teacher(self.teacher_id.get(), self.teacher_name.get())
        if ok:
            messagebox.showinfo("Success", msg)
            self.teacher_id.delete(0, "end")
            self.teacher_name.delete(0, "end")
            self.refresh()
            self.on_refresh_dashboard()

    def refresh(self):
        rows = [(t["teacher_id"], t["name"]) for t in self.api.get_teachers()]
        self.table.set_rows(rows)
