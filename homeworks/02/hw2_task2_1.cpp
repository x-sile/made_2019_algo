/*
3_1. Первые k элементов длинной последовательности.
Дана очень длинная последовательность целых чисел длины n.
 Требуется вывести в отсортированном виде её первые k элементов.
 Последовательность может не помещаться в память. Время работы O(n * log(k)). Память O(k). Использовать слияние.
*/

#include <iostream>
#include <cstdlib>

using namespace std;


int get_median_of_three_index(const int *arr, int size) {
    if (size < 3) { // можно не добавлять, просто быстрее на тестах
        return 0;
    }
    // код нахождения медианы из трех
    if (arr[0] > arr[size / 2]) {
        if (arr[size / 2] > arr[size - 1]) {
            return size / 2;
        } else {
            return (arr[size - 1] < arr[0]) ? size - 1 : 0;
        }
    } else {
        if (arr[size / 2] < arr[size - 1]) {
            return size / 2;
        } else {
            return (arr[size - 1] > arr[0]) ? size - 1 : 0;
        }
    }
}

int partition(int *arr, int size) {
    // просто имлпементация алгоритма, описанного в з адании
    int i = 0;
    int j = 0;
    int pivot = get_median_of_three_index(arr, size);
    swap(arr[pivot], arr[size - 1]);
    while (j < size - 1) {
        if (arr[j] <= arr[size - 1]) {
            swap(arr[i], arr[j]);
            i++;
        }
        j++;
    }
    swap(arr[i], arr[size - 1]);
    return i;
}

int find_k_stat(int *arr, int size, int k) {
    // тут будем поддерживать левую и правую границу, за пределами которых точно не надо искать
    int left_ptr = 0; // индекс с которого начинать партицировать (включительно)
    int right_ptr = size; // индекс до которого партицировать (не включительно)
    int pivot_pos = partition(arr, size);
    int current_pos = pivot_pos;
    while (current_pos != k) {
        if (current_pos > k) { // если перебрали, то задаем новую правую границу
            right_ptr = left_ptr + pivot_pos;
        } else { // если не добрали, то левую границу
            left_ptr = left_ptr + pivot_pos + 1;
        }
        pivot_pos = partition(arr + left_ptr, right_ptr - left_ptr);
        current_pos = left_ptr + pivot_pos;
    }
    return arr[k];
}

int main() {
    int size = 0;
    int k = 0;
    cin >> size >> k;
    int *arr = new int[size];
    for (int i = 0; i < size; i++) {
        cin >> arr[i];
    }
    int k_stat = find_k_stat(arr, size, k);
    cout << k_stat;
    delete[] arr;
    return EXIT_SUCCESS;
}
