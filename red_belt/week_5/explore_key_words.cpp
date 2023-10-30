#include "test_runner.h"
#include "profile.h"

#include <map>
#include <string>
#include <algorithm>
#include <future>
using namespace std;

struct Stats {
    map<string, int> word_frequences;

    void operator += (const Stats& other) {
        for (const auto& [key, value] : other.word_frequences) {
            word_frequences[key] += value;
        }
    }
};

vector<string> SplitIntoWords(const string& s) {
    vector<string> result;
    auto curr_it = begin(s);
    auto prev_it = begin(s);
    while (curr_it != end(s)) {
        curr_it = find(prev_it, end(s), ' ');
        result.push_back(string(prev_it, curr_it));
        prev_it = curr_it + 1;
    }
    return result;
}

Stats ExploreLine(const set<string>& key_words, const string& line) {
    Stats stats;
    for (const string&  word : SplitIntoWords(line)) {
        if (key_words.count(word) == 1) {
            stats.word_frequences[word]++;
        }
    }
    return stats;
}

Stats ExploreLines(const set<string>& key_words, vector<string> lines) {
    Stats stats;
    for (const auto& line : lines) {
        stats += ExploreLine(key_words, line);
    }
    return stats;
}

Stats ExploreKeyWordsSingleThread(const set<string>& key_words, istream& input) {
    Stats result;
    for (string line; getline(input, line); ) {
        result += ExploreLine(key_words, line);
    }
    return result;
}

Stats ExploreKeyWordsMultiThreadThread(const set<string>& key_words, istream& input) {
    size_t max_batch_size = 5000;
    vector<string> batch;
    batch.reserve(max_batch_size);
    vector<future<Stats>> futures;
    for (string line; getline(input, line); ) {
        batch.push_back(line);
        if (batch.size() >= max_batch_size) {
            futures.push_back(async(ExploreLines, ref(key_words), move(batch)));
        }
    }
    Stats result;
    if (!batch.empty()) {
        result += ExploreLines(key_words, move(batch));
    }

    for (auto& f : futures) {
        result += f.get();
    }

    //    size_t chunks_count = 4;
    //    size_t chunk_size = lines.size() / chunks_count;
    //    for (size_t i = 0; i < chunks_count; i++) {
    //        auto first = lines.begin() + i * chunk_size;
    //        auto last = lines.begin() + (i + 1) * chunk_size;
    //        last = last > lines.end() ? lines.end() : last;
    //        last = i == chunks_count - 1 ? lines.end() : last;
    //        futures.push_back(async(ExploreLines<vector<string>::iterator>, ref(key_words), first, last));
    //    }

    return result;
}

Stats ExploreKeyWords(const set<string>& key_words, istream& input) {
    return ExploreKeyWordsMultiThreadThread(key_words, input);
}

void TestBasic() {
    const set<string> key_words = {"yangle", "rocks", "sucks", "all"};

    stringstream ss;
    ss << "this new yangle service really rocks\n";
    ss << "It sucks when yangle isn't available\n";
    ss << "10 reasons why yangle is the best IT company\n";
    ss << "yangle rocks others suck\n";
    ss << "Goondex really sucks, but yangle rocks. Use yangle\n";

    const auto stats = ExploreKeyWords(key_words, ss);
    const map<string, int> expected = {
        {"yangle", 6},
        {"rocks", 2},
        {"sucks", 1}
    };
    ASSERT_EQUAL(stats.word_frequences, expected);
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestBasic);
}
