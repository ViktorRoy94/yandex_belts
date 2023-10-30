#include <algorithm>
#include <string>
#include <vector>
#include <set>
#include "test_runner.h"

using namespace std;

class Learner {
private:
    set<string> dict;

public:
    int Learn(const vector<string>& words) {
        int newWords = 0;
        for (const auto& word : words) {
            auto result  = dict.insert(word);
            if (result.second) {
                ++newWords;
            }
        }
        return newWords;
    }

    vector<string> KnownWords() {
        return {dict.begin(), dict.end()};
    }
};

void TestLearner() {
    {
        Learner l;
        vector<string> words = {"a", "b", "c"};
        int count = l.Learn(words);
        ASSERT_EQUAL(count, 3);
        ASSERT_EQUAL(l.KnownWords(), words);
    }
    {
        Learner l;
        vector<string> words = {"c", "b", "a"};
        int count = l.Learn(words);
        ASSERT_EQUAL(count, 3);
        reverse(words.begin(), words.end());
        ASSERT_EQUAL(l.KnownWords(), words);
    }
    {
        Learner l;
        vector<string> words = {"a", "a", "a"};
        int count = l.Learn(words);
        ASSERT_EQUAL(count, 1);
        vector<string> result_words = {"a"};
        ASSERT_EQUAL(l.KnownWords(), result_words);
    }
}

#ifdef LOCAL_BUILD
int main() {
    TestRunner tr;
    RUN_TEST(tr, TestLearner);
}
#endif


