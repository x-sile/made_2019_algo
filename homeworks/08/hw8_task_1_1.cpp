/*
Найдите все вхождения шаблона в строку. Длина шаблона – p, длина строки ­– n. Время O(n + p), доп. память – O(p).
Вариант 1. С помощью префикс-функции (алгоритм Кнута-Морриса-Пратта).
    Формат входного файла
Шаблон, символ перевода строки, строка.
    Формат выходного файла
Позиции вхождения шаблона в строке.
 */

#include <iostream>
#include <vector>

std::vector<int> KMP(const std::string &pattern, const std::string &str) {
    std::vector<int> result;
    std::vector<int> pattern_prefix(pattern.size());

    size_t cur_prefix_value = 0;
    pattern_prefix[0] = cur_prefix_value;
    size_t i = 1;
    while (i < pattern.size()) {
        if (pattern[i] == pattern[cur_prefix_value]) {
            cur_prefix_value++;
        } else {
            while (cur_prefix_value > 0) {
                cur_prefix_value = pattern_prefix[cur_prefix_value - 1];
                if (pattern[i] == pattern[cur_prefix_value]) {
                    cur_prefix_value++;
                    break;
                }
            }
        }
        pattern_prefix[i] = cur_prefix_value;
        i++;
    }

    size_t match_length = 0;
    size_t j = 0;
    while (j < str.size()) {
        if (str[j] == pattern[match_length]) {
            match_length++;
        } else {
            while (match_length > 0) {
                match_length = pattern_prefix[match_length - 1];
                if (str[j] == pattern[match_length]) {
                    match_length++;
                    break;
                }
            }
        }
        j++;
        if (match_length == pattern.size()) {
            result.push_back(j - match_length);
            match_length = pattern_prefix[match_length - 1];
        }
    }
    return result;
}

int main() {
    std::string pattern, str;

    std::cin >> pattern >> str;

    auto result = KMP(pattern, str);
    for (auto prefix_value: result) {
        std::cout << prefix_value << " ";
    }
    return EXIT_SUCCESS;
}
