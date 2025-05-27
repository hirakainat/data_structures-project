
?#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

const int TIME_SLOTS = 8;
const int DAYS_OF_WEEK = 6;

void DisplayWelcome() {
    cout << "\n";
    cout << "             _______  __  __    _____   \n";
    cout << "            |__   __||  \\/ |   / ____|  \n";
    cout << "               | |   | \\  /|  |(___    \n";
    cout << "               | |   | |\\/||   \\___ \\   \n";
    cout << "               | |   | |  | |  ____) |  \n";
    cout << "               |_|   |_|  |_| |_____/   \n";
    cout << "=====================================================================" << endl;
    cout << "                   Welcome to Timetable Manager                    " << endl;
    cout << "               Load and manage your class schedules easily.        " << endl;
    cout << "=====================================================================" << endl;
    cout << "\n";
}

const string DAYS[DAYS_OF_WEEK] = {
    "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
};

string trim(const string& s) {
    auto a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    auto b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

string extractDay(const string& cell) {
    for (int i = 0; i < DAYS_OF_WEEK; ++i) {
        if (cell.find(DAYS[i]) != string::npos)
            return DAYS[i];
    }
    return "";
}

int getDayIndex(const string& day) {
    for (int i = 0; i < DAYS_OF_WEEK; ++i)
        if (day == DAYS[i]) return i;
    return -1;
}

string getTimeslot(int ts) {
    switch (ts) {
    case 0: return "8:30 am"; case 1: return "10:00 am"; case 2: return "11:30 am"; case 3: return "1:00 pm";
    case 4: return "2:30 pm"; case 5: return "4:00 pm"; case 6: return "5:30 pm"; case 7: return "7:00 pm";
    case 8: return "8:30 pm"; default: return "";
    }
}

struct TimetableNode {
    string data;
    TimetableNode* next;
};

struct Timetable_TimeNode {
    string data;
    Timetable_TimeNode* next;
};

class TimeTable {
public:
    string classroomName;
    TimetableNode* head;
    TimetableNode* tail;
    Timetable_TimeNode* thead;
    Timetable_TimeNode* Ttail;

    TimeTable(const string& name)
        : classroomName(name), head(nullptr), tail(nullptr),
        thead(nullptr), Ttail(nullptr)
    {}

    void addClass(const string& timeLabel, const string& course) {
        auto tnode = new Timetable_TimeNode{ timeLabel, nullptr };
        if (!thead) thead = Ttail = tnode;
        else { Ttail->next = tnode; Ttail = tnode; }

        auto cnode = new TimetableNode{ course, nullptr };
        if (!head) head = tail = cnode;
        else { tail->next = cnode; tail = cnode; }
    }

    ~TimeTable() {
        TimetableNode* cn = head;
        while (cn) { auto tmp = cn; cn = cn->next; delete tmp; }
        Timetable_TimeNode* tn = thead;
        while (tn) { auto tmp = tn; tn = tn->next; delete tmp; }
    }
};

class TTADT {
private:
    TimeTable* days[DAYS_OF_WEEK];

public:
    TTADT() {
        for (int i = 0; i < DAYS_OF_WEEK; ++i) days[i] = nullptr;
    }
    ~TTADT() {
        for (int i = 0; i < DAYS_OF_WEEK; ++i) delete days[i];
    }

    void loadTimetable(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Cannot open " << filename << endl;
            return;
        }

        string header; getline(file, header);
        string line, currentDay;
        int currentDayIndex = -1;

        while (getline(file, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string cell;

            getline(ss, cell, ',');
            string dayCell = trim(cell);
            string dayFound = extractDay(dayCell);
            if (!dayFound.empty()) {
                currentDay = dayFound;
                currentDayIndex = getDayIndex(dayFound);
                if (!days[currentDayIndex])
                    days[currentDayIndex] = new TimeTable(currentDay);
                ofstream out(currentDay + ".txt", ios::app);
                out << "\n=== " << currentDay << " ===\n";
                continue;
            }
            if (currentDayIndex < 0) continue;

            getline(ss, cell, ',');
            string classroom = trim(cell);
            if (classroom.empty()) continue;

            ofstream out(currentDay + ".txt", ios::app);
            out << "\n******************** " << classroom << " ********************\n";

            TimeTable* tt = days[currentDayIndex];
            int counter = 0;
            while (getline(ss, cell, ',') && counter < TIME_SLOTS) {
                string course = trim(cell);
                string timeSlot = getTimeslot(counter);
                if (!course.empty()) {
                    tt->addClass(timeSlot, course);
                    out << classroom << " -- " << timeSlot << ": " << course << "\n";
                }
                counter++;
            }
        }
    }

    void printDaySchedule(int dayIndex) {
        if (dayIndex < 0 || dayIndex >= DAYS_OF_WEEK) {
            cout << "Invalid day index\n"; return;
        }
        string fn = DAYS[dayIndex] + ".txt";
        ifstream f(fn);
        if (!f.is_open()) {
            cout << "Cannot open " << fn << "\n"; return;
        }
        cout << "Schedule for " << DAYS[dayIndex] << ":\n";
        string l;
        while (getline(f, l)) cout << l << "\n";
    }

    // === New functions reading the *.txt files ===

    // 1) Print course info: all instances of that course


    void printCourseInfo(const string& courseName) {
        bool found = false;

        for (int d = 0; d < DAYS_OF_WEEK; ++d) {
            ifstream f((DAYS[d] + ".txt").c_str());
            if (!f.is_open()) continue;

            string printed[100];        // buffer for up to 100 unique lines
            int printedCount = 0;

            string line;
            while (getline(f, line)) {
                if (line.find(courseName) == string::npos)
                    continue;

                // check if we've already printed this exact line
                bool already = false;
                for (int k = 0; k < printedCount; ++k) {
                    if (printed[k] == line) {
                        already = true;
                        break;
                    }
                }
                if (already)
                    continue;

                // not yet printed: record and print it
                if (printedCount < 100)
                    printed[printedCount++] = line;

                cout << "[" << DAYS[d] << "] " << line << "\n";
                found = true;
            }
            f.close();
        }

        if (!found) {
            cout << "Not found " << courseName << "\n";
        }
    }




    // 2) Determine free classrooms at given day/time
    void determineFreeClassrooms(const string& day, const string& timeSlot) {
        int di = getDayIndex(day);
        if (di < 0) { cout << "Invalid day\n"; return; }
        // build room list + mark occupied
        struct R { string name; bool occ; R* next; };
        R* rooms = nullptr;
        // first pass: collect rooms
        ifstream f1(day + ".txt");
        string line;
        while (getline(f1, line)) {
            if (line.find("********************") != string::npos) {
                // extract between stars
                auto start = line.find("**** ") + 5;
                auto end = line.find(" ****", start);
                string rn = line.substr(start, end - start);
                bool ex = false;
                for (R* p = rooms; p; p = p->next) if (p->name == rn) { ex = true; break; }
                if (!ex) {
                    auto n = new R{ rn,false,rooms };
                    rooms = n;
                }
            }
        }
        // second pass: mark occupied
        f1.clear(); f1.seekg(0);
        while (getline(f1, line)) {
            if (line.find("--") != string::npos && line.find(timeSlot) != string::npos) {
                // room is before " --"
                auto end = line.find(" --");
                string rn = line.substr(0, end);
                for (R* p = rooms; p; p = p->next)
                    if (p->name == rn) p->occ = true;
            }
        }
        // print free
        cout << "Free rooms on " << day << " at " << timeSlot << ":\n";
        bool any = false;
        for (R* p = rooms; p; p = p->next) {
            if (!p->occ) { cout << " - " << p->name << "\n"; any = true; }
        }
        if (!any) cout << "  (none)\n";
        // cleanup
        while (rooms) { auto t = rooms; rooms = rooms->next; delete t; }
    }

    // 3) Print timetable of section by reading all day files
    void printTimetableOfSection(const string& section) {
        bool found = false;

        for (int d = 0; d < DAYS_OF_WEEK; ++d) {
            ifstream f((DAYS[d] + ".txt").c_str());
            if (!f.is_open()) continue;

            string printed[100];        // buffer for up to 100 unique lines
            int printedCount = 0;

            string line;
            while (getline(f, line)) {
                if (line.find(section) == string::npos)
                    continue;

                // check if we've already printed this exact line
                bool already = false;
                for (int k = 0; k < printedCount; ++k) {
                    if (printed[k] == line) {
                        already = true;
                        break;
                    }
                }
                if (already)
                    continue;

                // not yet printed: record and print it
                if (printedCount < 100)
                    printed[printedCount++] = line;

                cout << "[" << DAYS[d] << "] " << line << "\n";
                found = true;
            }
            f.close();
        }

        if (!found) {
            cout << "No timetable found for section " << section << "\n";
        }
    }

};

int main() {
    DisplayWelcome();
    TTADT schedule;
    schedule.loadTimetable("D:/timetable.csv");
    cout << "Finished writing Monday.txt to Saturday.txt\n";
    /*char c[50];
    cout << "Enter Course name: ";
    cin.getline(c, 50);
    schedule.printCourseInfo(c);*/
    int choice;
    do {
        cout << "\nMenu:\n"
            << "1. Print course info\n"
            << "2. Determine free classrooms\n"
            << "3. Print timetable of section\n"
            << "4. Exit\n"
            << "Enter choice: ";
        cin >> choice;

        if (choice == 1) {
            char c[50];
            cout << "Enter Course name: ";
            cin.ignore();
            cin.getline(c, 50);
            schedule.printCourseInfo(c);
        }
        else if (choice == 2) {
            string d, t;
            cout << "Enter Day: ";
            cin >> d;
            cout << "Enter Time: ";
            cin >> t;
            schedule.determineFreeClassrooms(d, t);
        }
        else if (choice == 3) {
            string s; cout << "Enter Section: ";
            cin >> s;
            schedule.printTimetableOfSection(s);
        }
    } while (choice != 4);

    cout << "\nExiting the program !!! GoodBye\n";
    return 0;
}
