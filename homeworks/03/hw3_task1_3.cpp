/*
Дано число N < 106 и последовательность целых чисел из [-231..231] длиной N.
Требуется построить бинарное дерево, заданное наивным порядком вставки.
Т.е., при добавлении очередного числа K в дерево с корнем root, если root→Key ≤ K,
то узел K добавляется в правое поддерево root; иначе в левое поддерево root.
Рекурсия запрещена.
6_3. Выведите элементы в порядке post-order (снизу вверх).
*/

#include <iostream>
#include <functional>
#include <stack>

using namespace std;

struct TreeNode {
    explicit TreeNode(int _key) : key(_key) {}

    int key;
    TreeNode *left = nullptr;
    TreeNode *right = nullptr;
};

using Callback = std::function<void(TreeNode *node)>;

class Tree {
public:
    void add(int key);
    void print();
    ~Tree();

private:
    TreeNode *root = nullptr;
    void traverse(TreeNode *node, const Callback &callback);
};

void Tree::add(int key) {
    if (!root) {
        root = new TreeNode(key);
        return;
    }
    TreeNode *node = root;
    TreeNode *parent = root;
    while (node) {
        parent = node;
        node = key < node->key ? node->left : node->right;
    }
    node = new TreeNode(key);
    if (key < parent->key) {
        parent->left = node;
    } else {
        parent->right = node;
    }
}

void Tree::traverse(TreeNode *node, const Callback &callback) {
    // будем использовать 2 стека, чтобы обойти в нужном порядке
    // в одном накапливаем вывод, в другой кладем кандидатов на обход
    stack<TreeNode *> first_stack;
    stack<TreeNode *> second_stack;

    second_stack.push(node);

    while (!second_stack.empty()) {
        node = second_stack.top();
        second_stack.pop();
        first_stack.push(node);
        if (node->left) {
            second_stack.push(node->left);
        }
        if (node->right) {
            second_stack.push(node->right);
        }
    }

    while (!first_stack.empty()) {
        callback(first_stack.top());
        first_stack.pop();
    }
}

void Tree::print() {
    traverse(root, [](TreeNode *node) { cout << node->key << " "; });
}

Tree::~Tree() {
    if (!root) return;
    traverse(root, [](TreeNode *node) { delete node; });
}

int main() {
    int n = 0;
    cin >> n;

    Tree tree;
    for (int i = 0; i < n; i++) {
        int key;
        cin >> key;
        tree.add(key);
    }
    tree.print();
    return EXIT_SUCCESS;
}
