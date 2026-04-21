from dataclasses import dataclass

@dataclass
class TeacherDTO:
    teacher_id: str
    name: str

@dataclass
class RoomDTO:
    room_id: str
    name: str
    capacity: int

@dataclass
class CourseEventDTO:
    section_id: str
    subject_name: str
    teacher_id: str
    duration: int
    frequency: int
    is_lab: bool

