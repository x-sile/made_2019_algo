/*
Дан неориентированный связный граф. Требуется найти вес минимального остовного дерева в этом графе.
Вариант 1. С помощью алгоритма Прима.
Вариант 2. С помощью алгоритма Крускала.
Вариант 3. С помощью алгоритма Борувки.
Ваш номер варианта прописан в ведомости.
Формат ввода
Первая строка содержит два натуральных числа n и m — количество вершин и ребер графа соответственно
(1 ≤ n ≤ 20000, 0 ≤ m ≤ 100000).
Следующие m строк содержат описание ребер по одному на строке.
Ребро номер i описывается тремя натуральными числами bi, ei и wi — номера концов ребра и его вес соответственно
(1 ≤ bi, ei ≤ n, 0 ≤ wi ≤ 100000).
 Выведите единственное целое число - вес минимального остовного дерева.
*/

#include <algorithm>
#include <iostream>
#include <vector>
#include <tuple>
#include <set>

using namespace std;


int find_mst_weight(const vector<vector<pair<int, int>>> &graph) {
    int BIG_CONST = 999999;

    vector<bool> added(graph.size(), false);
    vector<int> distance(graph.size(), BIG_CONST);
    set<pair<int, int>> visited;
    added[0] = true;
    distance[0] = 0;
    int n_added = 1;
    int from = 0;
    int to = 0;
    int weight = 0;
    int weights = 0;
    while (n_added < graph.size()) {
        for (auto edge: graph[from]) {
            tie(to, weight) = edge;
            if (added[to]) {
                continue;
            }
            int new_weight = min(distance[to], weight);
            if (new_weight < distance[to]) {
                visited.erase(make_pair(distance[to], to));
                distance[to] = new_weight;
                visited.insert(make_pair(distance[to], to));
            }
        }
        from = visited.begin()->second;
        added[from] = true;
        n_added++;
        weights += visited.begin()->first;
        visited.erase(visited.begin());
    }
    return weights;
}

int main() {
    int n_vertex, n_edges;

    cin >> n_vertex >> n_edges;

    vector<vector<pair<int, int>>> graph(n_vertex);
    int vertex1, vertex2, weight;
    for (size_t i = 0; i < n_edges; i++) {
        cin >> vertex1 >> vertex2 >> weight;
        vertex1--;
        vertex2--;
        graph[vertex1].push_back(make_pair(vertex2, weight));
        graph[vertex2].push_back(make_pair(vertex1, weight));
    }

    cout << find_mst_weight(graph);

    return EXIT_SUCCESS;
}
