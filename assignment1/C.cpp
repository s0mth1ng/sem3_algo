#include <iostream>
#include <numeric>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include <unordered_map>

//#include "profile.h"

using namespace std;

struct Word {
    string s;
    size_t pos;
};


vector<Word> ParseMask(const string &mask) {
    vector<Word> result;
    for (size_t ind = 0; ind < mask.size();) {
        size_t next = mask.find('?', ind);
        if (next == string::npos) {
            next = mask.size();
        }
        if (next > ind) {
            result.push_back({mask.substr(ind, next - ind), ind});
        }
        ind = next + 1;
    }
    return result;
}

class AhoCorasick {
private:
    struct Vertex;
    using VertexPtr = shared_ptr<Vertex>;

    struct Vertex {

        unordered_map<char, VertexPtr> to, go;
        vector<size_t> inds;
        VertexPtr link, parent;
        char parent_char;
        bool is_terminal = false;

        explicit Vertex(char edge, VertexPtr p = VertexPtr()) {
            parent_char = edge;
            parent = move(p);
        }
    };

    shared_ptr<Vertex> root = make_shared<Vertex>(-1);

    vector<VertexPtr> out(VertexPtr v);

public:
    void addPattern(const string &p, size_t ind);

    VertexPtr go(const VertexPtr &from, char edge);

    VertexPtr link(const VertexPtr &from);

    void processText(const string &text, vector<int> &cnt, const vector<Word> &words) {
        auto current = root;
        for (size_t i = 0; i < text.size(); ++i) {
            current = go(current, text[i]);
            auto terminal = out(current);
            for (const auto &v : terminal) {
                for (size_t ind : v->inds) {
                    if (i + 1 >= words[ind].pos + words[ind].s.size()) {
                        cnt[i + 1 - words[ind].pos - words[ind].s.size()]++;
                    }
                }
            }
        }
    }
};

void AhoCorasick::addPattern(const string &p, size_t ind) {
    shared_ptr current = root;
    for (char c : p) {
        if (!current->to.count(c)) {
            current->to[c] = make_shared<Vertex>(c, current);
        }
        current = current->to.at(c);
    }
    current->is_terminal = true;
    current->inds.push_back(ind);
}

AhoCorasick::VertexPtr AhoCorasick::go(const AhoCorasick::VertexPtr &from, char edge) {
    if (!from->go.count(edge)) {
        if (from->to.count(edge)) {
            from->go[edge] = from->to.at(edge);
        } else if (from == root) {
            from->go[edge] = root;
        } else {
            from->go[edge] = go(link(from), edge);
        }
    }
    return from->go.at(edge);
}

AhoCorasick::VertexPtr AhoCorasick::link(const AhoCorasick::VertexPtr &from) {
    if (!from->link) {
        if (from == root || from->parent == root) {
            from->link = root;
        } else {
            from->link = go(link(from->parent), from->parent_char);
        }
    }
    return from->link;
}

vector<AhoCorasick::VertexPtr> AhoCorasick::out(AhoCorasick::VertexPtr v) {
    vector<VertexPtr> res;
    if (!v->is_terminal) {
        v = link(v);
    }
    while (v != root) {
        if (v->is_terminal) {
            res.push_back(v);
        }
        v = link(v);
    }
    return res;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);
#ifndef ONLINE_JUDGE
    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);
#endif
    string mask;
    string text;
    cin >> mask >> text;
    auto words = ParseMask(mask);
    AhoCorasick trie;
    for (size_t i = 0; i < words.size(); ++i) {
        trie.addPattern(words[i].s, i);
    }

    vector<int> cnt(text.size());
    {
//        LOG_DURATION("processing");
        trie.processText(text, cnt, words);
    }
    for (int i = 0; i + mask.size() <= cnt.size(); ++i) {
        if (cnt[i] == words.size()) {
            cout << i << ' ';
        }
    }
    return 0;
}