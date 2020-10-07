#include <iostream>
#include <numeric>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

uint64_t count_palindromes(const string &s, bool is_even) {
    int l = 0, r = -1;
    size_t sz = s.size();
    vector<int> dp(sz);
    for (int i = 0; i < sz; ++i) {
        int k = i > r ? !is_even : min(dp[l + r - i + is_even], r - i + 1);
        while (i + k < sz && i - k - is_even >= 0 && s[i + k] == s[i - k - is_even])
            ++k;
        dp[i] = k;
        if (i + k - 1 > r)
            l = i - k + !is_even, r = i + k - 1;
    }
    return accumulate(dp.begin(), dp.end(), 0ULL);
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);
#ifndef ONLINE_JUDGE
    freopen("input.txt", "r", stdin);
    // freopen("output.txt", "w", stdout);
#endif
    string s;
    cin >> s;
    cout << count_palindromes(s, false) + count_palindromes(s, true) - static_cast<uint64_t>(s.size());
    return 0;
}