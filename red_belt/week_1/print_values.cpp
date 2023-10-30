#include "test_runner.h"

#include <ostream>
using namespace std;

#define PRINT_VALUES(out, x, y) out << (x) << endl << (y) << endl

int main() {
    TestRunner tr;
    tr.RunTest([] {
        ostringstream output;
        PRINT_VALUES(output, 5, "red belt");
        ASSERT_EQUAL(output.str(), "5\nred belt\n");
    }, "PRINT_VALUES usage example");

    tr.RunTest([] {
        ostringstream output;
        PRINT_VALUES(output, 5, 10);
        ASSERT_EQUAL(output.str(), "5\n10\n");
    }, "PRINT_VALUES test 1");

    tr.RunTest([] {
        ostringstream output;
        PRINT_VALUES(output, "5", "10");
        ASSERT_EQUAL(output.str(), "5\n10\n");
    }, "PRINT_VALUES test 2");

    tr.RunTest([] {
        ostringstream output;
        PRINT_VALUES(output, 2+3, 5+5);
        ASSERT_EQUAL(output.str(), "5\n10\n");
    }, "PRINT_VALUES test 3");

    tr.RunTest([] {
        ostringstream output;
        PRINT_VALUES(output, 2+3, 5+5);
        ASSERT_EQUAL(output.str(), "5\n10\n");
    }, "PRINT_VALUES test 3");

    tr.RunTest([] {
        ostringstream output;
        PRINT_VALUES(PRINT_VALUES(output, 2+3, 5+5), 2+3, 5+5);
        ASSERT_EQUAL(output.str(), "5\n10\n5\n10\n");
    }, "PRINT_VALUES test 4");
}
