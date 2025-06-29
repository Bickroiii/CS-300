// Ricky Suarez
// CS 300 Project Two
// Southern New Hampshire University


#define WIN32_LEAN_AND_MEAN    // trim down Windows headers
#define NOMINMAX               // disable the min/max macros
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <windows.h>    // for Sleep
#include <limits>       // for ignore()


using std::cout;
using std::cin;
using std::string;
using std::vector;

// pause duration in milliseconds
constexpr int kPauseMs = 5000;

// holds all the data for one course
struct CourseData {
    string code;
    string name;
    vector<string> prereqs;
};

// binary search tree manager for CourseData
class CourseManager {
    struct TreeNode {
        CourseData info;
        TreeNode* left;
        TreeNode* right;
        // build a node from course info
        TreeNode(const CourseData& cd)
            : info(cd), left(nullptr), right(nullptr) {
        }
    };

    TreeNode* root_;
    int count_;

    // helper for in order walk
    void inOrderWalk(TreeNode* node) {
        if (!node) return;
        inOrderWalk(node->left);
        cout << node->info.code << ", " << node->info.name << "\n";
        inOrderWalk(node->right);
    }

    // helper to remove a node by key
    TreeNode* removeNode(TreeNode* node, const string& key) {
        if (!node) return nullptr;
        if (key < node->info.code) {
            node->left = removeNode(node->left, key);
        }
        else if (key > node->info.code) {
            node->right = removeNode(node->right, key);
        }
        else {
            // no children
            if (!node->left && !node->right) {
                delete node;
                node = nullptr;
                --count_;
            }
            // only left child
            else if (!node->right) {
                TreeNode* tmp = node->left;
                delete node;
                node = tmp;
                --count_;
            }
            // only right child
            else if (!node->left) {
                TreeNode* tmp = node->right;
                delete node;
                node = tmp;
                --count_;
            }
            // two children
            else {
                TreeNode* succ = node->right;
                while (succ->left) succ = succ->left;
                node->info = succ->info;
                node->right = removeNode(node->right, succ->info.code);
            }
        }
        return node;
    }

public:
    CourseManager() : root_(nullptr), count_(0) {}

    // add one course node
    void Insert(const CourseData& cd) {
        if (!root_) {
            root_ = new TreeNode(cd);
        }
        else {
            TreeNode* cur = root_;
            while (true) {
                if (cd.code < cur->info.code) {
                    if (!cur->left) {
                        cur->left = new TreeNode(cd);
                        break;
                    }
                    cur = cur->left;
                }
                else {
                    if (!cur->right) {
                        cur->right = new TreeNode(cd);
                        break;
                    }
                    cur = cur->right;
                }
            }
        }
        ++count_;
    }

    // list all courses sorted by code
    void DisplayAll() {
        inOrderWalk(root_);
    }

    // find one course or return empty record
    CourseData Search(const string& key) {
        TreeNode* cur = root_;
        while (cur) {
            if (cur->info.code == key) {
                return cur->info;
            }
            cur = (key < cur->info.code)
                ? cur->left
                : cur->right;
        }
        return CourseData{};
    }

    // remove a course by code
    void Remove(const string& key) {
        root_ = removeNode(root_, key);
    }

    int Size() const {
        return count_;
    }
};

// split a comma separated line into tokens
vector<string> splitCSV(const string& line) {
    vector<string> parts;
    string temp;
    for (char c : line) {
        if (c == ',') {
            parts.push_back(temp);
            temp.clear();
        }
        else {
            temp.push_back(c);
        }
    }
    parts.push_back(temp);
    return parts;
}

// load data from file into the tree
void loadCourses(const string& path, CourseManager& mgr) {
    std::ifstream inFile(path);
    if (!inFile) {
        cout << "Unable to open file\n";
        return;
    }

    string row;
    while (std::getline(inFile, row)) {
        auto tokens = splitCSV(row);
        if (tokens.size() < 2) {
            cout << "Bad record skipped\n";
            continue;
        }
        CourseData cd;
        cd.code = tokens[0];
        cd.name = tokens[1];
        for (size_t i = 2; i < tokens.size(); ++i) {
            if (!tokens[i].empty()) {
                cd.prereqs.push_back(tokens[i]);
            }
        }
        mgr.Insert(cd);
    }
}

// print one course in detail
void showCourse(const CourseData& cd) {
    cout << cd.code << ", " << cd.name << "\nPrerequisites: ";
    if (cd.prereqs.empty()) {
        cout << "none\n";
    }
    else {
        for (size_t i = 0; i < cd.prereqs.size(); ++i) {
            cout << cd.prereqs[i];
            if (i + 1 < cd.prereqs.size()) {
                cout << ", ";
            }
        }
        cout << "\n";
    }
}

// convert text to uppercase
void toUpper(string& s) {
    for (auto& c : s) {
        c = toupper(c);
    }
}

int main(int argc, char* argv[]) {
    string filePath;
    string query;

    // handle command line args
    switch (argc) {
    case 2:
        filePath = argv[1];
        break;
    case 3:
        filePath = argv[1];
        query = argv[2];
        break;
    default:
        filePath = "ABCU_Advising_Program_Input.csv";
    }

    CourseManager tree;
    int choice = 0;

    while (choice != 9) {
        cout << "Menu\n"
            << "1 Load Courses\n"
            << "2 List All\n"
            << "3 Find Course\n"
            << "4 Remove Course\n"
            << "9 Quit\n"
            << "Select: ";
        cin >> choice;
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
        case 1:
            loadCourses(filePath, tree);
            cout << tree.Size() << " courses loaded\n";
            Sleep(kPauseMs);
            break;

        case 2:
            tree.DisplayAll();
            cout << "Press enter to continue\n";
            cin.get();
            break;

        case 3:
            cout << "Enter course code: ";
            cin >> query;
            toUpper(query);
            {
                auto cd = tree.Search(query);
                if (!cd.code.empty()) {
                    showCourse(cd);
                }
                else {
                    cout << "Not found\n";
                }
            }
            Sleep(kPauseMs);
            break;

        case 4:
            cout << "Course code to remove: ";
            cin >> query;
            toUpper(query);
            tree.Remove(query);
            Sleep(kPauseMs);
            break;

        case 9:
            cout << "Goodbye\n";
            break;

        default:
            cout << "Invalid choice\n";
            Sleep(kPauseMs);
        }

        system("cls");
    }

    return 0;
}