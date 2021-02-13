#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

template <class T>
ostream& operator << (ostream& os, const vector<T>& s) {
  os << "{";
  bool first = true;
  for (const auto& x : s) {
    if (!first) {
      os << ", ";
    }
    first = false;
    os << x;
  }
  return os << "}";
}

template <class T>
ostream& operator << (ostream& os, const set<T>& s) {
  os << "{";
  bool first = true;
  for (const auto& x : s) {
    if (!first) {
      os << ", ";
    }
    first = false;
    os << x;
  }
  return os << "}";
}

template <class K, class V>
ostream& operator << (ostream& os, const map<K, V>& m) {
  os << "{";
  bool first = true;
  for (const auto& kv : m) {
    if (!first) {
      os << ", ";
    }
    first = false;
    os << kv.first << ": " << kv.second;
  }
  return os << "}";
}

template <class K, class V>
ostream& operator << (ostream& os, const pair<K, V>& p) {
  os << "{" << p.first << ": " << p.second;
  return os << "}";
}

template<class T, class U>
void AssertEqual(const T& t, const U& u, const string& hint = {}) {
  if (t != u) {
    ostringstream os;
    os << "Assertion failed: " << t << " != " << u;
    if (!hint.empty()) {
       os << " hint: " << hint;
    }
    throw runtime_error(os.str());
  }
}

void Assert(bool b, const string& hint) {
  AssertEqual(b, true, hint);
}

class TestRunner {
public:
  template <class TestFunc>
  void RunTest(TestFunc func, const string& test_name) {
    try {
      func();
      cerr << test_name << " OK" << endl;
    } catch (exception& e) {
      ++fail_count;
      cerr << test_name << " fail: " << e.what() << endl;
    } catch (...) {
      ++fail_count;
      cerr << "Unknown exception caught" << endl;
    }
  }

  ~TestRunner() {
    if (fail_count > 0) {
      cerr << fail_count << " unit tests failed. Terminate" << endl;
      exit(1);
    }
  }

private:
  int fail_count = 0;
};

#ifdef LOCAL_BUILD
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
#endif

void AssertRational(const Rational& r, const pair<int, int>& correct, const string& message) {
  AssertEqual(pair<int, int>(r.Numerator(), r.Denominator()), correct, message);
}

void TestDefaultConstructor() {
    AssertRational(Rational(), {0, 1}, "Default constructor");
    AssertRational(Rational(2, 3), {2, 3}, "2/3");
    AssertRational(Rational(3, 6), {1, 2}, "3/6 -> 1/2");
    AssertRational(Rational(-3, -6), {1, 2}, "-3/-6 -> 1/2");
    AssertRational(Rational(-3, 6), {-1, 2}, "-3/6 -> -1/2");
    AssertRational(Rational(3,- 6), {-1, 2}, "3/-6 -> -1/2");
    AssertRational(Rational(0,- 6), {0, 1}, "0/-6 ->  0/1");
}

int main() {
  TestRunner runner;
  runner.RunTest(TestDefaultConstructor, "TestDefaultConstructor");
  return 0;
}
