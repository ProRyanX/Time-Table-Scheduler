#ifndef SCHEDULER_API_H
#define SCHEDULER_API_H

#ifdef _WIN32
#define API_EXPORT __declspec(dllexport)
#else
#define API_EXPORT
#endif

extern "C" {
    API_EXPORT bool init_scheduler(const char* db_path,
                                const char* courses_csv,
                                const char* teachers_csv,
                                const char* rooms_csv);

    API_EXPORT int generate_timetable();
    API_EXPORT bool export_timetable(const char* output_dir);

    API_EXPORT const char* get_last_error();
    API_EXPORT int get_best_score();

    API_EXPORT const char* get_timetable_json();
    API_EXPORT const char* get_score_json();
}

#endif
