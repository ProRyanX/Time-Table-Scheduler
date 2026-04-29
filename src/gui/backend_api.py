from flask import Flask, request, jsonify
from flask_cors import CORS
import ctypes
import os
import json

app = Flask(__name__)
CORS(app)  # Allow GUI to call this API

class SchedulerBackend:
    def __init__(self):
        self.lib = None
        self.initialized = False
        
    def load_library(self):
        """Load C++ shared library"""
        # Get absolute path to the project root
        base_dir = os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))
        
        lib_paths = [
            os.path.join(base_dir, "build", "libscheduler.so"),
            os.path.join(base_dir, "build", "scheduler.dll"),
            os.path.join(base_dir, "build", "Release", "scheduler.dll")
        ]
        
        for path in lib_paths:
            if os.path.exists(path):
                self.lib = ctypes.CDLL(path)
                print(f"Loaded Native C++ API: {path}") # Just to give terminal feedback
                break
        
        if not self.lib:
            raise Exception(f"C++ library not found. Searched in: {lib_paths}")
                
        # Define function signatures (same as before)
        self.lib.init_scheduler.argtypes = [ctypes.c_char_p] * 4
        self.lib.init_scheduler.restype = ctypes.c_bool
        
        self.lib.generate_timetable.argtypes = []
        self.lib.generate_timetable.restype = ctypes.c_int
        
        self.lib.export_timetable.argtypes = [ctypes.c_char_p]
        self.lib.export_timetable.restype = ctypes.c_bool
        
        self.lib.get_last_error.argtypes = []
        self.lib.get_last_error.restype = ctypes.c_char_p
    
    def init(self, data_dir="data"):
        """Initialize with data files"""
        if not self.lib:
            self.load_library()
        
        ok = self.lib.init_scheduler(
            f"{data_dir}/db.sqlite".encode(),
            f"{data_dir}/courses.csv".encode(),
            f"{data_dir}/teachers.csv".encode(),
            f"{data_dir}/rooms.csv".encode()
        )
        
        if ok:
            self.initialized = True
        else:
            error = self.lib.get_last_error().decode()
            raise Exception(f"Init failed: {error}")
        
        return ok
    
    def generate(self):
        """Generate timetable"""
        if not self.initialized:
            raise Exception("Backend not initialized")
        
        score = self.lib.generate_timetable()
        if score < 0:
            error = self.lib.get_last_error().decode()
            raise Exception(f"Generation failed: {error}")
        
        return score
    
    def export(self, output_dir="outputs"):
        """Export to files"""
        if not self.initialized:
            raise Exception("Backend not initialized")
        
        return self.lib.export_timetable(output_dir.encode())

# Create global backend instance
backend = SchedulerBackend()

# Flask routes
@app.route('/api/init', methods=['POST'])
def init_backend():
    """Initialize the scheduler"""
    try:
        data = request.get_json() or {}
        data_dir = data.get('data_dir', 'data')
        backend.init(data_dir)
        return jsonify({"success": True, "message": "Backend initialized"})
    except Exception as e:
        return jsonify({"success": False, "error": str(e)}), 500

@app.route('/api/generate', methods=['POST'])
def generate_timetable():
    """Generate timetable"""
    try:
        score = backend.generate()
        
        # Optionally export automatically
        backend.export("outputs")
        
        return jsonify({
            "success": True,
            "score": score,
            "message": f"Timetable generated with score {score}"
        })
    except Exception as e:
        return jsonify({"success": False, "error": str(e)}), 500

@app.route('/api/export', methods=['POST'])
def export_timetable():
    """Export timetable to files"""
    try:
        data = request.get_json() or {}
        output_dir = data.get('output_dir', 'outputs')
        
        if backend.export(output_dir):
            return jsonify({
                "success": True,
                "output_dir": output_dir,
                "message": f"Exported to {output_dir}"
            })
        else:
            return jsonify({
                "success": False,
                "error": "Export failed"
            }), 500
    except Exception as e:
        return jsonify({"success": False, "error": str(e)}), 500

@app.route('/api/status', methods=['GET'])
def get_status():
    """Check backend status"""
    return jsonify({
        "initialized": backend.initialized,
        "ready": backend.initialized and backend.lib is not None
    })

@app.route('/api/health', methods=['GET'])
def health_check():
    """Health check endpoint"""
    return jsonify({"status": "healthy"})

if __name__ == '__main__':
    # Run Flask server on localhost
    app.run(host='127.0.0.1', port=5000, debug=False, threaded=True)
