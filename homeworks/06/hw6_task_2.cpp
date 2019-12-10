/*
 Мосты.
Ребро неориентированного графа называется мостом, если удаление этого ребра из графа
увеличивает число компонент связности.
Дан неориентированный граф, требуется найти в нем все мосты.

 Ввод: Первая строка входного файла содержит два целых числа n и m — количество вершин и ребер графа
соответственно (1 ≤ n ≤ 20000, 1 ≤ m ≤ 200000).
Следующие m строк содержат описание ребер по одному на строке. Ребро номер i описывается двумя натуральными числами bi,
ei — номерами концов ребра (1 ≤ bi, ei ≤ n).
 Вывод: Первая строка выходного файла должна содержать одно натуральное число b — количество мостов в заданном графе.
На следующей строке выведите b целых чисел — номера ребер, которые являются мостами, в возрастающем порядке.
Ребра нумеруются с единицы в том порядке, в котором они заданы во входном файле.
*/

#include <fstream>
#include <set>
#include <stack>
#include <unordered_map>
#include <vector>


using namespace std;

void dfs(const vector<vector<int> > &graph, const unordered_map<string, pair<int, bool> > &edges, set<int> &bridges) {
    int INIT_VALUE = -1;
    stack<pair<int, int> > verts;
    vector<int> entry(graph.size(), INIT_VALUE);
    vector<int> lowest(graph.size(), INIT_VALUE);

    for (int vertex = 0; vertex < graph.size(); vertex++) { // если граф не связанный, нужно обойти все
        if (entry[vertex] != INIT_VALUE) {
            continue; // если уже посещали, то идем дальше
        }

        int time = 0;
        int current_vertex = vertex;
        int parent_vertex = current_vertex;
        verts.push(make_pair(current_vertex, parent_vertex));
        while (!verts.empty()) {
            tie(current_vertex, parent_vertex) = verts.top();
            if (entry[current_vertex] != INIT_VALUE) { // уже посещали
                if (lowest[current_vertex] < lowest[parent_vertex]) {
                    lowest[parent_vertex] = lowest[current_vertex];
                } else if (lowest[current_vertex] > entry[parent_vertex]) {
                    int index;
                    bool is_multiple;
                    auto edge = to_string(current_vertex) + "_" + to_string(parent_vertex);
                    tie(index, is_multiple) = edges.at(edge);
                    if (!is_multiple) { // если ребро парное, то точно не мост
                        bridges.insert(index);
                    }
                }
                verts.pop();
                continue;
            }
            entry[current_vertex] = time;
            lowest[current_vertex] = time;
            for (auto vert: graph[current_vertex]) {
                if (vert != parent_vertex) { // обратную вершину не пушим
                    verts.push(make_pair(vert, current_vertex));
                }
            }
            time++;
        }
    }
}


int main() {
    ifstream in("bridges.in");
    ofstream out("bridges.out");
    int n_vertex, n_edges;

    in >> n_vertex >> n_edges;

    unordered_map<string, pair<int, bool> > edges{}; // {edge: (index_in_source_file, is_multiple)}
    vector<vector<int> > graph(n_vertex);
    set<int> bridges;
    for (int i = 0; i < n_edges; i++) {
        int vertex1, vertex2;
        in >> vertex1 >> vertex2;
        vertex1--; // перейдем к отсчету с нуля
        vertex2--;
        graph[vertex1].push_back(vertex2);
        graph[vertex2].push_back(vertex1);
        auto edge = to_string(vertex1) + "_" + to_string(vertex2); // чтобы не писать хеш функцию для pair
        auto reverse_edge = to_string(vertex2) + "_" + to_string(vertex1);
        auto is_multiple = (edges.find(edge) != edges.end());
        edges[edge] = make_pair(i + 1, is_multiple);
        edges[reverse_edge] = make_pair(i + 1, is_multiple);
    }

    dfs(graph, edges, bridges);

    out << bridges.size() << endl;
    for (auto bridge: bridges) {
        out << bridge << " ";
    }

    in.close();
    out.close();

    return EXIT_SUCCESS;
}
