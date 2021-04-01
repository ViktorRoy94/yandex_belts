#include "test_runner.h"
#include <string>
#include <set>

using namespace std;

class Translator {
public:
    void Add(string_view source, string_view target) {
        auto it1 = data_.emplace(string(source));
        auto it2 = data_.emplace(string(target));

        string_view source_view = *it1.first;
        string_view target_view = *it2.first;

        dict_[source_view] = target_view;
        reversed_dict_[target_view] = source_view;
    }
    string_view TranslateForward(string_view source) const {
        return dict_.count(source) > 0 ? dict_.at(source) : "";
    }
    string_view TranslateBackward(string_view target) const {
        return reversed_dict_.count(target) > 0 ? reversed_dict_.at(target) : "";
    }

private:
    set<string> data_;
    map<string_view, string_view> dict_;
    map<string_view, string_view> reversed_dict_;
};

void TestSimple() {
    Translator translator;
    translator.Add(string("okno"), string("window"));
    translator.Add(string("stol"), string("table"));

    ASSERT_EQUAL(translator.TranslateForward("okno"), "window");
    ASSERT_EQUAL(translator.TranslateBackward("table"), "stol");
    ASSERT_EQUAL(translator.TranslateBackward("stol"), "");
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestSimple);
    return 0;
}
