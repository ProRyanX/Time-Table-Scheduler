import requests
from typing import Dict, Any

class SchedulerClient:
    """Flask API client for GUI"""
    
    def __init__(self, base_url="http://127.0.0.1:5000"):
        self.base_url = base_url
        self.timeout = 30
    
    def init_scheduler(self, data_dir="data") -> bool:
        """Initialize backend via Flask"""
        try:
            response = requests.post(
                f"{self.base_url}/api/init",
                json={"data_dir": data_dir},
                timeout=self.timeout
            )
            
            if response.status_code == 200:
                return response.json().get("success", False)
            return False
        except requests.RequestException as e:
            print(f"Failed to connect to backend: {e}")
            return False
    
    def generate_timetable(self) -> Dict[str, Any]:
        """Generate timetable"""
        try:
            response = requests.post(
                f"{self.base_url}/api/generate",
                timeout=self.timeout
            )
            
            if response.status_code == 200:
                return response.json()
            return {"success": False, "error": f"HTTP {response.status_code}"}
        except requests.RequestException as e:
            return {"success": False, "error": str(e)}
    
    def export_timetable(self, output_dir="outputs") -> bool:
        """Export timetable"""
        try:
            response = requests.post(
                f"{self.base_url}/api/export",
                json={"output_dir": output_dir},
                timeout=self.timeout
            )
            return response.status_code == 200 and response.json().get("success", False)
        except:
            return False
    
    def get_status(self) -> Dict[str, Any]:
        """Get backend status"""
        try:
            response = requests.get(
                f"{self.base_url}/api/status",
                timeout=5
            )
            if response.status_code == 200:
                return response.json()
        except:
            pass
        return {"initialized": False, "ready": False}

# Global client instance
scheduler = SchedulerClient()
