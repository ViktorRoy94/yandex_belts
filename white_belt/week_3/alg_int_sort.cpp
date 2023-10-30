#include <iostream>
#include <algorithm>
#include <vector>

bool abs_comp (int i,int j) {
    return abs(i) < abs(j);
}

int main() {
    int n = 0;
    std::cin >> n;

    std::vector<int> nums(n);
    for (auto& i: nums) {
        std::cin >> i;
    }

    sort(nums.begin(), nums.end(), abs_comp);

    for (const auto& i: nums) {
        std::cout << i << " ";
    }

    return 0;
}
