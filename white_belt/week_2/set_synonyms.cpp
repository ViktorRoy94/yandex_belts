#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <string>


int main() {
#ifdef DEBUG
    std::ifstream in("tests/set_synonyms_test_1.txt");
    std::cin.rdbuf(in.rdbuf());
#endif

    std::map<std::string, std::set<std::string>> dict;

    int n;
    std::cin >> n;
    for (int i=0; i < n; i++ ) {
        std::string op;
        std::cin >> op;
        if (op == "ADD") {
            std::string word1, word2;
            std::cin >> word1 >> word2;

            dict[word1].insert(word2);
            dict[word2].insert(word1);

        }
        if (op == "COUNT") {
            std::string word;
            std::cin >> word;

            std::cout << dict[word].size() << std::endl;

        }
        if (op == "CHECK") {
            std::string word1, word2;
            std::cin >> word1 >> word2;

            if (dict[word1].count(word2) == 1) {
                std::cout << "YES" << std::endl;
            }
            else {
                std::cout << "NO" << std::endl;
            }
        }
    }
    return 0;
}
