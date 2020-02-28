/*
11_1. Цикл минимальной длины
Дан невзвешенный неориентированный граф. Найдите цикл минимальной длины.
Ввод: v: кол-во вершин (макс. 50000), n: кол-во ребер (макс. 200000), n пар реберных вершин.
Вывод: одно целое число равное длине минимального цикла. Если цикла нет, то вывести -1.
*/

#include <iostream>
#include <vector>
#include <queue>

using namespace std;

int bfs(const vector<vector<int> > &graph, int vertex) {
    int BIG_CONST = 999999;
    int min_cycle = BIG_CONST;
    vector<int> distance(graph.size(), BIG_CONST);
    vector<int> parent(graph.size(), -1);
    queue<int> edges;

    edges.push(vertex);
    parent[vertex] = vertex;
    distance[vertex] = 0;
    while (!edges.empty()) {
        int cur_vertex = edges.front();
        edges.pop();
        for (auto adj: graph[cur_vertex]) {
            if (distance[adj] == BIG_CONST) {
                edges.push(adj);
                parent[adj] = cur_vertex;
                distance[adj] = distance[cur_vertex] + 1;
            } else if (adj != parent[cur_vertex]) {
                min_cycle = min(min_cycle, distance[adj] + distance[cur_vertex] + 1);
            }
        }
    }
    return min_cycle;
}


int main() {
    int n_vertex;
    int n_edges;

    cin >> n_vertex >> n_edges;

    vector<vector<int> > graph(n_vertex);
    int vertex1;
    int vertex2;
    for (size_t i = 0; i < n_edges; i++) {
        cin >> vertex1 >> vertex2;
        graph[vertex1].push_back(vertex2);
        graph[vertex2].push_back(vertex1);
    }

    int min_cycle = 999999;

    for (size_t vertex = 0; vertex < n_vertex; vertex++) {
        int cycle_len = bfs(graph, vertex);
        if (cycle_len < min_cycle) {
            min_cycle = cycle_len;
        }
    }

    if (min_cycle == 999999) {
        cout << "-1";
    } else {
        cout << min_cycle;
    }

    return EXIT_SUCCESS;
}
