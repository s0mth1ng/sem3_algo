#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <string>
#include <vector>

void SortSuffixesByLength(std::vector<size_t>& suf,
                          const std::vector<size_t>& cls,
                          size_t length) {
    size_t size = suf.size();
    std::vector<size_t> tmp_suf(size);
    std::vector<size_t> tmp_counter(size);
    for (size_t i = 0; i < size; ++i) {
        tmp_suf[i] = (suf[i] - length + size) % size;
    }
    for (size_t i = 0; i < size; ++i) {
        tmp_counter[cls[tmp_suf[i]]]++;
    }
    for (size_t i = 1; i < size; ++i) {
        tmp_counter[i] += tmp_counter[i - 1];
    }
    for (int i = static_cast<int>(size) - 1; i >= 0; --i) {
        suf[--tmp_counter[cls[tmp_suf[i]]]] = tmp_suf[i];
    }
}

void UpdateClassesByLength(const std::vector<size_t>& suf,
                           std::vector<size_t>& cls,
                           size_t length) {
    size_t size = suf.size();
    std::vector<size_t> tmp_classes(size);
    tmp_classes[suf[0]] = 0;
    size_t classes_count = 1;
    for (size_t i = 1; i < size; ++i) {
        size_t lhs = (suf[i] + length) % size;
        size_t rhs = (suf[i - 1] + length) % size;
        if (cls[suf[i]] != cls[suf[i - 1]] || cls[lhs] != cls[rhs]) {
            ++classes_count;
        }
        tmp_classes[suf[i]] = classes_count - 1;
    }
    std::copy(begin(tmp_classes), end(tmp_classes), begin(cls));
}

std::vector<size_t> ComputeSuffixArray(std::string str) {
    str.push_back(0);

    size_t size = str.size();
    std::vector<size_t> cls(size), suf(size);

    // sorting single chars
    size_t classes_count = 0;
    size_t suf_ind = 0;
    std::vector<std::vector<size_t>> ind_by_char(256);
    for (size_t ind = 0; ind < size; ++ind) {
        ind_by_char[str[ind]].push_back(ind);
    }
    for (size_t i = 0; i < ind_by_char.size(); ++i) {
        for (size_t ind : ind_by_char[i]) {
            cls[ind] = classes_count;
            suf[suf_ind++] = ind;
        }
        classes_count += !ind_by_char[i].empty();
    }

    for (size_t length = 1; length < size; length <<= 1lu) {
        SortSuffixesByLength(suf, cls, length);
        UpdateClassesByLength(suf, cls, length);
    }

    return {begin(suf) + 1, end(suf)};
}

std::vector<size_t> ComputeLCP(const std::string& str,
                               const std::vector<size_t>& suf_array) {
    size_t size = suf_array.size();
    std::vector<size_t> reverse_suf(size);
    for (size_t i = 0; i < size; ++i) {
        reverse_suf[suf_array[i]] = i;
    }

    std::vector<size_t> lcp(size);
    size_t cur_lcp = 0;
    for (size_t i = 0; i < size; ++i) {
        if (reverse_suf[i] + 1 == size) {
            cur_lcp = 0;
            continue;
        }
        size_t start = suf_array[reverse_suf[i] + 1];
        while (std::max(i, start) + cur_lcp < size &&
               str[i + cur_lcp] == str[start + cur_lcp]) {
            cur_lcp++;
        }
        lcp[reverse_suf[i]] = cur_lcp;
        if (cur_lcp) {
            cur_lcp--;
        }
    }

    return lcp;
}

void solve(std::istream& in, std::ostream& os) {
    std::string s, t;
    in >> s >> t;
    uint64_t k;
    in >> k;

    std::string data = s + '#' + t;
    size_t size = data.size();
    auto suf_arr = ComputeSuffixArray(data);
    auto lcp = ComputeLCP(data, suf_arr);

    size_t common_length = 0;
    uint64_t counter = 0;
    std::string ans = "-1";
    for (size_t i = 0; i + 1 < size; ++i) {
        auto [left, right] = std::minmax(suf_arr[i], suf_arr[i + 1]);
        bool isDiffParts = (left <= s.size() && right > s.size());
        if (!isDiffParts) {
            common_length = std::min(common_length, lcp[i]);
        } else {
            counter += (lcp[i] > common_length ? lcp[i] - common_length : 0lu);
            common_length = lcp[i];
        }
        if (counter >= k) {
            ans = data.substr(suf_arr[i], lcp[i] - counter + k);
            break;
        }
    }

    os << ans << '\n';
}

int main() {
#ifdef LOCAL
    std::ifstream fin("input.txt");
    solve(fin, std::cout);
#else
    // ~~~~~~~~~~~~~~~ fast io ~~~~~~~~~~~~~~~~~~
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    solve(std::cin, std::cout);
#endif
    return 0;
}
