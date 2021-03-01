#include <vector>
#include <memory>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <math.h>

using namespace std;

class Figure {
public:
    virtual string Name() = 0;
    virtual float Perimeter() = 0;
    virtual float Area() = 0;
};

class Rect : public Figure {
public:
    Rect(int width, int height) : width_(width), height_(height) {}
    string Name() override {
        return "RECT";
    }
    float Perimeter() override {
        return 2 * (width_ + height_);
    }
    float Area() override {
        return width_ * height_;
    }

private:
    int width_;
    int height_;
};

class Triangle : public Figure {
public:
    Triangle(int a, int b, int c) : a_(a), b_(b), c_(c) {}
    string Name() override {
        return "TRIANGLE";
    }
    float Perimeter() override {
        return a_ + b_ + c_;
    }
    float Area() override {
        float half_p = Perimeter() / 2;
        return sqrt(half_p * (half_p - a_) * (half_p - b_) * (half_p - c_));
    }
private:
    int a_, b_, c_;
};

class Circle : public Figure {
public:
    Circle(int r) : r_(r) {}
    string Name() override {
        return "CIRCLE";
    }
    float Perimeter() override {
        return 2 * 3.14 * r_;
    }
    float Area() override {
        return 3.14 * r_ * r_;
    }
private:
    int r_;
};

shared_ptr<Figure> CreateFigure(istringstream& is) {
    shared_ptr<Figure> result;
    string figure_type;
    is >> figure_type;
    if (figure_type == "RECT") {
        int width, height;
        is >> width >> height;
        result = make_shared<Rect>(width, height);

    } else if (figure_type == "TRIANGLE") {
        int a, b, c;
        is >> a >> b >> c;
        result = make_shared<Triangle>(a, b, c);

    } else if (figure_type == "CIRCLE") {
        int r;
        is >> r;
        result = make_shared<Circle>(r);
    }
    return result;
}

int main() {
  vector<shared_ptr<Figure>> figures;
  for (string line; getline(cin, line); ) {
    istringstream is(line);

    string command;
    is >> command;
    if (command == "ADD") {
      // Пропускаем "лишние" ведущие пробелы.
      // Подробнее об std::ws можно узнать здесь:
      // https://en.cppreference.com/w/cpp/io/manip/ws
      is >> ws;
      figures.push_back(CreateFigure(is));
    } else if (command == "PRINT") {
      for (const auto& current_figure : figures) {
        cout << fixed << setprecision(3)
             << current_figure->Name() << " "
             << current_figure->Perimeter() << " "
             << current_figure->Area() << endl;
      }
    }
  }
  return 0;
}