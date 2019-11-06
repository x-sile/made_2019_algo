/*
3_1. Первые k элементов длинной последовательности.
Дана очень длинная последовательность целых чисел длины n.
 Требуется вывести в отсортированном виде её первые k элементов.
 Последовательность может не помещаться в память. Время работы O(n * log(k)). Память O(k). Использовать слияние.
*/

#include <iostream>
#include <cstdlib>
#include <cstring>

using namespace std;

void merge(const int *leftArr, int leftArrSize, const int *rightArr, int rightArrSize, int *tmp, int tmp_size) {
    int i = 0;
    int j = 0;
    int size = 0; // количество элементов в tmp
    // из двух массивов берем максимум (он уже слева) и сдвигаем итераторы
    // если в одном массиве не осталось элементов, берем все из второго
    while (size < tmp_size) {
        if (i == leftArrSize) {
            memcpy(&tmp[size], &rightArr[j], sizeof(int) * (tmp_size - size));
            size = tmp_size;
        } else if (j == rightArrSize) {
            memcpy(&tmp[size], &leftArr[i], sizeof(int) * (tmp_size - size));
            size = tmp_size;
        } else if (leftArr[i] <= rightArr[j]) {
            tmp[size++] = leftArr[i++];
        } else {
            tmp[size++] = rightArr[j++];
        }
    }
}

void sort_top_k(int *arr, int size, int top_k) {
    if (size <= 1) { // если элемент один - возваращем исходные массив (тк сортируем inplace)
        return;
    }
    // это по сути код с лекции, только merge принимает еще параметр top_k, тк мержим только первые top_k эелементов
    int leftSize = size / 2;
    int rightSize = size - leftSize;
    sort_top_k(arr, leftSize, top_k);
    sort_top_k(arr + leftSize, rightSize, top_k);
    top_k = min(top_k, leftSize + rightSize); // если массивы маленькие, то мержим полностью
    int *tmp = new int[top_k]; // создаем массив размер окна, куда будем складывать top_k элементы
    merge(arr, leftSize, arr + leftSize, rightSize, tmp, top_k);
    memcpy(arr, tmp, sizeof(int) * top_k);
    delete[] tmp;

}


int main() {
    int size = 0;
    int top_k = 0;
    cin >> size >> top_k;

    int *arr = new int[size];
    for (int i = 0; i < size; i++) {
        int value = 0;
        cin >> value;
        arr[i] = value;
    }
    sort_top_k(arr, size, top_k);
    for (int i = 0; i < top_k; i++) {
        cout << arr[i] << " ";
    }
    delete[] arr;
    return EXIT_SUCCESS;
}
