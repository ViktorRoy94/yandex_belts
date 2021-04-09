#include "test_runner.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>
#include <set>
#include <map>
#include <utility>
#include <vector>
#include <chrono>

using namespace std;

template <typename T>
class PriorityCollection {
public:
    using Id = std::chrono::time_point<std::chrono::system_clock>;

    // Добавить объект с нулевым приоритетом
    // с помощью перемещения и вернуть его идентификатор
    Id Add(T object)
    {
        Id id = std::chrono::system_clock::now();
        data_[id] = {move(object), 0};
        sorted_priority_.insert({0, id});
        return id;
    }

    // Добавить все элементы диапазона [range_begin, range_end)
    // с помощью перемещения, записав выданные им идентификаторы
    // в диапазон [ids_begin, ...)
    template <typename ObjInputIt, typename IdOutputIt>
    void Add(ObjInputIt range_begin, ObjInputIt range_end,
             IdOutputIt ids_begin)
    {
        for (auto it = range_begin; it != range_end; it++) {
            *(ids_begin++) = this->Add(move(*it));
        }
    }

    // Определить, принадлежит ли идентификатор какому-либо
    // хранящемуся в контейнере объекту
    bool IsValid(Id id) const {
        if (data_.count(id) > 0) {
            return true;
        } else {
            return false;
        }
    }

    // Получить объект по идентификатору
    const T& Get(Id id) const {
        return data_.at(id).object;
    }

    // Увеличить приоритет объекта на 1
    void Promote(Id id) {
        int& priority = data_.at(id).priority;
        sorted_priority_.erase({priority, id});
        priority++;
        sorted_priority_.insert({priority, id});
    }

    // Получить объект с максимальным приоритетом и его приоритет
    pair<const T&, int> GetMax() const {
        auto& max_elem = data_.at(prev(sorted_priority_.end())->second);
        return {move(max_elem.object), max_elem.priority};
    }

    // Аналогично GetMax, но удаляет элемент из контейнера
    pair<T, int> PopMax() {
        auto max_elem_it = prev(sorted_priority_.end());
        auto& item = data_.at(max_elem_it->second);

        data_.erase(max_elem_it->second);
        sorted_priority_.erase(max_elem_it);

        return {move(item.object), item.priority};
    }

private:
    struct DataItem {
        T object;
        int priority;
    };

    map<Id, DataItem> data_;
    set<pair<int, Id>> sorted_priority_;

};


class StringNonCopyable : public string {
public:
    using string::string;  // Позволяет использовать конструкторы строки
    StringNonCopyable(const StringNonCopyable&) = delete;
    StringNonCopyable(StringNonCopyable&&) = default;
    StringNonCopyable& operator=(const StringNonCopyable&) = delete;
    StringNonCopyable& operator=(StringNonCopyable&&) = default;
};

void TestNoCopy() {
    PriorityCollection<StringNonCopyable> strings;
    const auto white_id = strings.Add("white");
    const auto yellow_id = strings.Add("yellow");
    const auto red_id = strings.Add("red");

    strings.Promote(yellow_id);
    for (int i = 0; i < 2; ++i) {
        strings.Promote(red_id);
    }
    strings.Promote(yellow_id);
    {
        const auto item = strings.PopMax();
        ASSERT_EQUAL(item.first, "red");
        ASSERT_EQUAL(item.second, 2);
    }
    {
        const auto item = strings.PopMax();
        ASSERT_EQUAL(item.first, "yellow");
        ASSERT_EQUAL(item.second, 2);
    }
    {
        const auto item = strings.PopMax();
        ASSERT_EQUAL(item.first, "white");
        ASSERT_EQUAL(item.second, 0);
    }
}

void TestGetObjectWithEqualPriorities() {
    PriorityCollection<StringNonCopyable> strings;
    const auto white_id = strings.Add("white");
    const auto yellow_id = strings.Add("yellow");
    const auto red_id = strings.Add("red");

    {
        const auto item = strings.PopMax();
        ASSERT_EQUAL(item.first, "red");
        ASSERT_EQUAL(item.second, 0);
    }

}

//void TestAddForIterRange() {
//    PriorityCollection<StringNonCopyable> strings;

//    vector<size_t> result(3);
//    set<StringNonCopyable> pool;
//    pool.insert("1");
//    pool.insert("2");
//    pool.insert("3");
//    strings.Add(make_move_iterator(pool.begin()),
//                make_move_iterator(pool.end()),
//                result.begin());

//    ASSERT_EQUAL(result[0], 0u);
//    ASSERT_EQUAL(result[1], 1u);
//    ASSERT_EQUAL(result[2], 2u);
//}

void TheBestTest(){
    PriorityCollection<StringNonCopyable> contain;

    vector<StringNonCopyable> words;
    words.push_back("red");
    words.push_back("blue");
    words.push_back("green");
    words.push_back("yellow");
    words.push_back("white");

    vector<PriorityCollection<StringNonCopyable>::Id> check(words.size());

    contain.Add(words.begin(),words.end(), check.begin());

    //Проверка добавления вектора
    ASSERT_EQUAL(contain.Get(check[0]),"red");
    ASSERT_EQUAL(contain.Get(check[4]),"white");

    //Проверка валидности
    PriorityCollection<StringNonCopyable> spy;
    auto bomb = spy.Add({"joke"});

    ASSERT(!contain.IsValid(bomb));
    ASSERT(contain.IsValid(check[0]));
    contain.Promote(check[0]);            //red++
    contain.PopMax();                     //del red
    ASSERT(!contain.IsValid(check[0]));

    //Проверка GetMax()
    contain.Promote(check[4]);            //white++
    auto check_max = contain.GetMax();
    ASSERT_EQUAL(check_max.first,StringNonCopyable({"white"}));
    auto check_max_copy = contain.GetMax();
    ASSERT_EQUAL(check_max.first,check_max_copy.first);
    //Проверка извлечения максимума PopMax();

    ASSERT_EQUAL(check_max.first,"white");
    auto item = contain.PopMax();			//del white
    ASSERT_EQUAL(item.first,StringNonCopyable({"white"}));
    ASSERT_EQUAL(item.second,1);
    ASSERT(!contain.IsValid(check[4]));

    //проверка GetMax() по очередности добавления
    auto new_item = contain.Add({"new"});
    contain.Promote(check[3]);	// yellow++
    contain.Promote(new_item);
    contain.Promote(check[2]);	// green++
    auto x = contain.GetMax();
    ASSERT_EQUAL(x.second, 1);
    ASSERT_EQUAL(x.first, "new");
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestNoCopy);
    RUN_TEST(tr, TestGetObjectWithEqualPriorities);
    //    RUN_TEST(tr, TestAddForIterRange);
    RUN_TEST(tr, TheBestTest);
    return 0;
}

