#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <tuple>

template <typename T> T Sqr(const T& x);
template <typename T> std::vector<T> Sqr(const std::vector<T>& a);
template <typename F, typename S> std::pair<F, S> Sqr(const std::pair<F, S>& a);
template <typename K, typename V> std::map<K, V> Sqr(const std::map<K, V>& a);


template <typename T>
std::vector<T> Sqr(const std::vector<T>& a) {
    std::vector<T> result(a.size());
    for (size_t i = 0; i < a.size(); i++) {
        result[i] = Sqr(a[i]);
    }
    return result;
}

template <typename F, typename S>
std::pair<F, S> Sqr(const std::pair<F, S>& a) {
    return {Sqr(a.first), Sqr(a.second)};
}

template <typename K, typename V>
std::map<K, V> Sqr(const std::map<K, V>& a) {
    std::map<K, V> result;
    for (const auto& it: a) {
        result[it.first] = Sqr(it.second);
    }
    return result;
}

template <typename T>
T Sqr(const T& x) {
    return x * x;
}


#ifdef DEBUG
int main() {
    std::vector<int> v = {1, 2, 3};
    std::cout << "vector:";
    for (int x : Sqr(v)) {
      std::cout << ' ' << x;
    }
    std::cout << std::endl;

    std::map<int, std::pair<int, int>> map_of_pairs = {
      {4, {2, 2}},
      {7, {4, 3}}
    };
    std::cout << "map of pairs:" << std::endl;
    for (const auto& x : Sqr(map_of_pairs)) {
      std::cout << x.first << ' ' << x.second.first << ' ' << x.second.second << std::endl;
    }
    return 0;
}
#endif
