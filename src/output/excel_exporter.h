#ifndef EXCEL_EXPORTER_H
#define EXCEL_EXPORTER_H

// ================================================================
//  excel_exporter.h
//  Writes one .xlsx file per section into the outputs/ folder.
//
//  Uses: xlsxwriter (https://libxlsxwriter.github.io/)
//  Link: -lxlsxwriter
//
//  Output: outputs/SEC-A_timetable.xlsx
//          outputs/SEC-B_timetable.xlsx  etc.
//
//  Each sheet layout:
//    Row 0   : Header banner (merged, styled)
//    Row 1   : Day names (Mon–Fri)
//    Rows 2–9: Time slots (8 slots), each cell shows:
//                Subject | Room | Teacher
//    Row 10  : Score summary row
// ================================================================

#include "../../include/common.h"
#include "../../include/constants.h"
#include "heuristics.h"
#include <string>
#include <vector>

struct ScheduledEntry {
    int         day;        // 0–4
    int         slot;       // 0–7
    int         roomIndex;  // index into rooms vector
    int         eventIndex; // index into ClassEvent vector
    std::string sectionID;
};

class ExcelExporter {
public:
    ExcelExporter() = default;

    // ── Main export entry point ──────────────────────────────────
    //
    // timetable : the solved grid [MAX_ROOMS][MAX_DAYS][MAX_SLOTS]
    //             value = index into 'events', or -1 if empty
    // events    : all ClassEvents (ordered as used by solver)
    // rooms     : Room list (same order as timetable dim 0)
    // score     : TimetableScore for the selected candidate
    // outputDir : directory to write xlsx files into (e.g. "outputs")
    //
    void exportAll(
        int timetable[][MAX_DAYS][MAX_SLOTS],
        std::vector<ClassEvent>& events,
        std::vector<Room>&       rooms,
        TimetableScore&          score,
        const std::string&       outputDir = "outputs"
    );

    // Export a single section.
    // sectionID : e.g. "SEC-A"
    void exportSection(
        std::string&             sectionID,
        int timetable[][MAX_DAYS][MAX_SLOTS],
        std::vector<ClassEvent>& events,
        std::vector<Room>&       rooms,
        TimetableScore&          score,
        const std::string&       outputDir = "outputs"
    );

private:
    // Returns entries belonging to a given sectionID
    std::vector<ScheduledEntry> filterBySection(
        const std::string& sectionID,
        const int timetable[MAX_ROOMS][MAX_DAYS][MAX_SLOTS],
        const std::vector<ClassEvent>& events
    ) const;

    // Collect all unique section IDs from the event list
    std::vector<std::string> collectSections(
        const std::vector<ClassEvent>& events
    ) const;

    // Creates output directory if it doesn't exist
    void ensureDir(const std::string& dir) const;
};

#endif // EXCEL_EXPORTER_H