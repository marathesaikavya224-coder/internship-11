
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <limits>
#include <queue>
#include <stack>
#include <vector>
#include <algorithm>
using namespace std;

// ---------- CONSTANTS AND FILES ----------
const int N = 10;
const string PROFILE = "profile.txt";
const string DSA_FILE = "dsa.txt";
const string TASK_FILE = "tasks.txt";
const string PLAN_FILE = "plan.txt";
const string REVISION_FILE = "revisions.txt";

// ---------- INPUT HELPERS ----------
string getText(const string& prompt) {
    string s;
    cout << prompt;
    getline(cin >> ws, s);
    return s;
}

int getNumber(const string& prompt, int low, int high) {
    int n;
    while (true) {
        cout << prompt;
        if (cin >> n && n >= low && n <= high) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return n;
        }
        cout << "Invalid input. Enter " << low
             << " to " << high << ".\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

void pauseScreen() {
    cout << "\nPress ENTER to continue...";
    cin.get();
}

void line() {
    cout << "--------------------------------------------------\n";
}

void header() {
    cout << "\n==================================================\n";
    cout << "       STUDENT PLACEMENT PLANNER & TRACKER\n";
    cout << "==================================================\n";
}

// ---------- STUDENT PROFILE ----------
struct Student {
    string name, branch, year, goal;
};

void saveProfile(const Student& s) {
    ofstream f(PROFILE);
    f << quoted(s.name) << '\n'
      << quoted(s.branch) << '\n'
      << quoted(s.year) << '\n'
      << quoted(s.goal) << '\n';
}

bool loadProfile(Student& s) {
    ifstream f(PROFILE);
    return bool(f >> quoted(s.name) >> quoted(s.branch)
                   >> quoted(s.year) >> quoted(s.goal));
}

void editProfile(Student& s) {
    s.name = getText("Student name: ");
    s.branch = getText("Branch: ");
    s.year = getText("Year / semester: ");
    s.goal = getText("Career goal: ");
    saveProfile(s);
    cout << "Profile saved.\n";
}

// ---------- DSA TRACKER: ARRAY ----------
string topics[N] = {
    "Arrays", "Strings", "Linked Lists", "Stacks", "Queues",
    "Recursion", "Trees", "Graphs", "Greedy", "Dynamic Programming"
};
int progress[N] = {};
int solved[N] = {};

void saveDSA() {
    ofstream f(DSA_FILE);
    for (int i = 0; i < N; i++)
        f << progress[i] << ' ' << solved[i] << '\n';
}

void loadDSA() {
    ifstream f(DSA_FILE);
    for (int i = 0; i < N; i++)
        if (!(f >> progress[i] >> solved[i])) break;
}

void dsaMenu() {
    cout << "\nDSA PROGRESS\n";
    line();
    for (int i = 0; i < N; i++)
        cout << setw(2) << i + 1 << ". "
             << left << setw(25) << topics[i]
             << progress[i] << "% | Problems: "
             << solved[i] << '\n';

    int i = getNumber("Topic number to update (1-10): ", 1, N) - 1;
    progress[i] = getNumber("Completion percentage (0-100): ", 0, 100);
    solved[i] = getNumber("Problems solved (0-100000): ", 0, 100000);
    saveDSA();
    cout << "DSA progress saved.\n";
}

// ---------- TASK MANAGER: LINKED LIST ----------
struct Task {
    int id, priority, done;
    string name;
    Task* next;

    Task(int i, string n, int p)
        : id(i), priority(p), done(0), name(n), next(nullptr) {}
};

class TaskList {
    Task* head = nullptr;

public:
    ~TaskList() { clear(); }

    void clear() {
        while (head) {
            Task* t = head;
            head = head->next;
            delete t;
        }
    }

    Task* find(int id) {
        for (Task* t = head; t; t = t->next)
            if (t->id == id) return t;
        return nullptr;
    }

    void add(int id, const string& name, int priority, int done = 0) {
        if (find(id)) {
            cout << "That task ID already exists.\n";
            return;
        }
        Task* t = new Task(id, name, priority);
        t->done = done;
        if (!head) head = t;
        else {
            Task* p = head;
            while (p->next) p = p->next;
            p->next = t;
        }
    }

    void show() {
        line();
        cout << "ID   TASK                         PRIORITY  STATUS\n";
        line();
        if (!head) cout << "No tasks yet.\n";
        for (Task* t = head; t; t = t->next)
            cout << left << setw(5) << t->id
                 << setw(29) << t->name.substr(0, 28)
                 << setw(10) << (t->priority == 3 ? "High" :
                                  t->priority == 2 ? "Medium" : "Low")
                 << (t->done ? "Completed" : "Pending") << '\n';
    }

    void menu() {
        cout << "\n1. Add task\n2. View tasks\n3. Search task\n"
             << "4. Update task\n5. Delete task\n0. Back\n";
        int c = getNumber("Choice: ", 0, 5);

        if (c == 1) {
            int id = getNumber("Task ID: ", 1, 1000000);
            string name = getText("Task description: ");
            int p = getNumber("Priority (1 Low, 2 Medium, 3 High): ", 1, 3);
            add(id, name, p);
        } else if (c == 2) {
            show();
        } else if (c == 3) {
            int id = getNumber("Task ID to search: ", 1, 1000000);
            Task* t = find(id);
            if (t) cout << t->name << " | "
                        << (t->done ? "Completed" : "Pending") << '\n';
            else cout << "Task not found.\n";
        } else if (c == 4) {
            int id = getNumber("Task ID to update: ", 1, 1000000);
            Task* t = find(id);
            if (t) {
                t->done = getNumber("Status (0 Pending, 1 Completed): ", 0, 1);
                t->priority = getNumber("Priority (1-3): ", 1, 3);
                cout << "Task updated.\n";
            } else cout << "Task not found.\n";
        } else if (c == 5) {
            int id = getNumber("Task ID to delete: ", 1, 1000000);
            Task* t = head;
            Task* prev = nullptr;
            while (t && t->id != id) {
                prev = t;
                t = t->next;
            }
            if (!t) cout << "Task not found.\n";
            else {
                if (prev) prev->next = t->next;
                else head = t->next;
                delete t;
                cout << "Task deleted.\n";
            }
        }
        save();
    }

    void save() {
        ofstream f(TASK_FILE);
        for (Task* t = head; t; t = t->next)
            f << t->id << ' ' << t->priority << ' ' << t->done
              << ' ' << quoted(t->name) << '\n';
    }

    void load() {
        ifstream f(TASK_FILE);
        int id, p, done;
        string name;
        while (f >> id >> p >> done >> quoted(name))
            add(id, name, p, done);
    }

    int total() {
        int n = 0;
        for (Task* t = head; t; t = t->next) n++;
        return n;
    }

    int completed() {
        int n = 0;
        for (Task* t = head; t; t = t->next)
            if (t->done) n++;
        return n;
    }
};

// ---------- DAILY PLANNER: QUEUE ----------
queue<string> plan;

void savePlan() {
    ofstream f(PLAN_FILE);
    queue<string> copy = plan;
    while (!copy.empty()) {
        f << quoted(copy.front()) << '\n';
        copy.pop();
    }
}

void loadPlan() {
    ifstream f(PLAN_FILE);
    string s;
    while (f >> quoted(s)) plan.push(s);
}

void planMenu() {
    cout << "\nDAILY STUDY PLANNER\n";
    cout << "1. Add activity\n2. View plan\n3. Complete next activity\n0. Back\n";
    int c = getNumber("Choice: ", 0, 3);

    if (c == 1) {
        plan.push(getText("Study activity: "));
        cout << "Activity added to the queue.\n";
    } else if (c == 2) {
        if (plan.empty()) cout << "No activities planned.\n";
        else {
            queue<string> copy = plan;
            int i = 1;
            while (!copy.empty()) {
                cout << i++ << ". " << copy.front() << '\n';
                copy.pop();
            }
        }
    } else if (c == 3) {
        if (plan.empty()) cout << "The plan is empty.\n";
        else {
            cout << "Completed: " << plan.front() << '\n';
            plan.pop();
        }
    }
    savePlan();
}

// ---------- REVISION HISTORY: STACK ----------
stack<string> revisions;

void saveRevisions() {
    ofstream f(REVISION_FILE);
    stack<string> copy = revisions;
    vector<string> items;
    while (!copy.empty()) {
        items.push_back(copy.top());
        copy.pop();
    }
    for (const string& s : items) f << quoted(s) << '\n';
}

void loadRevisions() {
    ifstream f(REVISION_FILE);
    vector<string> items;
    string s;
    while (f >> quoted(s)) items.push_back(s);
    for (auto it = items.rbegin(); it != items.rend(); ++it)
        revisions.push(*it);
}

void revisionMenu() {
    cout << "\nREVISION HISTORY\n";
    cout << "1. Record studied topic\n2. View latest first\n3. Remove latest\n0. Back\n";
    int c = getNumber("Choice: ", 0, 3);

    if (c == 1) {
        revisions.push(getText("Topic studied: "));
        cout << "Revision recorded.\n";
    } else if (c == 2) {
        stack<string> copy = revisions;
        if (copy.empty()) cout << "No revisions recorded.\n";
        while (!copy.empty()) {
            cout << "- " << copy.top() << '\n';
            copy.pop();
        }
    } else if (c == 3) {
        if (revisions.empty()) cout << "History is empty.\n";
        else {
            cout << "Removed: " << revisions.top() << '\n';
            revisions.pop();
        }
    }
    saveRevisions();
}

// ---------- DASHBOARD ----------
void dashboard(const Student& s, TaskList& tasks) {
    header();
    cout << "Welcome, " << s.name << '\n';
    cout << "Branch: " << s.branch << " | Year: " << s.year << '\n';
    cout << "Career goal: " << s.goal << '\n';
    line();

    int total = 0;
    for (int i = 0; i < N; i++) total += progress[i];

    cout << "DSA average progress : " << total / N << "%\n";
    cout << "Tasks completed      : " << tasks.completed()
         << " / " << tasks.total() << '\n';
    cout << "Study activities     : " << plan.size() << '\n';
    cout << "Revision topics      : " << revisions.size() << '\n';
}

// ---------- MAIN PROGRAM ----------
int main() {
    Student student;
    TaskList tasks;

    loadDSA();
    tasks.load();
    loadPlan();
    loadRevisions();

    if (!loadProfile(student)) {
        header();
        cout << "STUDENT PROFILE SETUP\n";
        editProfile(student);
    }

    while (true) {
        header();
        cout << "Student: " << student.name << '\n';
        line();
        cout << "1. Dashboard\n";
        cout << "2. DSA Progress Tracker\n";
        cout << "3. Task Manager\n";
        cout << "4. Daily Study Planner\n";
        cout << "5. Revision History\n";
        cout << "6. Edit Profile\n";
        cout << "0. Save and Exit\n";
        line();

        int c = getNumber("Enter choice: ", 0, 6);

        if (c == 0) break;
        if (c == 1) dashboard(student, tasks);
        if (c == 2) dsaMenu();
        if (c == 3) tasks.menu();
        if (c == 4) planMenu();
        if (c == 5) revisionMenu();
        if (c == 6) editProfile(student);

        if (c != 0) pauseScreen();
    }

    saveProfile(student);
    saveDSA();
    tasks.save();
    savePlan();
    saveRevisions();

    cout << "\nAll progress saved. Goodbye!\n";
    return 0;
}
