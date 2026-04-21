import threading
import customtkinter as ctk
from tkinter import messagebox
from main_ui.widgets import SectionTitle
from services.backend_api import BackendAPI

class GeneratePage(ctk.CTkFrame):
    def __init__(self, master, api, on_generated):
        super().__init__(master)
        self.api = api
        self.on_generated = on_generated

        SectionTitle(self, "Generate Timetable").grid(row=0, column=0, padx=20, pady=(20, 10), sticky="w")

        card = ctk.CTkFrame(self)
        card.grid(row=1, column=0, padx=20, pady=20, sticky="nsew")
        card.grid_columnconfigure(0, weight=1)

        self.status_label = ctk.CTkLabel(card, text="Ready to generate timetable", font=ctk.CTkFont(size=16))
        self.status_label.grid(row=0, column=0, padx=20, pady=(20, 10), sticky="w")

        self.progress = ctk.CTkProgressBar(card)
        self.progress.grid(row=1, column=0, padx=20, pady=10, sticky="ew")
        self.progress.set(0)

        self.log_box = ctk.CTkTextbox(card, height=240)
        self.log_box.grid(row=2, column=0, padx=20, pady=10, sticky="nsew")

        self.generate_btn = ctk.CTkButton(card, text="Generate Now", height=42, command=self.start_generation)
        self.generate_btn.grid(row=3, column=0, padx=20, pady=(10, 20), sticky="ew")

        card.grid_rowconfigure(2, weight=1)
        self.grid_rowconfigure(1, weight=1)
        self.grid_columnconfigure(0, weight=1)

    def append_log(self, text):
        self.log_box.insert("end", text + "\n")
        self.log_box.see("end")

    def start_generation(self):
        self.generate_btn.configure(state="disabled")
        self.progress.set(0)
        self.log_box.delete("1.0", "end")
        self.status_label.configure(text="Generating timetable...")

        def task():
            def progress_callback(percent):
                self.after(0, lambda: self.progress.set(percent / 100))
                self.after(0, lambda: self.append_log(f"Progress: {percent}%"))

            result = self.api.generate_timetable(progress_callback=progress_callback)

            def done():
                self.generate_btn.configure(state="normal")
                if result["success"]:
                    self.status_label.configure(text="Generation complete")
                    self.append_log("Timetable generated successfully")
                    self.on_generated(result)
                    messagebox.showinfo("Success", "Timetable generated successfully")
                else:
                    self.status_label.configure(text="Generation failed")
                    messagebox.showerror("Error", result.get("error", "Generation failed"))

            self.after(0, done)

        threading.Thread(target=task, daemon=True).start()
