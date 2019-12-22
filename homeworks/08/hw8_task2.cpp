/*
Шаблон поиска задан строкой длины m, в которой кроме обычных символов могут встречаться символы “?”.
Найти позиции всех вхождений шаблона в тексте длины n. Каждое вхождение шаблона предполагает,
что все обычные символы совпадают с соответствующими из текста, а вместо символа “?” в тексте встречается
произвольный символ. Гарантируется, что сам “?” в тексте не встречается.
Время работы - O(n + m + Z), где Z - общее число вхождений подстрок шаблона “между вопросиками” в исходном тексте.
(Ахо-Корасик)
m ≤ 5000, n ≤ 2000000. Время: 10с, память 32Mb.
Формат входного файла
    Шаблон, символ перевода строки, строка.
Формат выходного файла
Позиции вхождения шаблона в строке.
*/

#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <vector>

struct Node {
    std::map<char, std::shared_ptr<Node>> go;
    std::shared_ptr<Node> parent = nullptr;
    std::shared_ptr<Node> suffLink = nullptr;
    std::shared_ptr<Node> terminalLink = nullptr;
    std::vector<int> offsets; // оффсет с начала паттерна
    char symbol;
    bool is_terminal = false;
};

class Trie {
public:
    Trie(int pattern_size_);

    Trie(const Trie &) = delete;

    Trie(Trie &&) = delete;

    Trie &operator=(const Trie &) = delete;

    Trie &operator=(Trie &&) = delete;

    ~Trie() = default;

    void add(const std::string &key, int offset);

    std::vector<int> find(const std::string &str);

private:
    std::shared_ptr<Node> getLink(std::shared_ptr<Node> &node, char symbol);

    std::shared_ptr<Node> getSuffLink(std::shared_ptr<Node> &node);

    std::shared_ptr<Node> getTerminalLink(std::shared_ptr<Node> &node);

    std::shared_ptr<Node> root;
    int n_patterns = 0;
    int pattern_size = 0;
};

Trie::Trie(int pattern_size_) {
    root = std::make_shared<Node>();
    pattern_size = pattern_size_;
}

void Trie::add(const std::string &key, int offset) {
    auto current = root;
    for (auto symbol : key) {
        auto next = current->go.find(symbol);
        if (next == current->go.end()) {
            auto parent = current;
            current = current->go[symbol] = std::make_shared<Node>();
            current->parent = parent;
            current->symbol = symbol;
        } else {
            current = next->second;
        }
    }
    current->is_terminal = true;
    current->offsets.push_back(offset);
    n_patterns++;
}

std::shared_ptr<Node> Trie::getLink(std::shared_ptr<Node> &node, char symbol) {
    if (node->go.find(symbol) == node->go.end()) {
        if (node == root) {
            node->go[symbol] = root;
        } else {
            auto sufflink = getSuffLink(node);
            node->go[symbol] = getLink(sufflink, symbol);
        }
    }
    return node->go[symbol];
}

std::shared_ptr<Node> Trie::getSuffLink(std::shared_ptr<Node> &node) {
    if (!node->suffLink) {
        if ((node == root) || (node->parent == root)) {
            node->suffLink = root;
        } else {
            auto parent_sufflink = getSuffLink(node->parent);
            node->suffLink = getLink(parent_sufflink, node->symbol);
        }
    }
    return node->suffLink;
}

std::shared_ptr<Node> Trie::getTerminalLink(std::shared_ptr<Node> &node) {
    if (!node->terminalLink) {
        auto sufflink = getSuffLink(node);
        if ((sufflink == root) || (sufflink->is_terminal)) {
            node->terminalLink = sufflink;
        } else {
            node->terminalLink = getTerminalLink(sufflink);
        }
    }
    return node->terminalLink;
}


std::vector<int> Trie::find(const std::string &str) {
    std::vector<int> result;
    std::vector<int> patterns_found(str.size());

    auto node = root;
    for (int i = 0; i < str.size(); i++) {
        node = getLink(node, str[i]);
        auto current = node;
        while (current != root) {
            if (!current->is_terminal) {
                current = getTerminalLink(current);
                continue;
            }
            for (auto offset: current->offsets) {
                int index = i - offset + 1;
                if ((index >= 0) && (index + pattern_size <= str.size())) {
                    patterns_found[index]++;
                    if (patterns_found[index] == n_patterns) {
                        result.emplace_back(index);
                    }
                }
            }
            current = getTerminalLink(current);
        }
    }
    return result;
}


int main() {
    std::string pattern, str;
    std::cin >> pattern >> str;

    if (std::all_of(pattern.begin(), pattern.end(), [](char symbol) { return symbol == '?'; })) {
        for (int i = 0; i < str.size() - pattern.size() + 1; i++) {
            std::cout << i << " ";
        }
        return EXIT_SUCCESS;
    }

    Trie trie(pattern.size());
    int offset = 0;
    std::string cur_pattern;
    for (auto symbol: pattern) {
        if (symbol != '?') {
            cur_pattern += symbol;
        } else if (!cur_pattern.empty()) {
            trie.add(cur_pattern, offset);
            cur_pattern = "";
        }
        offset++;
    }
    if (!cur_pattern.empty()) {
        trie.add(cur_pattern, offset);
    }

    auto result = trie.find(str);
    for (auto index: result) {
        std::cout << index << " ";
    }

    return EXIT_SUCCESS;
}
