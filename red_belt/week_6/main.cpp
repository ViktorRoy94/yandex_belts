#include "search_server.h"
#include "parse.h"
#include "test_runner.h"
#include "profile.h"

#include <algorithm>
#include <iterator>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <random>
#include <thread>
#include <unistd.h>
using namespace std;

void TestFunctionality(
        const vector<string>& docs,
        const vector<string>& queries,
        const vector<string>& expected
        ) {
    istringstream docs_input(Join('\n', docs));
    istringstream queries_input(Join('\n', queries));

    SearchServer srv;
    srv.UpdateDocumentBase(docs_input);
    ostringstream queries_output;
    srv.AddQueriesStream(queries_input, queries_output);

    const string result = queries_output.str();
    const auto lines = SplitBy(Strip(result), '\n');
    ASSERT_EQUAL(lines.size(), expected.size());
    for (size_t i = 0; i < lines.size(); ++i) {
        ASSERT_EQUAL(lines[i], expected[i]);
    }
}

void TestSerpFormat() {
    const vector<string> docs = {
        "london is the capital of great britain",
        "i am travelling down the river"
    };
    const vector<string> queries = {"london", "the"};
    const vector<string> expected = {
        "london: {docid: 0, hitcount: 1}",
        Join(' ', vector{
            "the:",
            "{docid: 0, hitcount: 1}",
            "{docid: 1, hitcount: 1}"
        })
    };

    TestFunctionality(docs, queries, expected);
}

void TestTop5() {
    const vector<string> docs = {
        "milk a",
        "milk b",
        "milk c",
        "milk d",
        "milk e",
        "milk f",
        "milk g",
        "water a",
        "water b",
        "fire and earth"
    };

    const vector<string> queries = {"milk", "water", "rock"};
    const vector<string> expected = {
        Join(' ', vector{
            "milk:",
            "{docid: 0, hitcount: 1}",
            "{docid: 1, hitcount: 1}",
            "{docid: 2, hitcount: 1}",
            "{docid: 3, hitcount: 1}",
            "{docid: 4, hitcount: 1}"
        }),
        Join(' ', vector{
            "water:",
            "{docid: 7, hitcount: 1}",
            "{docid: 8, hitcount: 1}",
        }),
        "rock:",
    };
    TestFunctionality(docs, queries, expected);
}

void TestHitcount() {
    const vector<string> docs = {
        "the river goes through the entire city there is a house near it",
        "the wall",
        "walle",
        "is is is is",
    };
    const vector<string> queries = {"the", "wall", "all", "is", "the is"};
    const vector<string> expected = {
        Join(' ', vector{
            "the:",
            "{docid: 0, hitcount: 2}",
            "{docid: 1, hitcount: 1}",
        }),
        "wall: {docid: 1, hitcount: 1}",
        "all:",
        Join(' ', vector{
            "is:",
            "{docid: 3, hitcount: 4}",
            "{docid: 0, hitcount: 1}",
        }),
        Join(' ', vector{
            "the is:",
            "{docid: 3, hitcount: 4}",
            "{docid: 0, hitcount: 3}",
            "{docid: 1, hitcount: 1}",
        }),
    };
    TestFunctionality(docs, queries, expected);
}

void TestRanking() {
    const vector<string> docs = {
        "london is the capital of great britain",
        "paris is the capital of france",
        "berlin is the capital of germany",
        "rome is the capital of italy",
        "madrid is the capital of spain",
        "lisboa is the capital of portugal",
        "bern is the capital of switzerland",
        "moscow is the capital of russia",
        "kiev is the capital of ukraine",
        "minsk is the capital of belarus",
        "astana is the capital of kazakhstan",
        "beijing is the capital of china",
        "tokyo is the capital of japan",
        "bangkok is the capital of thailand",
        "welcome to moscow the capital of russia the third rome",
        "amsterdam is the capital of netherlands",
        "helsinki is the capital of finland",
        "oslo is the capital of norway",
        "stockgolm is the capital of sweden",
        "riga is the capital of latvia",
        "tallin is the capital of estonia",
        "warsaw is the capital of poland",
    };

    const vector<string> queries = {"moscow is the capital of russia"};
    const vector<string> expected = {
        Join(' ', vector{
            "moscow is the capital of russia:",
            "{docid: 7, hitcount: 6}",
            "{docid: 14, hitcount: 6}",
            "{docid: 0, hitcount: 4}",
            "{docid: 1, hitcount: 4}",
            "{docid: 2, hitcount: 4}",
        })
    };
    TestFunctionality(docs, queries, expected);
}

void TestBasicSearch() {
    const vector<string> docs = {
        "we are ready to go",
        "come on everybody shake you hands",
        "i love this game",
        "just like exception safety is not about writing try catch everywhere in your code move semantics are not about typing double ampersand everywhere in your code",
        "daddy daddy daddy dad dad dad",
        "tell me the meaning of being lonely",
        "just keep track of it",
        "how hard could it be",
        "it is going to be legen wait for it dary legendary",
        "we dont need no education"
    };

    const vector<string> queries = {
        "we need some help",
        "it",
        "i love this game",
        "tell me why",
        "dislike",
        "about"
    };

    const vector<string> expected = {
        Join(' ', vector{
            "we need some help:",
            "{docid: 9, hitcount: 2}",
            "{docid: 0, hitcount: 1}"
        }),
        Join(' ', vector{
            "it:",
            "{docid: 8, hitcount: 2}",
            "{docid: 6, hitcount: 1}",
            "{docid: 7, hitcount: 1}",
        }),
        "i love this game: {docid: 2, hitcount: 4}",
        "tell me why: {docid: 5, hitcount: 2}",
        "dislike:",
        "about: {docid: 3, hitcount: 2}",
    };
    TestFunctionality(docs, queries, expected);
}

void TestFailedCase6() {
    const vector<string> docs = {
        "a   b c   d",
        "  a b c d",
        "   c  d e f   ",
        "d  e f    g"
    };

    const vector<string> queries = {
        "a ",
        "b ",
        "c ",
        "d ",
        "e ",
        "f ",
        "g ",
        "b  c   d  f",
        "cde g",
        " c d",
        "  a b  ",
        "g    "
    };

    const vector<string> expected = {
        "a : {docid: 0, hitcount: 1} {docid: 1, hitcount: 1}",
        "b : {docid: 0, hitcount: 1} {docid: 1, hitcount: 1}",
        "c : {docid: 0, hitcount: 1} {docid: 1, hitcount: 1} {docid: 2, hitcount: 1}",
        "d : {docid: 0, hitcount: 1} {docid: 1, hitcount: 1} {docid: 2, hitcount: 1} {docid: 3, hitcount: 1}",
        "e : {docid: 2, hitcount: 1} {docid: 3, hitcount: 1}",
        "f : {docid: 2, hitcount: 1} {docid: 3, hitcount: 1}",
        "g : {docid: 3, hitcount: 1}",
        "b  c   d  f: {docid: 0, hitcount: 3} {docid: 1, hitcount: 3} {docid: 2, hitcount: 3} {docid: 3, hitcount: 2}",
        "cde g: {docid: 3, hitcount: 1}",
        " c d: {docid: 0, hitcount: 2} {docid: 1, hitcount: 2} {docid: 2, hitcount: 2} {docid: 3, hitcount: 1}",
        "  a b  : {docid: 0, hitcount: 2} {docid: 1, hitcount: 2}",
        "g    : {docid: 3, hitcount: 1}"
    };
    TestFunctionality(docs, queries, expected);
}

string gen_random(const int len) {
    string tmp_s;
    static const char alphanum[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";

    tmp_s.reserve(len);
    for (int i = 0; i < len; ++i)
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];

    return tmp_s;
}

string select_random(const vector<string> &s) {
    auto r = rand() % s.size();
    auto it = std::begin(s);
    std::advance(it, r);
    return *it;
}

void TestPerfomance() {
    size_t word_length = 100;
    size_t unique_strings_count = 1500;

    vector<string> unique_strings(unique_strings_count);
    vector<string> docs(5000);
    vector<string> queries(500000);

    srand(time(nullptr));

    {
        LOG_DURATION("Prepare data");
        for (size_t i = 0; i < unique_strings_count; i++) {
            unique_strings[i] = gen_random(word_length);
        }

        for (auto& doc : docs) {
            //            size_t words_count = rand() % 1000;
            size_t words_count = 1000;
            for (size_t i = 0; i < words_count; i++) {
                doc += select_random(unique_strings);
                doc += " ";
            }
        }

        for (auto& q : queries) {
            //            size_t words_count = rand() % 10;
            size_t words_count = 10;
            for (size_t i = 0; i < words_count; i++) {
                q += select_random(unique_strings);
                q += " ";
            }
        }
    }

    LOG_DURATION("Create input streams");
    istringstream document_input(Join('\n', docs));
    istringstream query_input(Join('\n', queries));
    ostringstream search_results_output;

    {
        LOG_DURATION("SearchServerRun");
        SearchServer srv(document_input);
        srv.AddQueriesStream(query_input, search_results_output);
    }
}

void TestSplitIntoWords() {
    {
        vector<string_view> result = SplitIntoWords("         a  \t b \t c   d");
        ASSERT_EQUAL(string(result[0]), "a");
        ASSERT_EQUAL(string(result[1]), "b");
        ASSERT_EQUAL(string(result[2]), "c");
        ASSERT_EQUAL(string(result[3]), "d");
        ASSERT_EQUAL(result.size(), 4u);
    }
    {
        vector<string_view> result = SplitIntoWords("    cde g");
        ASSERT_EQUAL(string(result[0]), "cde");
        ASSERT_EQUAL(string(result[1]), "g");
        ASSERT_EQUAL(result.size(), 2u);
    }
    {
        vector<string_view> result = SplitIntoWords("g    ");
        ASSERT_EQUAL(string(result[0]), "g");
        ASSERT_EQUAL(result.size(), 1u);
    }
}

void TestVectorOneFor() {
    LOG_DURATION("vec increment");
    vector<size_t> vec(10000);
    fill(vec.begin(), vec.end(), 0);
    for (size_t i = 0; i < 500000u * 10u * 7000u; i++) {
        //        size_t docid = rand() % vec.size();
        ++vec[rand() % vec.size()];
    }
}

void TestVectorMultipleFor() {
    LOG_DURATION("vec increment");
    vector<size_t> vec(10000);
    fill(vec.begin(), vec.end(), 0);
    for (size_t i = 0; i < 500000u; i++) {
        for (size_t j = 0; j < 10u; j++) {
            for (size_t k = 0; k < 7000u; k++) {
                //                size_t docid = rand() % vec.size();
                ++vec[k % vec.size()];
            }
        }
    }
}

void TestSearchServer(vector<pair<istream, ostream*>> streams) {
    LOG_DURATION("Total multithread");
    SearchServer srv(streams.front().first);
    for (auto& [input, output] :
         IteratorRange(begin(streams) + 1, end(streams))) {
        this_thread::sleep_for(chrono::milliseconds(rand() % 1000));
        if (!output) {
            srv.UpdateDocumentBase(input);
        } else {
            srv.AddQueriesStream(input, *output);
        }
    }
}

void TestPerfomanceMultithread() {
    size_t word_length = 100;
    size_t unique_strings_count = 1500;

    vector<string> unique_strings(unique_strings_count);
    vector<string> docs(10000);
    vector<string> queries(10000);

    srand(time(nullptr));

    {
        LOG_DURATION("Prepare data");
        for (size_t i = 0; i < unique_strings_count; i++) {
            unique_strings[i] = gen_random(word_length);
        }

        for (auto& doc : docs) {
            //            size_t words_count = rand() % 1000;
            size_t words_count = 1000;
            for (size_t i = 0; i < words_count; i++) {
                doc += select_random(unique_strings);
                doc += " ";
            }
        }

        for (auto& q : queries) {
            //            size_t words_count = rand() % 10;
            size_t words_count = 10;
            for (size_t i = 0; i < words_count; i++) {
                q += select_random(unique_strings);
                q += " ";
            }
        }
    }

    LOG_DURATION("Create input streams");
    istringstream document_input(Join('\n', docs));
    istringstream query_input(Join('\n', queries));
    ostringstream search_results_output;

    {
        LOG_DURATION("SearchServerRun");
        SearchServer srv(document_input);
        srv.AddQueriesStream(query_input, search_results_output);
    }
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestSplitIntoWords);
    RUN_TEST(tr, TestSerpFormat);
    RUN_TEST(tr, TestTop5);
    RUN_TEST(tr, TestHitcount);
    RUN_TEST(tr, TestRanking);
    RUN_TEST(tr, TestBasicSearch);
    RUN_TEST(tr, TestFailedCase6);
    RUN_TEST(tr, TestPerfomance);
    //    RUN_TEST(tr, TestVectorOneFor);
    //    RUN_TEST(tr, TestVectorMultipleFor);

    return 0;
}
