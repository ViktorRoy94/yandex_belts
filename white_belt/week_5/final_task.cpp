#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <algorithm>


class Date {
public:
    Date():day(0), month(0), year(0) {}
    Date(int day, int month, int year) {
        if (month < 1 || month > 12) {
            throw std::invalid_argument("Month value is invalid: " + std::to_string(month));
        }

        if (day < 1 || day > 31) {
            throw std::invalid_argument("Day value is invalid: " + std::to_string(day));
        }

        this->day = day;
        this->month = month;
        this->year = year;
    }

    int GetYear() const {
        return year;
    }
    int GetMonth() const {
        return month;
    }
    int GetDay() const {
        return day;
    }

private:
    int day;
    int month;
    int year;
};

bool operator<(const Date& lhs, const Date& rhs) {
    if (lhs.GetYear() == rhs.GetYear()) {
        if (lhs.GetMonth() == rhs.GetMonth()) {
            return lhs.GetDay() < rhs.GetDay();
        }
        else
            return lhs.GetMonth() < rhs.GetMonth();
    }
    else
        return lhs.GetYear() < rhs.GetYear();
}

std::istream& operator>>(std::istream& stream, Date& d) {
    std::string date_str;
    stream >> date_str;
    std::stringstream date_stream(date_str);

    bool ok = true;

    int year = 0;
    ok = ok && (date_stream >> year);
    ok = ok && (date_stream.peek() == '-');
    date_stream.ignore(1);

    int month = 0;
    ok = ok && (date_stream >> month);
    ok = ok && (date_stream.peek() == '-');
    date_stream.ignore(1);

    int day = 0;
    ok = ok && (date_stream >> day);
    ok = ok && date_stream.eof();

    if (!ok) {
        throw std::invalid_argument("Wrong date format: " + date_str);
    }

    d = Date(day, month, year);

    return stream;
}
std::ostream& operator<<(std::ostream& stream, const Date& d) {
    stream << std::setw(4) << std::setfill('0') << d.GetYear() << "-";
    stream << std::setw(2) << std::setfill('0') << d.GetMonth() << "-";
    stream << std::setw(2) << std::setfill('0') << d.GetDay();
    return stream;
}


class Database {
public:
    void AddEvent(const Date& date, const std::string& event) {
        data[date].insert(event);
    }
    bool DeleteEvent(const Date& date, const std::string& event) {
        if (data.count(date) > 0 && data[date].count(event) > 0) {
            data[date].erase(event);
            return true;
        }
        else {
            return false;
        }
    }
    int  DeleteDate(const Date& date) {
        if (data.count(date) > 0) {
            int size = data[date].size();
            data.erase(date);
            return size;
        }
        else {
            return 0;
        }
    }

    std::set<std::string> Find(const Date& date) const {
        if (data.count(date) > 0) {
            return data.at(date);
        }
        else {
            return {};
        }
    }

    void Print() const {
        for (const auto& it: data) {
            for (const auto& e : it.second) {
                std::cout << it.first << " " << e << std::endl;
            }
        }
    }

private:
    std::map<Date, std::set<std::string>> data;
};

int main() {
#ifdef DEBUG
    std::ifstream in("tests/input.txt");
    std::cin.rdbuf(in.rdbuf());
#endif

    Database db;

    try {
        std::string line;
        while (std::getline(std::cin, line)) {
            if (line.empty())
                continue;

            std::stringstream line_stream(line);
            std::string command;
            line_stream >> command;

            if (command == "Add") {
                Date date;
                std::string event;
                line_stream >> date >> event;
                db.AddEvent(date, event);
            }
            else if (command == "Del") {
                Date date;
                std::string event;
                line_stream >> date >> event;

                if (event == "") {
                    int count = db.DeleteDate(date);
                    std::cout << "Deleted " << count << " events" << std::endl;
                }
                else {
                    if (db.DeleteEvent(date, event)) {
                        std::cout << "Deleted successfully" << std::endl;
                    }
                    else {
                        std::cout << "Event not found" << std::endl;
                    }
                }
            }
            else if (command == "Find") {
                Date date;
                line_stream >> date;

                std::set<std::string> events = db.Find(date);
                for (const auto& e : events) {
                    std::cout << e << std::endl;
                }
            }
            else if (command == "Print") {
                db.Print();
            }
            else {
                std::cout << "Unknown command: " << command << std::endl;
            }

        }
    }
    catch (std::invalid_argument& e) {
        std::cout << e.what() << std::endl;
        return 0;
    }
    return 0;
}
