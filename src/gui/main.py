from main_ui.main_window import TimetableApp
from services.cpp_bridge import scheduler
import subprocess
import sys
import atexit
import time

flask_process = None

def start_flask_server():
    """Start Flask server as subprocess"""
    global flask_process
    
    # Start Flask server
    flask_process = subprocess.Popen(
        [sys.executable, "backend_api.py"],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        cwd="."  # Adjust path as needed
    )
    
    # Wait for server to be ready
    for _ in range(10):  # Max 5 seconds
        try:
            if scheduler.get_status().get("status") == "healthy":
                break
        except:
            pass
        time.sleep(0.5)

def init_backend():
    """Initialize via Flask API"""
    if not scheduler.init_scheduler("data"):
        raise RuntimeError("Failed to initialize C++ backend via Flask")
    
    # Optional: Generate automatically
    result = scheduler.generate_timetable()
    if not result["success"]:
        raise RuntimeError(f"Failed to generate: {result.get('error')}")
    
    print(f"Success! Score: {result['score']}")

def cleanup():
    """Stop Flask server on exit"""
    if flask_process:
        flask_process.terminate()
        flask_process.wait(timeout=5)

if __name__ == "__main__":
    try:
        # Start Flask server
        start_flask_server()
        
        # Initialize backend
        init_backend()
        
        # Run GUI
        app = TimetableApp()
        app.mainloop()
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)
    finally:
        cleanup()
