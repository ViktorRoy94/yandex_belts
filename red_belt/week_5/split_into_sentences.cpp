#include "test_runner.h"

#include <vector>

using namespace std;

// Объявляем Sentence<Token> для произвольного типа Token
// синонимом vector<Token>.
// Благодаря этому в качестве возвращаемого значения
// функции можно указать не малопонятный вектор векторов,
// а вектор предложений — vector<Sentence<Token>>.
template <typename Token>
using Sentence = vector<Token>;

// Класс Token имеет метод bool IsEndSentencePunctuation() const
template <typename Token>
vector<Sentence<Token>> SplitIntoSentences(vector<Token> tokens) {
    vector<Sentence<Token>> result;
    bool prev_token_is_end = false;
    for(Token& t : tokens) {
        bool cur_token_is_end = t.IsEndSentencePunctuation();
        if ((!cur_token_is_end && prev_token_is_end) || result.empty()) {
            result.push_back({});
        }
        result.back().push_back(move(t));
        prev_token_is_end = cur_token_is_end;
    }
    return move(result);
}


struct TestToken {
    string data;
    bool is_end_sentence_punctuation = false;

    bool IsEndSentencePunctuation() const {
        return is_end_sentence_punctuation;
    }
    bool operator==(const TestToken& other) const {
        return data == other.data && is_end_sentence_punctuation == other.is_end_sentence_punctuation;
    }
};

ostream& operator<<(ostream& stream, const TestToken& token) {
    return stream << token.data;
}

struct TestNoCopyToken {
    TestNoCopyToken(const string& s) {
        data = move(s);
    }
    TestNoCopyToken(const TestNoCopyToken& token) = delete;
    void operator=(const TestNoCopyToken& other) = delete;

    TestNoCopyToken(TestNoCopyToken&& token) {
        data = move(token.data);
        is_end_sentence_punctuation = token.is_end_sentence_punctuation;
    }
    void operator=(TestNoCopyToken&& other) {
        data = move(other.data);
        is_end_sentence_punctuation = other.is_end_sentence_punctuation;
    }

    string data;
    bool is_end_sentence_punctuation = false;

    bool IsEndSentencePunctuation() const {
        return is_end_sentence_punctuation;
    }
    bool operator==(const TestNoCopyToken& other) const {
        return data == other.data && is_end_sentence_punctuation == other.is_end_sentence_punctuation;
    }
};

ostream& operator<<(ostream& stream, const TestNoCopyToken& token) {
    return stream << token.data;
}

// Тест содержит копирования объектов класса TestToken.
// Для проверки отсутствия копирований в функции SplitIntoSentences
// необходимо написать отдельный тест.
void TestSplitting() {
    ASSERT_EQUAL(
                SplitIntoSentences(vector<TestToken>({{"Split"}, {"into"}, {"sentences"}, {"!"}})),
                vector<Sentence<TestToken>>({
                                                {{"Split"}, {"into"}, {"sentences"}, {"!"}}
                                            })
                );

    ASSERT_EQUAL(
                SplitIntoSentences(vector<TestToken>({{"Split"}, {"into"}, {"sentences"}, {"!", true}})),
                vector<Sentence<TestToken>>({
                                                {{"Split"}, {"into"}, {"sentences"}, {"!", true}}
                                            })
                );

    ASSERT_EQUAL(
                SplitIntoSentences(vector<TestToken>({{"Split"}, {"into"}, {"sentences"}, {"!", true}, {"!", true}, {"Without"}, {"copies"}, {".", true}})),
                vector<Sentence<TestToken>>({
                                                {{"Split"}, {"into"}, {"sentences"}, {"!", true}, {"!", true}},
                                                {{"Without"}, {"copies"}, {".", true}},
                                            })
                );
}

void TestNoCopySplitting() {

    vector<TestNoCopyToken> data;
    data.push_back(TestNoCopyToken("Split"));
    data.push_back(TestNoCopyToken("into"));
    data.push_back(TestNoCopyToken("sentences"));
    data.push_back(TestNoCopyToken("!"));

    vector<Sentence<TestNoCopyToken>> result;
    result.push_back({});
    result.back().push_back(TestNoCopyToken("Split"));
    result.back().push_back(TestNoCopyToken("into"));
    result.back().push_back(TestNoCopyToken("sentences"));
    result.back().push_back(TestNoCopyToken("!"));

    ASSERT_EQUAL(SplitIntoSentences(move(data)), result);
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestSplitting);
    RUN_TEST(tr, TestNoCopySplitting);
    return 0;
}
