/*
    Задача 8. Хеш-таблица (10 баллов)
Реализуйте структуру данных типа “множество строк” на основе динамической хеш-таблицы с открытой адресацией.
Хранимые строки непустые и состоят из строчных латинских букв.
Хеш-функция строки должна быть реализована с помощью вычисления значения многочлена методом Горнера.
Начальный размер таблицы должен быть равным 8-ми. Перехеширование выполняйте при добавлении элементов в случае,
когда коэффициент заполнения таблицы достигает 3/4.
Структура данных должна поддерживать операции добавления строки в множество,
удаления строки из множества и проверки принадлежности данной строки множеству.
    Формат входных данных
Каждая строка входных данных задает одну операцию над множеством.
Запись операции состоит из типа операции и следующей за ним через пробел строки, над которой проводится операция.
Тип операции  – один из трех символов:
    +  означает добавление данной строки в множество;
    -  означает удаление  строки из множества;
    ?  означает проверку принадлежности данной строки множеству.
При добавлении элемента в множество НЕ ГАРАНТИРУЕТСЯ, что он отсутствует в этом множестве.
При удалении элемента из множества НЕ ГАРАНТИРУЕТСЯ, что он присутствует в этом множестве.
    Формат выходных данных
Программа должна вывести для каждой операции одну из двух строк OK или FAIL,
в зависимости от того, встречается ли данное слово в нашем множестве.
    1_2. Для разрешения коллизий используйте двойное хеширование.
*/

#include <cassert>
#include <iostream>
#include <string>
#include <stdexcept>

class HashTable {
public:
    explicit HashTable(size_t initial_size);
    HashTable(const HashTable &) = delete;
    HashTable(HashTable &&) = delete;
    HashTable &operator=(const HashTable &) = delete;
    HashTable &operator=(HashTable &&) = delete;
    bool Has(const std::string &key) const;
    bool Add(const std::string &key);
    bool Remove(const std::string &key);
    ~HashTable();

private:
    float LOAD_FACTOR = 0.75; // заполненность таблицы
    size_t size = 0; // текущее число элементов в таблице
    size_t capacity = 0; // максимальное число элементов в таблице
    std::string DELETED = "DELETED"; // спец символ для удаленных строк
    int hash1_mult = 31; // java hash number
    int hash2_mult = 1; // вторую хэш-функцию сделаем проще, просто суммируем все строки
    size_t hash(const std::string &str, int mult) const;
    void rehash();

    struct HashTableNode {
        explicit HashTableNode(std::string key_) : key(std::move(key_)) {}
        std::string key;
    };

    HashTableNode **table;
};

HashTable::HashTable(size_t initial_size) {
    capacity = initial_size;
    table = new HashTableNode *[capacity];

    for (size_t i = 0; i < capacity; i++) {
        table[i] = nullptr;
    }
}

bool HashTable::Has(const std::string &key) const {
    assert(!key.empty());

    if (key == DELETED) {
        throw std::invalid_argument("Exactly such string is not allowed");
    }

    // можно для оптимизации вычислять hash2, только если не нашли по hash1, но так меньше кода
    const size_t hash1 = hash(key, hash1_mult);
    const size_t hash2 = hash(key, hash2_mult);
    for (size_t i = 0; i < capacity; i++) {
        size_t hash_ = (hash1 + (2 * hash2 + 1) * i) % capacity; // вторая хэш-функция должна быть нечетной
        if (!table[hash_]) {
            return false;
        } else if (table[hash_]->key == key) {
            return true;
        }
    }
    return false;
}


bool HashTable::Add(const std::string &key) {
    assert(!key.empty());

    if (key == DELETED) {
        throw std::invalid_argument("Exactly such string is not allowed");
    }

    if (((float) size / capacity) >= LOAD_FACTOR) {
        rehash();
    }

    const size_t hash1 = hash(key, hash1_mult);
    const size_t hash2 = hash(key, hash2_mult);
    int insert_index = -1; // место, куда вставить элемент (т.к. некоторые ноды могут быть удаленным, надо проверить все)
    for (size_t i = 0; i < capacity; i++) {
        size_t hash_ = (hash1 + (2 * hash2 + 1) * i) % capacity;
        if (!table[hash_]) {
            if (insert_index == -1) { // еще не нашли места для вставки
                insert_index = hash_;
            }
            break;
        } else if (table[hash_]->key == DELETED) {
            if (insert_index == -1) {
                insert_index = hash_;
            }
        } else if (table[hash_]->key == key) {
            return false;
        }
    }
    table[insert_index] = new HashTableNode(key);
    size++;
    return true;
}

bool HashTable::Remove(const std::string &key) {
    assert(!key.empty());

    if (key == DELETED) {
        throw std::invalid_argument("Exactly such string is not allowed");
    }

    const size_t hash1 = hash(key, hash1_mult);
    const size_t hash2 = hash(key, hash2_mult);
    for (size_t i = 0; i < capacity; i++) {
        size_t hash_ = (hash1 + (2 * hash2 + 1) * i) % capacity;
        if (!table[hash_]) {
            return false;
        } else if (table[hash_]->key == key) {
            table[hash_]->key = DELETED;
            size--;
            return true;
        }
    }
    return false;
}

size_t HashTable::hash(const std::string &str, int mult) const {
    size_t hash = 0;
    for (char ch : str)
        hash = (hash * mult + ch) % capacity;
    return hash;
}

void HashTable::rehash() {
    capacity *= 2;
    size = 0;
    HashTableNode **newTable = new HashTableNode *[capacity]; // сюда перевешиваем ненулевые и неудаленные ноды

    for (size_t i = 0; i < capacity; i++) {
        newTable[i] = nullptr;
    }

    for (size_t i = 0; i < capacity / 2; i++) {
        if (!table[i] || (table[i]->key == DELETED)) {
            delete table[i];
        } else {
            const size_t hash1 = hash(table[i]->key, hash1_mult);
            const size_t hash2 = hash(table[i]->key, hash2_mult);
            for (size_t j = 0; j < capacity; j++) {
                size_t hash_ = (hash1 + (2 * hash2 + 1) * j) % capacity;
                if (!newTable[hash_]) {
                    newTable[hash_] = table[i];
                    size++;
                    break;
                }
            }
        }
    }
    delete[] table;
    table = newTable;
}


HashTable::~HashTable() {
    for (size_t i = 0; i < capacity; i++) {
        delete table[i];
    }
    delete[] table;
}

int main() {
    HashTable table(8);
    char command = ' ';
    std::string value;
    while (std::cin >> command >> value) {
        switch (command) {
            case '?':
                std::cout << (table.Has(value) ? "OK" : "FAIL") << std::endl;
                break;
            case '+':
                std::cout << (table.Add(value) ? "OK" : "FAIL") << std::endl;
                break;
            case '-':
                std::cout << (table.Remove(value) ? "OK" : "FAIL") << std::endl;
                break;
        }
    }
    return 0;
}
