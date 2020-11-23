#include <algorithm>
#include <iostream>
#include <memory>
#include <numeric>
#include <string>
#include <unordered_map>
#include <vector>

struct Word {
    std::string s;
    uint32_t pos;
};

std::vector<Word> ParseMask(const std::string& mask) {
    std::vector<Word> result;
    for (uint32_t ind = 0; ind < mask.size();) {
        size_t next = mask.find('?', ind);
        if (next == std::string::npos) {
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
    using VertexPtr = std::shared_ptr<Vertex>;

    struct Vertex {
        std::unordered_map<char, VertexPtr> to, go;
        std::vector<uint32_t> inds;
        VertexPtr link, parent;
        char parent_char;
        bool is_terminal = false;

        explicit Vertex(char edge, VertexPtr p = VertexPtr()) {
            parent_char = edge;
            parent = move(p);
        }
    };

    std::shared_ptr<Vertex> _root = std::make_shared<Vertex>(-1);

    std::vector<VertexPtr> GetTerminalNodes(VertexPtr v);
    VertexPtr go(const VertexPtr& from, char edge);
    VertexPtr link(const VertexPtr& from);

   public:
    struct OccurrenceInfo {
        uint32_t pos;
        std::vector<uint32_t> words_ids;

        explicit OccurrenceInfo(uint32_t pos) : pos(pos) {}
    };

    void AddPattern(const std::string& p, uint32_t ind);
    std::vector<OccurrenceInfo> GetAllOccurrences(const std::string& text);
};

void AhoCorasick::AddPattern(const std::string& p, uint32_t ind) {
    auto current = _root;
    for (char c : p) {
        if (!current->to.count(c)) {
            current->to[c] = std::make_shared<Vertex>(c, current);
        }
        current = current->to.at(c);
    }
    current->is_terminal = true;
    current->inds.push_back(ind);
}

AhoCorasick::VertexPtr AhoCorasick::go(const AhoCorasick::VertexPtr& from,
                                       char edge) {
    if (!from->go.count(edge)) {
        if (from->to.count(edge)) {
            from->go[edge] = from->to.at(edge);
        } else if (from == _root) {
            from->go[edge] = _root;
        } else {
            from->go[edge] = go(link(from), edge);
        }
    }
    return from->go.at(edge);
}

AhoCorasick::VertexPtr AhoCorasick::link(const AhoCorasick::VertexPtr& from) {
    if (!from->link) {
        if (from == _root || from->parent == _root) {
            from->link = _root;
        } else {
            from->link = go(link(from->parent), from->parent_char);
        }
    }
    return from->link;
}

std::vector<AhoCorasick::OccurrenceInfo> AhoCorasick::GetAllOccurrences(
    const std::string& text) {
    std::vector<AhoCorasick::OccurrenceInfo> occurrences;
    auto current = _root;
    for (uint32_t i = 0; i < text.size(); ++i) {
        AhoCorasick::OccurrenceInfo info(i);
        current = go(current, text[i]);
        auto terminal = GetTerminalNodes(current);
        for (const auto& v : terminal) {
            info.words_ids.insert(info.words_ids.end(), v->inds.begin(),
                                  v->inds.end());
        }
        occurrences.push_back(info);
    }
    return occurrences;
}

std::vector<AhoCorasick::VertexPtr> AhoCorasick::GetTerminalNodes(
    AhoCorasick::VertexPtr v) {
    std::vector<VertexPtr> res;
    if (!v->is_terminal) {
        v = link(v);
    }
    while (v != _root) {
        if (v->is_terminal) {
            res.push_back(v);
        }
        v = link(v);
    }
    return res;
}

void Solve(std::istream& in, std::ostream& out) {
    std::string mask;
    std::string text;
    in >> mask >> text;
    auto words = ParseMask(mask);
    AhoCorasick trie;
    for (uint32_t i = 0; i < words.size(); ++i) {
        trie.AddPattern(words[i].s, i);
    }

    auto occurrences = trie.GetAllOccurrences(text);
    std::vector<uint32_t> cnt(text.size());
    for (const auto& info : occurrences) {
        for (auto ind : info.words_ids) {
            if (info.pos + 1 >= words[ind].pos + words[ind].s.size()) {
                cnt[info.pos + 1 - words[ind].pos - words[ind].s.size()]++;
            }
        }
    }
    for (int i = 0; i + mask.size() <= cnt.size(); ++i) {
        if (cnt[i] == words.size()) {
            out << i << ' ';
        }
    }
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
    Solve(std::cin, std::cout);
    return 0;
}
