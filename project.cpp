#include <bits/stdc++.h>
#include <filesystem>
#include <sstream>
using namespace std;


struct requirements {
    float cgpa;
    vector<string> languages;
    vector<int> experience;
    bool female;
};


void operator>> (istream &in, struct requirements &req) {
    cout << "Enter the minimum CGPA: ";
    cin >> req.cgpa;
    while(cin.fail()) {
        cout << "Please enter a valid integer" << endl;
        // error = 1;
        cin.clear();
        cin.ignore(80, '\n');
        cout << "Enter the minimum CGPA: ";
        cin >> req.cgpa;
    }
    int i;
    cout << "Enter number of languages: ";
    cin >> i;
    while(cin.fail()) {
        cout << "Please enter a valid integer" << endl;
        // error = 1;
        cin.clear();
        cin.ignore(80, '\n');
        cout << "Enter number of languages: ";
        cin >> i;
    }
    string lang;
    int exp;
    while(i--) {
        int invalid = 0;
        cout << "Enter the language and experience: ";
        cin >> lang;

        try {
            cin >> exp;
            if(exp > 100) {
                throw (exp);
            }
        }
        catch (int exp) {
            i++;
            invalid = 1;
            cout << "Invalid experience entered" << endl;
        }
        if(!invalid) {
            req.languages.push_back(lang);
            req.experience.push_back(exp);
        }
    }

    char gender;
    cout << "Enter F for only female and B for both male and female: ";
    cin >> gender;

    auto decide = [](char c) {
        if(c == 'F') {
            return true;
        }
        return false;
    };

    req.female = decide(gender) ? true : false;
    
    cout << endl << endl;
}


class Base {
    public:
        static int count;
        virtual void showDetails() = 0;
};


int Base::count = 0;


class Resume: public Base {
    string name;
    float cgpa;
    vector<string> skills;
    vector<int> experience;
    bool female;
    string path;
    int valid;
    
    friend void rankResume(struct requirements, vector<Resume>&);

    public:
        Resume(string n, float gpa, bool f, vector<string> s, vector<int> e, string p) {
            name = n;
            cgpa = gpa;
            female = f;
            skills = s;
            experience = e;
            path = p;
            valid = 2;
            count++;
        }

        void showDetails() {
            cout << "Name: " << this->name << endl;
            cout << "CGPA: " << this->cgpa << endl;
            cout << "Skills: " << endl;
            for(int i = 0; i < this->skills.size(); i++) {
                cout << this->skills[i] << ": " << this->experience[i] << endl;
            }
            cout << "File path: " << this->path << endl;
            cout << endl;
        }
};


void recruiterInput(struct requirements &req) {
    cin >> req;
}


namespace space1 {
    inline bool isFemale(string gender) {
        if(gender == "Male") {
            return false;
        }
        return true;
    }


    void splitLine(string line, vector<string>& languages, vector<int>& experience) {
        string lang;
        int exp;
        string str_exp;
        int i;
        for(i = 0; i < line.size(); i++) {
            if(line[i] == ':') {
                break;
            }
            lang.push_back(line[i]);
        }
        languages.push_back(lang);
        for(i = i + 2; i < line.size(); i++) {
            str_exp.push_back(line[i]);
        }
        exp = stoi(str_exp);
        experience.push_back(exp);
    }


    void readResume(vector<Resume>& resumes) {
        namespace fs = std::filesystem;
        char name[] = "Resume";
        fs::path currentDir = getenv(name);
        fs::current_path(currentDir);

        for(const auto& entry : fs::directory_iterator(currentDir)) {
            if (entry.is_regular_file() && entry.path().extension() == ".txt") {
                string name;
                string str_cgpa;
                float cgpa;
                string skills;
                vector<string> lang;
                vector<int> exp;
                string gender;
                bool female;
                string space;
                string path;

                string line;
                ifstream read(entry.path().filename());

                getline(read, name);

                getline(read, str_cgpa);
                cgpa = stof(str_cgpa);

                getline(read, gender);
                if(isFemale(gender)) {
                    female = true;
                }
                else {
                    female = false;
                }

                getline(read, space);
                getline(read, space);

                int k = 2;
                while(getline(read, line) && line != "") {
                    splitLine(line, lang, exp);
                }

                path = currentDir.string() + '\\' + entry.path().filename().string();

                Resume r = Resume(name, cgpa, female, lang, exp, path);
                resumes.push_back(r);
            }
        }
    }
}


int found(string s, vector<string> skills) {
    for(int i = 0; i < skills.size(); i++) {
        if(s == skills[i]) {
            return i + 1;
        }
    }
    return 0;
}


void rankResume(struct requirements req, vector<Resume>& resumes) {
    vector<Resume> all;
    vector<Resume> notAll;

    for(int i = 0; i < resumes.size(); i++) {
        if(resumes[i].cgpa < req.cgpa || (req.female == 1 && resumes[i].female != 1)) {
            resumes[i].valid = 0;
        }
        else {
            for(int j = 0; j < req.languages.size(); j++) {
                int index = found(req.languages[j], resumes[i].skills);
                if(index) {
                    index--;
                    if(req.experience[j] > resumes[i].experience[index]) {
                        resumes[i].valid = 1;
                    }
                }
                else {
                    resumes[i].valid = 0;
                    break;
                }
            }
            if(resumes[i].valid == 2) {
                all.push_back(resumes[i]);
            }
            if(resumes[i].valid == 1) {
                notAll.push_back(resumes[i]);
            }
        }
    }

    cout << "Number of applicants: " << resumes[0].count << endl << endl;

    cout << "Resumes that satisfy all criteria: " << endl;
    for(int i = 0; i < all.size(); i++) {
        all[i].showDetails();
    }
    cout << "Resumes that satisfy some criteria: " << endl;
    for(int i = 0; i < notAll.size(); i++) {
        notAll[i].showDetails();
    }
}


int main() {
    vector<Resume> resumes;
    struct requirements req;
    recruiterInput(req);
    space1::readResume(resumes);
    rankResume(req, resumes);
    return 0;
}