#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

uint64_t CountPalindromes(const std::string& s, bool is_even) {
    int left = 0, right = -1;
    size_t sz = s.size();
    std::vector<int> dp(sz);
    for (int i = 0; i < (int)sz; ++i) {
        int k = i > right
                    ? !is_even
                    : std::min(dp[left + right - i + is_even], left - i + 1);
        while (i + k < (int)sz && i - k - is_even >= 0 &&
               s[i + k] == s[i - k - is_even]) {
            ++k;
        }
        dp[i] = k;
        if (i + k - 1 > right) {
            left = i - k + !is_even, right = i + k - 1;
        }
    }
    return std::accumulate(dp.begin(), dp.end(), 0ULL);
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
    std::string s;
    std::cin >> s;
    std::cout << CountPalindromes(s, false) + CountPalindromes(s, true) -
                     static_cast<uint64_t>(s.size());
    return 0;
}
