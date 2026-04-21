import tkinter as tk
from tkinter import ttk
import customtkinter as ctk

class SectionTitle(ctk.CTkLabel):
    def __init__(self, master, text):
        super().__init__(master, text=text, font=ctk.CTkFont(size=22, weight="bold"))

class StatCard(ctk.CTkFrame):
    def __init__(self, master, title, value):
        super().__init__(master, corner_radius=16)
        self.grid_columnconfigure(0, weight=1)
        ctk.CTkLabel(self, text=title, font=ctk.CTkFont(size=13)).grid(
            row=0, column=0, padx=16, pady=(14, 4), sticky="w"
        )
        ctk.CTkLabel(self, text=str(value), font=ctk.CTkFont(size=26, weight="bold")).grid(
            row=1, column=0, padx=16, pady=(0, 14), sticky="w"
        )

class DataTable(ctk.CTkFrame):
    def __init__(self, master, columns):
        super().__init__(master, corner_radius=12)
        self.tree = ttk.Treeview(self, columns=columns, show="headings", height=12)
        self.vsb = ttk.Scrollbar(self, orient="vertical", command=self.tree.yview)
        self.hsb = ttk.Scrollbar(self, orient="horizontal", command=self.tree.xview)
        self.tree.configure(yscrollcommand=self.vsb.set, xscrollcommand=self.hsb.set)

        for col in columns:
            self.tree.heading(col, text=col)
            self.tree.column(col, width=140, anchor="center")

        self.tree.grid(row=0, column=0, sticky="nsew")
        self.vsb.grid(row=0, column=1, sticky="ns")
        self.hsb.grid(row=1, column=0, sticky="ew")
        self.grid_rowconfigure(0, weight=1)
        self.grid_columnconfigure(0, weight=1)

    def set_rows(self, rows):
        for item in self.tree.get_children():
            self.tree.delete(item)
        for row in rows:
            self.tree.insert("", "end", values=row)
