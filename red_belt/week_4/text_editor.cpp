#include <string>
#include <list>
#include <vector>
#include <algorithm>
#include "test_runner.h"
using namespace std;

class Editor {
public:
    // Реализуйте конструктор по умолчанию и объявленные методы
    Editor() : cursor_position_(data_.end())
    {}

    void Left() {
        cursor_position_ = Advance(cursor_position_, -1);
    }
    void Right() {
        cursor_position_ = Advance(cursor_position_, 1);
    }
    void Insert(char token) {
        data_.insert(cursor_position_, token);
    }
    void Cut(size_t tokens = 1) {
        auto end_position = Advance(cursor_position_, tokens);
        buffer_.assign(cursor_position_, end_position);
        cursor_position_ = data_.erase(cursor_position_, end_position);
    }
    void Copy(size_t tokens = 1) {
        auto end_position = Advance(cursor_position_, tokens);
        buffer_.assign(cursor_position_, end_position);
    }
    void Paste() {
        data_.insert(cursor_position_, buffer_.begin(), buffer_.end());
    }
    string GetText() const {
        return {data_.begin(), data_.end()};
    }

private:
    using Iterator = list<char>::iterator;
    list<char> data_;
    Iterator cursor_position_;
    list<char> buffer_;

    Iterator Advance(Iterator it, int steps) const {
        while (steps > 0 && it != data_.end()) {
            ++it;
            --steps;
        }
        while (steps < 0 && it != data_.begin()) {
            --it;
            ++steps;
        }
        return it;
    }
};

void TypeText(Editor& editor, const string& text) {
    for(char c : text) {
        editor.Insert(c);
    }
}

void TestEditing() {
    {
        Editor editor;

        const size_t text_len = 12;
        const size_t first_part_len = 7;
        TypeText(editor, "hello, world");
        for(size_t i = 0; i < text_len; ++i) {
            editor.Left();
        }
        editor.Cut(first_part_len);
        for(size_t i = 0; i < text_len - first_part_len; ++i) {
            editor.Right();
        }
        TypeText(editor, ", ");
        editor.Paste();
        editor.Left();
        editor.Left();
        editor.Cut(3);

        ASSERT_EQUAL(editor.GetText(), "world, hello");
    }
    {
        Editor editor;

        TypeText(editor, "misprnit");
        editor.Left();
        editor.Left();
        editor.Left();
        editor.Cut(1);
        editor.Right();
        editor.Paste();

        ASSERT_EQUAL(editor.GetText(), "misprint");
    }
}

void TestReverse() {
    Editor editor;

    const string text = "esreveR";
    for(char c : text) {
        editor.Insert(c);
        editor.Left();
    }

    ASSERT_EQUAL(editor.GetText(), "Reverse");
}

void TestNoText() {
    Editor editor;
    ASSERT_EQUAL(editor.GetText(), "");

    editor.Left();
    editor.Left();
    editor.Right();
    editor.Right();
    editor.Copy(0);
    editor.Cut(0);
    editor.Paste();

    ASSERT_EQUAL(editor.GetText(), "");
}

void TestEmptyBuffer() {
    Editor editor;

    editor.Paste();
    TypeText(editor, "example");
    editor.Left();
    editor.Left();
    editor.Paste();
    editor.Right();
    editor.Paste();
    editor.Copy(0);
    editor.Paste();
    editor.Left();
    editor.Cut(0);
    editor.Paste();

    ASSERT_EQUAL(editor.GetText(), "example");
}

void TestCopyPasteLastChars()
{
    Editor editor;
    TypeText(editor, "happy birthday");
    editor.Left();
    editor.Left();
    editor.Copy(5);
    editor.Paste();
    ASSERT_EQUAL(editor.GetText(), "happy birthdayay");
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestEditing);
    RUN_TEST(tr, TestReverse);
    RUN_TEST(tr, TestNoText);
    RUN_TEST(tr, TestEmptyBuffer);
    RUN_TEST(tr, TestCopyPasteLastChars);
    return 0;
}