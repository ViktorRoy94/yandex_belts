#include <fstream>
#include <iostream>
#include <string>
#include <iomanip>
#include <vector>

struct Student
{
    std::string name;
    std::string surname;
    int day;
    int month;
    int year;
};

int main() {
#ifdef DEBUG
    std::ifstream in("tests/fstream_students_list_test.txt");
    std::cin.rdbuf(in.rdbuf());
#endif

    int n;
    std::cin >> n;
    std::vector<Student> students(n);
    for (auto& s : students) {
        std::cin >> s.name >> s.surname >> s.day >> s.month >> s.year;
    }

    int m;
    std::cin >> m;
    for (int i = 0; i < m; i++) {
        double num;
        std::string op;
        std::cin >> op >> num;
        num--;

        if (op == "name" && num >= 0 && num < n) {
            std::cout << students[num].name << " "
                      << students[num].surname << std::endl;
        }
        else if (op == "date" && num >= 0 && num < n) {
            std::cout << students[num].day << "."
                      << students[num].month << "."
                      << students[num].year << std::endl;
        }
        else {
            std::cout << "bad request" << std::endl;
        }
    }

    return 0;
}
