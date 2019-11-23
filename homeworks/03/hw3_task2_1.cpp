/*
Солдаты. В одной военной части решили построить в одну шеренгу по росту.
 Т.к. часть была далеко не образцовая, то солдаты часто приходили не вовремя,
 а то их и вовсе приходилось выгонять из шеренги за плохо начищенные сапоги.
 Однако солдаты в процессе прихода и ухода должны были всегда быть выстроены по росту –
 сначала самые высокие, а в конце – самые низкие.
 За расстановку солдат отвечал прапорщик, который заметил интересную особенность – все солдаты в части разного роста.
 Ваша задача состоит в том, чтобы помочь прапорщику правильно расставлять солдат,
 а именно для каждого приходящего солдата указывать, перед каким солдатом в строе он должен становится.
Вариант 7_1.  Требуемая скорость выполнения команды - O(log n) в среднем. В реализации используйте декартово дерево.
*/

#include <iostream>
#include <functional>
#include <cassert>

using namespace std;

struct TreapNode {
    explicit TreapNode(int key_, int priority_) : key(key_), priority(priority_) {}

    int key;
    int priority;
    int size = 0; // Order statistic tree - храним размер поддерева
    TreapNode *left = nullptr;
    TreapNode *right = nullptr;
};

using Callback = std::function<void(TreapNode *node)>;

class Treap {
private:
    TreapNode *root = nullptr;
    void traverse(TreapNode *node, const Callback &callback);
    int search_index(TreapNode *node, int key) const;
    int search_key(TreapNode *node, int index) const;

public:
    void split(TreapNode *node, int key, TreapNode *&left, TreapNode *&right);
    TreapNode *merge(TreapNode *&left, TreapNode *&right);
    int add(int key);
    void remove(int index);
    ~Treap();
};

void Treap::split(TreapNode *node, int key, TreapNode *&left, TreapNode *&right) {
    if (!node) {
        left = right = nullptr;
    } else if (node->key <= key) {
        split(node->right, key, node->right, right);
        left = node;
        // обновляем размер поддерева
        left->size = (left->left ? left->left->size + 1 : 0) + (left->right ? left->right->size + 1 : 0);
    } else {
        split(node->left, key, left, node->left);
        right = node;
        right->size = (right->right ? right->right->size + 1 : 0) + (right->left ? right->left->size + 1 : 0);
    }
}

TreapNode *Treap::merge(TreapNode *&left, TreapNode *&right) {
    if (!left) return right;
    if (!right) return left;

    if (left->priority > right->priority) {
        left->right = merge(left->right, right);
        left->size = (left->left ? left->left->size + 1 : 0) + left->right->size + 1;
        return left;
    } else {
        right->left = merge(left, right->left);
        right->size = (right->right ? right->right->size + 1 : 0) + right->left->size + 1;
        return right;
    }
}

int Treap::search_index(TreapNode *node, int key) const {
    // ищем позицию в строю (индекс начиная с максимального элемента) по ключу
    assert(node);
    int cur_index = node->right ? node->right->size + 1 : 0;
    if (key == node->key){
        return cur_index;
    } else if (key < node->key) {
        assert(node->left);
        return cur_index + 1 + search_index(node->left, key);
    } else if (key > node->key) {
        assert(node->right);
        return search_index(node->right, key);
    } else {
        throw std::invalid_argument("No key found");
    }
}


int Treap::search_key(TreapNode *node, int index) const {
    // ищем ключ по позиции элемента (начиная с самого большого)
    assert(node);
    int cur_index = node->right ? node->right->size + 1 : 0;
    if (index == cur_index) {
        return node->key;
    } else if (index > cur_index) {
        assert(node->left);
        return search_key(node->left, index - cur_index - 1);
    } else if (index < cur_index) {
        assert(node->right);
        return search_key(node->right, index);
    } else {
        throw std::invalid_argument("No index found");
    }
}

int Treap::add(int key) {
    TreapNode *left = nullptr;
    TreapNode *right = nullptr;
    split(root, key, left, right);
    TreapNode *tmp = new TreapNode(key, rand());
    left = merge(left, tmp);
    root = merge(left, right);
    return search_index(root, key);
}

void Treap::remove(int index) {
    TreapNode *left = nullptr;
    TreapNode *right = nullptr;
    TreapNode *tmp = nullptr;
    int key = search_key(root, index);
    split(root, key - 1, left, right);
    split(right, key, tmp, right);
    delete tmp;
    root = merge(left, right);
}

void Treap::traverse(TreapNode *node, const Callback &callback) {
    if (!node) return;
    traverse(node->left, callback);
    traverse(node->right, callback);
    callback(node);
}

Treap::~Treap() {
    traverse(root, [](TreapNode *node) { delete node; });
}


int main() {
    srand(42);

    int n = 0;
    cin >> n;

    int command = 0;
    int command_value = 0;
    Treap treap;
    for (int i = 0; i < n; i++) {
        cin >> command >> command_value;
        if (command == 1) {
            cout << treap.add(command_value) << endl;
        } else if (command == 2) {
            treap.remove(command_value);
        }
    }
    return EXIT_SUCCESS;
}
