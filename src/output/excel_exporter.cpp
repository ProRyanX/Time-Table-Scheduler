#include "excel_exporter.h"
#include <xlsxwriter.h>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <algorithm>

using namespace std;

static const char* DAY_NAMES[MAX_DAYS]  = { "Monday","Tuesday","Wednesday","Thursday","Friday" };
static const char* SLOT_TIMES[MAX_SLOTS] = {
    "08:00–09:00","09:00–10:00","10:00–11:00","11:00–12:00",
    "12:00–13:00 (LUNCH)",
    "13:00–14:00","14:00–15:00","15:00–16:00"
};

// ================================================================
//  ensureDir()
// ================================================================
void ExcelExporter::ensureDir(const string& dir) const {
#if defined(_WIN32)
    _mkdir(dir.c_str());
#else
    mkdir(dir.c_str(), 0755);
#endif
}

// ================================================================
//  collectSections()
// ================================================================
vector<string> ExcelExporter::collectSections(
    const vector<ClassEvent>& events) const
{
    vector<string> sections;
    for (const auto& e : events) {
        // ClassEvent stores sectionID as its first field
        const string& sec = e.getSectionID(); // uses getter from common.h
        if (find(sections.begin(), sections.end(), sec) == sections.end())
            sections.push_back(sec);
    }
    if (sections.empty()) sections.push_back("SEC-A"); // fallback
    return sections;
}

// ================================================================
//  filterBySection()
// ================================================================
vector<ScheduledEntry> ExcelExporter::filterBySection(
    const string& sectionID,
    const int timetable[][MAX_DAYS][MAX_SLOTS],
    const vector<ClassEvent>& events) const
{
    vector<ScheduledEntry> result;
    for (int r = 0; r < MAX_ROOMS; ++r) {
        for (int d = 0; d < MAX_DAYS; ++d) {
            for (int s = 0; s < MAX_SLOTS; ++s) {
                int idx = timetable[r][d][s];
                if (idx < 0 || idx >= (int)events.size()) continue;
                if (events[idx].getSectionID() == sectionID) {
                    result.push_back({ d, s, r, idx, sectionID });
                }
            }
        }
    }
    return result;
}

// ================================================================
//  exportAll()
// ================================================================
void ExcelExporter::exportAll(
    int timetable[MAX_ROOMS][MAX_DAYS][MAX_SLOTS],
    vector<ClassEvent>& events,
    vector<Room>&       rooms,
    TimetableScore&     score,
    const string&       outputDir)
{
    ensureDir(outputDir);
    auto sections = collectSections(events);
    for (auto& sec : sections) {
        exportSection(sec, timetable, events, rooms, score, outputDir);
    }
    cout << "[EXPORT] Wrote " << sections.size()
        << " timetable(s) to " << outputDir << "/\n";
}

// ================================================================
//  exportSection()
// ================================================================
void ExcelExporter::exportSection(
    string& sectionID,
    int timetable[][MAX_DAYS][MAX_SLOTS],
    vector<ClassEvent>& events,
    vector<Room>&       rooms,
    TimetableScore&     score,
    const string&             outputDir)
{
    ensureDir(outputDir);

    string filename = outputDir + "/" + sectionID + "_timetable.xlsx";
    lxw_workbook* wb = workbook_new(filename.c_str());
    if (!wb) {
        cerr << "[EXPORT] Failed to create workbook: " << filename << "\n";
        return;
    }
    lxw_worksheet* ws = workbook_add_worksheet(wb, sectionID.c_str());

    // ── Define formats ───────────────────────────────────────────
    // Title
    lxw_format* fmtTitle = workbook_add_format(wb);
    format_set_bold(fmtTitle);
    format_set_font_size(fmtTitle, 16);
    format_set_align(fmtTitle, LXW_ALIGN_CENTER);
    format_set_align(fmtTitle, LXW_ALIGN_VERTICAL_CENTER);
    format_set_fg_color(fmtTitle, 0x1F3864);
    format_set_font_color(fmtTitle, LXW_COLOR_WHITE);
    format_set_border(fmtTitle, LXW_BORDER_THIN);

    // Day header
    lxw_format* fmtDay = workbook_add_format(wb);
    format_set_bold(fmtDay);
    format_set_align(fmtDay, LXW_ALIGN_CENTER);
    format_set_fg_color(fmtDay, 0x2E75B6);
    format_set_font_color(fmtDay, LXW_COLOR_WHITE);
    format_set_border(fmtDay, LXW_BORDER_THIN);

    // Time slot label
    lxw_format* fmtSlot = workbook_add_format(wb);
    format_set_bold(fmtSlot);
    format_set_align(fmtSlot, LXW_ALIGN_CENTER);
    format_set_align(fmtSlot, LXW_ALIGN_VERTICAL_CENTER);
    format_set_fg_color(fmtSlot, 0xD6DCE4);
    format_set_border(fmtSlot, LXW_BORDER_THIN);
    format_set_text_wrap(fmtSlot);

    // Normal cell
    lxw_format* fmtCell = workbook_add_format(wb);
    format_set_align(fmtCell, LXW_ALIGN_CENTER);
    format_set_align(fmtCell, LXW_ALIGN_VERTICAL_CENTER);
    format_set_border(fmtCell, LXW_BORDER_THIN);
    format_set_text_wrap(fmtCell);

    // Lab cell (green tint)
    lxw_format* fmtLab = workbook_add_format(wb);
    format_set_align(fmtLab, LXW_ALIGN_CENTER);
    format_set_align(fmtLab, LXW_ALIGN_VERTICAL_CENTER);
    format_set_border(fmtLab, LXW_BORDER_THIN);
    format_set_text_wrap(fmtLab);
    format_set_fg_color(fmtLab, 0xE2EFDA);

    // Lunch cell
    lxw_format* fmtLunch = workbook_add_format(wb);
    format_set_bold(fmtLunch);
    format_set_align(fmtLunch, LXW_ALIGN_CENTER);
    format_set_align(fmtLunch, LXW_ALIGN_VERTICAL_CENTER);
    format_set_fg_color(fmtLunch, 0xFFF2CC);
    format_set_border(fmtLunch, LXW_BORDER_THIN);

    // Score row
    lxw_format* fmtScore = workbook_add_format(wb);
    format_set_italic(fmtScore);
    format_set_font_color(fmtScore, 0x595959);
    format_set_border(fmtScore, LXW_BORDER_THIN);

    // ── Column widths ────────────────────────────────────────────
    worksheet_set_column(ws, 0, 0, 22, nullptr);    // time column
    for (int d = 0; d < MAX_DAYS; ++d)
        worksheet_set_column(ws, d+1, d+1, 24, nullptr);

    // ── Row 0: Title banner ──────────────────────────────────────
    worksheet_set_row(ws, 0, 30, nullptr);
    worksheet_merge_range(ws, 0, 0, 0, MAX_DAYS,
        ("TIMETABLE — " + sectionID + "  |  Timetable Scheduler v2.0").c_str(),
        fmtTitle);

    // ── Row 1: Day headers ───────────────────────────────────────
    worksheet_write_string(ws, 1, 0, "Time Slot", fmtDay);
    for (int d = 0; d < MAX_DAYS; ++d)
        worksheet_write_string(ws, 1, d+1, DAY_NAMES[d], fmtDay);

    // ── Build lookup: (day,slot) → entry ─────────────────────────
    auto entries = filterBySection(sectionID, timetable, events);
    // Map: day*100+slot → ScheduledEntry*
    auto findEntry = [&](int day, int slot) -> const ScheduledEntry* {
        for (const auto& e : entries)
            if (e.day==day && e.slot==slot) return &e;
        return nullptr;
    };

    // ── Rows 2–9: Slots ──────────────────────────────────────────
    for (int s = 0; s < MAX_SLOTS; ++s) {
        int row = s + 2;
        worksheet_set_row(ws, row, 40, nullptr);

        // Time label
        worksheet_write_string(ws, row, 0, SLOT_TIMES[s], fmtSlot);

        for (int d = 0; d < MAX_DAYS; ++d) {
            // Lunch
            if (s == 4) {
                worksheet_write_string(ws, row, d+1, "— LUNCH —", fmtLunch);
                continue;
            }
            const ScheduledEntry* e = findEntry(d, s);
            if (!e) {
                worksheet_write_string(ws, row, d+1, "", fmtCell);
                continue;
            }
            const ClassEvent& ev   = events[e->eventIndex];
            string roomName = (e->roomIndex < (int)rooms.size())
                            ? rooms[e->roomIndex].getRoomName() : "?";

            // Cell content: Subject\nRoom | Teacher
            string cell = ev.getSubjectName()
                        + "\n" + roomName
                        + " | " + ev.getTeacherID()
                        + (ev.isLab() ? "\n[LAB]" : "");

            worksheet_write_string(ws, row, d+1, cell.c_str(),
                                    ev.isLab() ? fmtLab : fmtCell);
        }
    }

    // ── Row 11: Score summary ─────────────────────────────────────
    int scoreRow = MAX_SLOTS + 3;
    worksheet_set_row(ws, scoreRow, 18, nullptr);
    ostringstream scoreLine;
    scoreLine << "Algorithm Score: " << score.getTotalScore()
            << "  |  Conflicts: " << score.getConflicts()
            << "  |  Idle Gaps: " << score.getIdleGaps()
            << "  |  Lab Score: " << score.getLabPlaceScore()
            << "  |  Distribution: " << score.getDistributionScore();
    worksheet_merge_range(ws, scoreRow, 0, scoreRow, MAX_DAYS,
                            scoreLine.str().c_str(), fmtScore);

    workbook_close(wb);
    cout << "[EXPORT] Written: " << filename << "\n";
}