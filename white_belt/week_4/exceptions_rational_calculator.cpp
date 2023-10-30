#include <iostream>
#include <sstream>
#include <set>
#include <map>
#include <vector>

class Rational {
public:
    Rational() {
        numerator = 0;
        denominator = 1;
    }

    Rational(int numerator, int denominator) {
        if (denominator == 0) {
            throw std::invalid_argument("Invalid argument");
        }

        int gcf_value = gcf(abs(numerator), abs(denominator));

        if (denominator < 0) {
            numerator *= -1;
            denominator *= -1;
        }

        this->numerator = numerator / gcf_value;
        this->denominator = denominator / gcf_value;
    }

    int Numerator() const {
        return numerator;
    }

    int Denominator() const {
        return denominator;
    }

    Rational operator+(const Rational& r) {
        return Rational {numerator * r.denominator + r.numerator * denominator,
                    denominator * r.denominator};
    }

    Rational operator-(const Rational& r) {
        return Rational {numerator * r.denominator - r.numerator * denominator,
                    denominator * r.denominator};
    }

    Rational operator*(const Rational& r) {
        return Rational {numerator *  r.numerator, denominator * r.denominator};
    }

    Rational operator/(const Rational& r) {
        if (r.numerator == 0)
            throw std::domain_error("Division by zero");
        return Rational {numerator *  r.denominator, r.numerator * denominator};
    }

private:
    int numerator;
    int denominator;

    int gcf(int a, int b) {
        while (a != 0 and b != 0) {
            if (a > b) {
                a = a % b;
            }
            else {
                b = b % a;
            }
        }
        return a + b;
    }
};

bool operator==(const Rational& r1, const Rational& r2) {
    if (r1.Denominator() == r2.Denominator() &&
            r1.Numerator() == r2.Numerator())
        return true;
    else
        return false;
}

bool operator<(const Rational& r1, const Rational& r2) {
    return r1.Numerator() * r2.Denominator() < r2.Numerator() * r1.Denominator();
}

std::ostream& operator<<(std::ostream& stream, const Rational& r) {
    stream << r.Numerator() << "/" << r.Denominator();
    return stream;
}

std::istream& operator>>(std::istream& stream, Rational& r) {
    int numerator = r.Numerator();
    int denominator = r.Denominator();
    stream >> numerator;
    stream.ignore(1);
    stream >> denominator;
    r = Rational(numerator, denominator);
    return stream;
}


int main() {
    Rational r1, r2;
    std::string op;

    try {
        std::cin >> r1 >> op >> r2;
        if (op == "+") {
            std::cout << r1 + r2 << std::endl;
        }
        else if (op == "-") {
            std::cout << r1 - r2 << std::endl;
        }
        else if (op == "*") {
            std::cout << r1 * r2 << std::endl;
        }
        else if (op == "/") {
            std::cout << r1 / r2 << std::endl;
        }
    }
    catch (std::exception& ex) {
        std::cout << ex.what() << std::endl;
    }

    return 0;
}
