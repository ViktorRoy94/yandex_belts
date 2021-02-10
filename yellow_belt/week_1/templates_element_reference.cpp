#include <iostream>
#include <string>
#include <map>

template <typename K, typename V> V& GetRefStrict(const std::map<K, V>& a, K key);


template <typename K, typename V>
V& GetRefStrict(std::map<K, V>& a, K key) {
    if (a.count(key) == 0) {
        throw std::runtime_error("No such key in the map");
    }
    return a[key];
}


#ifdef LOCAL_BUILD
int main() {
    std::map<int, std::string> m = {{0, "value"}};
    std::string& item = GetRefStrict(m, 0);
    item = "newvalue";
    std::cout << m[0] << std::endl; // выведет newvalue
    return 0;
}
#endif
