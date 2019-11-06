/*
Формат входных данных.
В первой строке количество команд n. n ≤ 1000000.
Каждая команда задаётся как 2 целых числа: a b.
a = 1 - push front
a = 2 - pop front
a = 3 - push back
a = 4 - pop back
Для очереди используются команды 2 и 3. Для дека используются все четыре команды.
Если дана команда pop*, то число b - ожидаемое значение.Если команда pop вызвана для пустой структуры данных, то ожидается “-1”.
Формат выходных данных.
Требуется напечатать YES - если все ожидаемые значения совпали. Иначе, если хотя бы одно ожидание не оправдалось, то напечатать NO.

Реализовать дек с динамическим зацикленным буфером.
*/

#include <iostream>
#include <cassert>

using namespace std;

class Deque {
public:
    void pushBack(int value);
    void pushFront(int value);
    int popBack();
    int popFront();
    bool isEmpty() const;
    ~Deque();

private:
    int capacity = 8; // количество зарезервированных элементов в массиве, изначально 8
    int size = 0; // текущее количество элементов в массиве
    int head = 0; // индекс головы
    int tail = 0; // индекс хвоста
    // tail - при увеличении идет налево, head - направо
    // самый левый индекс массива буду называть начало, самый правый - конец
    int *buffer = new int[capacity]; // инициализируем массив
    void handleBuffer(); // метод, удваивающий буффер при необходимости
};

void Deque::pushBack(int value) {
    handleBuffer();
    if (isEmpty()) {
        buffer[tail] = value;
    } else {
        if (tail == 0) { // если tail дошел до начала массива, перекидываем на конец
            tail = capacity;
        }
        tail--;
        buffer[tail] = value;
    }
    size++;
}

void Deque::pushFront(int value) {
    handleBuffer();
    if (isEmpty()) {
        buffer[head] = value;
    } else {
        if (head == capacity - 1) { // если head дошел до конца массива, перекидываем на начала
            head = -1;
        }
        head++;
        buffer[head] = value;
    }
    size++;
}

int Deque::popBack() {
    assert(!isEmpty());
    size--;
    if (isEmpty()) {
        return buffer[tail]; // если в массиве один элемент, возвращаем его
    } else {
        if (tail == capacity - 1) { // если хвост в конце массива, перекидываем на начало
            tail = 0;
            return buffer[capacity - 1];
        }
    }
    return buffer[tail++];
}

int Deque::popFront() {
    assert(!isEmpty());
    size--;
    if (head == tail) {
        return buffer[head];
    } else {
        if (head == 0) { // если голова в начале массива, перекидываем на конец
            head = capacity - 1;
            return buffer[0];
        }
    }
    return buffer[head--];
}

bool Deque::isEmpty() const{
    return size == 0;
}

Deque::~Deque() {
    delete[] buffer;
}

void Deque::handleBuffer() {
    if (size == capacity) { // если полностью заполнили массив, удваиваем буффер
        capacity *= 2;
        int *tmp = new int[capacity];
        copy(buffer + tail, buffer + size, tmp); // копируем элементы от хвоста до конца
        if (head < tail) {
            copy(buffer, buffer + tail, tmp + size - tail); // копируем элементы от начала до хвоста
        }
        // освобождаем память старого массива, ставим правильные индексы
        delete[] buffer;
        buffer = tmp;
        tail = 0;
        head = size - 1;
    }
}


int main() {
    int commands_count = 0;
    cin >> commands_count;

    Deque deque;
    for (int i = 0; i < commands_count; i++) {
        int command = 0;
        int value = 0;
        cin >> command >> value;
        if (command == 1) {
            deque.pushFront(value);
        } else if (command == 3) {
            deque.pushBack(value);
        } else {
            if (deque.isEmpty()) {
                if (value != -1) {
                    cout << "NO";
                    return EXIT_SUCCESS;
                }
            } else {
                if (((command == 2) && (deque.popFront() != value)) || ((command == 4) && (deque.popBack() != value))) {
                    cout << "NO";
                    return EXIT_SUCCESS;
                }
            }
        }
    }
    cout << "YES";
    return EXIT_SUCCESS;
}
