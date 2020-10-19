#include <bits/stdc++.h>

using namespace std;

vector<int> prefix_function(string s) {
    int n = (int) s.length();
    vector<int> pi(n);
    for (int i = 1; i < n; ++i) {
        int j = pi[i - 1];
        while (j > 0 && s[i] != s[j])
            j = pi[j - 1];
        if (s[i] == s[j]) ++j;
        pi[i] = j;
    }
    return pi;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);
#ifndef ONLINE_JUDGE
    freopen("input.txt", "r", stdin);
    // freopen("output.txt", "w", stdout);
#endif
    string t, s;
    cin >> s >> t;
    auto pi = prefix_function(s + "#" + t);
    int n = s.size();
    for (int i = n + 1; i < pi.size(); ++i) {
        if (pi[i] == n) {
            cout << i - 2 * n << ' ';
        }
    }
}
