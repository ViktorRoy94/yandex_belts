#include <string>

struct Specialization {
    std::string value;
    explicit Specialization(std::string new_value) {
        value = new_value;
    }
};

struct Course {
    std::string value;
    explicit Course(std::string new_value) {
        value = new_value;
    }
};

struct Week {
    std::string value;
    explicit Week(std::string new_value) {
        value = new_value;
    }
};


struct LectureTitle {
  std::string specialization;
  std::string course;
  std::string week;

  LectureTitle(const Specialization& new_spec, const Course& new_course, const Week& new_week) {
        specialization = new_spec.value;
        course = new_course.value;
        week = new_week.value;
  }
};


int main() {
    // right way
    LectureTitle title1(
        Specialization("C++"),
        Course("White belt"),
        Week("4th")
    );

    // not right way
//    LectureTitle title2("C++", "White belt", "4th");

//    LectureTitle title3(std::string("C++"), std::string("White belt"), std::string("4th"));

//    LectureTitle title4 = {"C++", "White belt", "4th"};

//    LectureTitle title5 = {{"C++"}, {"White belt"}, {"4th"}};

//    LectureTitle title6(
//        Course("White belt"),
//        Specialization("C++"),
//        Week("4th")
//    );

//    LectureTitle title7(
//        Specialization("C++"),
//        Week("4th"),
//        Course("White belt")
//    );
}
