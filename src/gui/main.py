import subprocess
import sys
import time
from src.gui.main_ui.main_window import TimetableApp
from src.gui.services.cpp_bridge import SchedulerClient

flask_process = None
client = SchedulerClient()

def start_flask_server():
    """Start Flask server as subprocess"""
    global flask_process
    
    # Start Flask server (point to the correct file path)
    flask_process = subprocess.Popen(
        [sys.executable, "src/gui/services/backend_api.py"],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        cwd="."  # Run from project root
    )
    
    # Wait for server to be ready
    for _ in range(10):  # Max 5 seconds
        try:
            # cpp_bridge currently doesn't check health, so we just wait briefly
            status = client.get_status()
            if isinstance(status, dict) and status.get("ready") is not None:
                break
        except Exception:
            pass
        time.sleep(0.5)

def cleanup():
    """Stop Flask server on exit"""
    if flask_process:
        flask_process.terminate()
        flask_process.wait(timeout=5)

if __name__ == "__main__":
    try:
        # 1. Start backend server
        print("Starting Flask backend...")
        start_flask_server()
        
        # 2. Run GUI
        print("Starting GUI...")
        app = TimetableApp()
        app.mainloop()
        
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)
    finally:
        cleanup()