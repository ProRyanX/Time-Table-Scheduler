import customtkinter as ctk
from tkinter import messagebox
from main_ui.widgets import SectionTitle, DataTable
from services.validation import validate_required

class RoomsPage(ctk.CTkFrame):
    def __init__(self, master, api, on_refresh_dashboard):
        super().__init__(master)
        self.api = api
        self.on_refresh_dashboard = on_refresh_dashboard

        SectionTitle(self, "Manage Rooms").grid(row=0, column=0, padx=20, pady=(20, 10), sticky="w")

        top = ctk.CTkFrame(self)
        top.grid(row=1, column=0, padx=20, pady=10, sticky="ew")
        top.grid_columnconfigure((0, 1, 2, 3, 4), weight=1)

        self.room_id = ctk.CTkEntry(top, placeholder_text="Room ID")
        self.room_name = ctk.CTkEntry(top, placeholder_text="Room Name")
        self.capacity = ctk.CTkEntry(top, placeholder_text="Capacity")

        self.room_id.grid(row=0, column=0, padx=8, pady=12, sticky="ew")
        self.room_name.grid(row=0, column=1, padx=8, pady=12, sticky="ew")
        self.capacity.grid(row=0, column=2, padx=8, pady=12, sticky="ew")

        ctk.CTkButton(top, text="Add Room", command=self.add_room).grid(row=0, column=3, padx=8, pady=12, sticky="ew")
        ctk.CTkButton(top, text="Refresh", command=self.refresh).grid(row=0, column=4, padx=8, pady=12, sticky="ew")

        self.table = DataTable(self, ["Room ID", "Name", "Capacity"])
        self.table.grid(row=2, column=0, padx=20, pady=(0, 20), sticky="nsew")

        self.grid_rowconfigure(2, weight=1)
        self.grid_columnconfigure(0, weight=1)

        self.refresh()

    def add_room(self):
        fields = {
            "Room ID": self.room_id.get(),
            "Room Name": self.room_name.get(),
            "Capacity": self.capacity.get(),
        }
        ok, msg = validate_required(fields)
        if not ok:
            messagebox.showerror("Validation Error", msg)
            return

        ok, msg = self.api.add_room(self.room_id.get(), self.room_name.get(), self.capacity.get())
        if ok:
            messagebox.showinfo("Success", msg)
            self.room_id.delete(0, "end")
            self.room_name.delete(0, "end")
            self.capacity.delete(0, "end")
            self.refresh()
            self.on_refresh_dashboard()

    def refresh(self):
        rows = [(r["room_id"], r["name"], r["capacity"]) for r in self.api.get_rooms()]
        self.table.set_rows(rows)
