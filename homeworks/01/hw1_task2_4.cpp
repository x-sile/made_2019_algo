/*
Дан массив целых чисел A[0..n), n не превосходит 10^8.
 Также задан размер некоторого окна (последовательно расположенных элементов массива) в этом массиве k, k<=n.
 Требуется для каждого положения окна (от 0 и до n-k) вывести значение максимума в окне.
 Скорость работы O(n log n), память O(n).
*/

#include <iostream>
#include <vector>

using namespace std;

struct Node {
    // храним здесь значение и индекс в исходной последовательности
    int index;
    int value;
};


class Heap {
public:
    explicit Heap(const vector<Node> &arr_) : arr(arr_) {}; // IDE пишет что можно std::move (а это затратно по памяти?)
    void buildHeap(int window_size);
    void insertElement(Node &node);
    Node getMax();
    void removeMax();

private:
    vector<Node> arr; // начальный массив
    int size = 0; // количество элементов в куче
    void siftDown(int i);
    void siftUp(int i);
    int leftChild(int i);
    int rightChild(int i);
    int parent(int i);
};


void Heap::siftDown(int i) {
    int left_child = leftChild(i);
    int right_child = rightChild(i);
    int child = left_child;
    if (left_child > 0 && right_child > 0 && arr[left_child].value < arr[right_child].value) {
        child = right_child; // если есть оба ребенка и правый больше, то меняем скорее с ним
    }
    if (child > 0 && arr[child].value > arr[i].value) { // ребенок б ольше текущего элемента, то меянемся
        swap(arr[i], arr[child]);
        siftDown(child);
    }
}

void Heap::siftUp(int i) {
    int par = parent(i);
    if (par >= 0 && arr[par].value < arr[i].value) { // если есть родитель и он меньше текущего элемента, то меняемся
        swap(arr[i], arr[par]);
        siftUp(par);
    }
}

void Heap::buildHeap(int window_size) {
    size = window_size;
    for (int i = size / 2 - 1; i >= 0; i--) {
        siftDown(i);
    }
}

void Heap::insertElement(Node &node) {
    arr[size++] = node;
    siftUp(size - 1);;
}

Node Heap::getMax() {
    return arr[0];
}

void Heap::removeMax() {
    // удаляем максимальный (первый) элемент, просеиваем последний элемент вниз
    swap(arr[0], arr[size - 1]);
    arr.pop_back();
    size--;
    siftDown(0);
}

int Heap::leftChild(int i) {
    int left_child = 2 * i + 1;
    if (left_child < size) {
        return left_child;
    }
    return -1; // если ребенка нет
}

int Heap::rightChild(int i) {
    int right_child = 2 * i + 2;
    if (right_child < size) {
        return right_child;
    }
    return -1; // если ребенка нет
}

int Heap::parent(int i) {
    int parent = (i - 1) / 2;
    if (i == 0) { // у корня нет родителя
        return -1;
    }
    return parent;
}


int main() {
    int n_elements = 0;
    cin >> n_elements;

    vector<Node> array;
    array.reserve(n_elements);
    for (int i = 0; i < n_elements; i++) {
        int value = 0;
        cin >> value;
        Node node{i, value};
        array.push_back(node); // заполняем вектор
    }
    int window_size = 0;
    cin >> window_size;

    // если размер окна 1, просто печатаем элементы, так быстрее
    if (window_size == 1) {
        for (int i = 0; i < n_elements; i++) {
            cout << array[i].value << " ";
        }
        return EXIT_SUCCESS;
    }

    Heap heap{array};
    heap.buildHeap(window_size);
    cout << heap.getMax().value << " ";
    for (int i = window_size; i < n_elements; i++) {
        heap.insertElement(array[i]);
        Node max_node = heap.getMax();
        // ищем максимальный элемент, если его индекс уже не в куче, то удаляем и повторяем снова до успеха
        while (max_node.index <= i - window_size) {
            heap.removeMax();
            max_node = heap.getMax();
        }
        cout << max_node.value << " ";
    }
    return EXIT_SUCCESS;
}
