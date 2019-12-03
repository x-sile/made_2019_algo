#include "Huffman.h"

#include <queue>
#include <stack>
#include <unordered_map>
#include <vector>

using namespace std;

struct Node {
    byte code; // символ
    long count; // количество раз, которое символ встретился в файле
    Node *left;
    Node *right;
    byte leaf_size = 0; // размер словаря - будем считать, что если 0, то это 256!

    Node(byte code_ = 0, long count_ = 0, byte leaf_size_ = 0, Node *left_ = nullptr, Node *right_ = nullptr)
            : code(code_), count(count_), leaf_size(leaf_size_), left(left_), right(right_) {}
};

void delete_tree(Node *tree) {
    if (!tree) return;
    delete_tree(tree->left);
    delete_tree(tree->right);
    delete tree;
}

// компаратор для конструирования min-heap
struct Comparator {
    bool operator()(const Node *left, const Node *right) const {
        return left->count > right->count;
    }
};

// класс для записи байтов/битов (немного отличается от семинарского)
class BitsWriter {
public:
    void write_bit(bool bit);

    void write_byte(byte value);

    vector<byte> get_result();

private:
    vector<byte> buffer;
    byte accumulator = 0;
    int bits_count = 0; // - при чтении последнего бита будем считать, что если 0, то это 8!
};

void BitsWriter::write_bit(bool bit) {
    // Ставим бит в аккумулятор на нужное место, пишем слева направо (тут отличие от семинара!)
    accumulator |= static_cast<byte>(bit) << (7 - bits_count);
    bits_count++;
    if (bits_count == 8) {
        buffer.push_back(accumulator);
        bits_count = 0;
        accumulator = 0;
    }
}

void BitsWriter::write_byte(byte value) {
    if (bits_count == 0) {
        buffer.push_back(value);
    } else {
        accumulator |= value >> bits_count;
        buffer.push_back(accumulator);
        accumulator = value << (8 - bits_count);
    }
}

vector<byte> BitsWriter::get_result() {
    if (bits_count != 0) {
        // Добавляем в буфер аккумулятор, если в нем что-то есть
        buffer.push_back(accumulator);
    }
    buffer.push_back(static_cast<byte>(bits_count));
    return buffer;
}

class BitsReader {
public:
    BitsReader(const vector<byte> &buffer_) : buffer(buffer_) {}

    bool read_bit();

    byte read_byte();

    long bits_processed() const; // сколько бит мы уже прочитали

private:
    const vector<byte> &buffer;
    int bytes_count = 0; // сколько байт мы прочитали
    int bits_count = 0; // сколько бит текущего байта мы прочитали
};

bool BitsReader::read_bit() {
    bool bit = (buffer[bytes_count] >> (7 - bits_count)) & 1;
    bits_count++;
    if (bits_count == 8) {
        bits_count = 0;
        bytes_count++;
    }
    return bit;
}

byte BitsReader::read_byte() {
    if (bits_count == 0) {
        return buffer[bytes_count++];
    } else {
        byte accumulator = 0;
        for (size_t i = 0; i < 8; i++) {
            auto bit = read_bit();
            accumulator |= static_cast<byte>(bit) << (7 - i);
        }
        return accumulator;
    }
}

long BitsReader::bits_processed() const {
    return bytes_count * 8 + bits_count;
}

pair<Node *, vector<byte> > create_tree_from_source(IInputStream &original) {
    // читаем байты и конструируем частотности символов + сразу пишем в вектор, чтобы не читать два раза из файла
    unordered_map<byte, long> counter{};
    vector<byte> file;
    byte value = 0;
    while (original.Read(value)) {
        file.push_back(value);
        counter[value]++;
    }

    // перекладываем все в min-heap
    priority_queue<Node *, vector<Node *>, Comparator> heap;
    for (auto &code: counter) {
        auto node = new Node(code.first, code.second, 1);
        heap.push(node);
    }

    // обходим кучу и строим деревце
    // рассмотрим краевой случай для удобства
    if (heap.size() == 1) {
        Node *node1 = heap.top();
        heap.pop();
        auto node = new Node(0, node1->count, node1->leaf_size, node1, nullptr);
        heap.push(node);
    } else {
        while (heap.size() != 1) {
            Node *node1 = heap.top();
            heap.pop();
            Node *node2 = heap.top();
            heap.pop();
            auto node = new Node(0, node1->count + node2->count,
                                 node1->leaf_size + node2->leaf_size, node1, node2);
            heap.push(node);
        }
    }

    return make_pair(heap.top(), file);
}

void compress_tree(Node *tree, vector<bool> &route, unordered_map<byte, vector<bool> > &table,
                   vector<byte> &compressed_tree) {
    // обходим дерево и одновременно строим таблицу соответствия символов и их закодированных последовательностей
    // и сжимаем дерево
    // дерево сжимается так: обходим - для каждой внутренней ноды пишем 0, для листа 1 + сам символ
    if (!tree->left && !tree->right) {
        table[tree->code] = route;
        compressed_tree.push_back('1');
        compressed_tree.push_back(tree->code);
    } else {
        compressed_tree.push_back('0');
    } // тут мы записали лишний бит для корня, не будем его писать потом
    if (tree->left) {
        route.push_back(false);
        compress_tree(tree->left, route, table, compressed_tree);
    }
    if (tree->right) {
        route.push_back(true);
        compress_tree(tree->right, route, table, compressed_tree);
    }
    route.pop_back();
}

pair<unordered_map<byte, vector<bool> >, vector<byte> > build_table(Node *tree) {
    // строим таблицу соответствия символов и их закодированных последовательностей
    vector<bool> route;
    vector<byte> compressed_tree;
    unordered_map<byte, vector<bool> > table;
    compress_tree(tree, route, table, compressed_tree);
    return make_pair(table, compressed_tree);
}

void compress(const vector<byte> &input, const vector<byte> &compressed_tree, byte leaf_size,
              const unordered_map<byte, vector<bool> > &table, IOutputStream &output) {
    // пишем все в файл
    BitsWriter bits_writer;

    bits_writer.write_byte(leaf_size); // сначала пишем размер словаря, а затем дерево
    for (size_t i = 1;
         i < compressed_tree.size(); i++) { // не пишем первый бит, тк в дереве записали лишний для корня
        if (compressed_tree[i] == '0') {
            bits_writer.write_bit(false);
        } else if (compressed_tree[i] == '1') {
            bits_writer.write_bit(true);
            i++;
            bits_writer.write_byte(compressed_tree[i]);
        }
    }

    // пишем сжатый файл + на конце количество значащих бит в последнем байте
    vector<bool> bits;
    for (auto value: input) {
        bits = table.at(value);
        for (auto bit: bits) {
            bits_writer.write_bit(bit);
        }
    }
    vector<byte> compressed = bits_writer.get_result();

    // перекладываем все в файл
    for (auto &code: compressed) {
        output.Write(code);
    }
}

vector<byte> read_compressed(IInputStream &compressed) {
    // считываем сжатый файл
    vector<byte> file;
    byte value = 0;
    while (compressed.Read(value)) {
        file.push_back(value);
    }
    return file;
}

vector<byte> decompress(const vector<byte> &file) {
    // декодируем файл
    BitsReader bits_reader(file);
    int leaf_size = bits_reader.read_byte();
    leaf_size = (leaf_size == 0) ? 256 : leaf_size; // если ноль, то у нас фулхаус (все возможные байты)

    // строим дерево
    int symbols_decoded = 0;
    byte code = 0;
    auto tree = new Node();
    auto node = tree;
    stack<Node *> nodes;
    nodes.push(node);
    while (symbols_decoded < leaf_size) {
        auto bit = bits_reader.read_bit();
        Node *new_node;
        if (bit) {
            code = bits_reader.read_byte();
            symbols_decoded++;
            new_node = new Node(code);
        } else {
            new_node = new Node(0);
            nodes.push(new_node);
        }
        if (!node->left) {
            node->left = new_node;
        } else {
            node->right = new_node;
        }
        if (!bit) {
            node = new_node;
        }
        while ((symbols_decoded < leaf_size) && (node->left && node->right)) {
            node = nodes.top();
            nodes.pop();
        }
    }

    // с помощью дерева декодируем файл
    vector<byte> decoded;
    node = tree;
    int last_bits = (file[file.size() - 1] == 0) ? 8 : file[file.size() - 1]; // если ноль, но все биты значимые
    auto bits_to_read = (file.size() - 2) * 8 + last_bits;
    while (bits_reader.bits_processed() < bits_to_read) {
        auto bit = bits_reader.read_bit();
        node = bit ? node->right : node->left;
        if (!node->left && !node->right) {
            decoded.push_back(node->code);
            node = tree;
        }
    }

    delete_tree(tree);

    return decoded;
}

void Encode(IInputStream &original, IOutputStream &compressed) {
    vector<byte> file;
    Node *tree;
    tie(tree, file) = create_tree_from_source(original);
    vector<byte> compressed_tree;
    unordered_map<byte, vector<bool> > table{};
    tie(table, compressed_tree) = build_table(tree);
    compress(file, compressed_tree, tree->leaf_size, table, compressed);
    delete_tree(tree);
}

void Decode(IInputStream &compressed, IOutputStream &original) {
    auto file = read_compressed(compressed);
    auto decoded = decompress(file);
    for (auto value: decoded) {
        original.Write(value);
    }
}

/*
int main() {
    ifstream in_("tests/input.txt");
    ofstream compr_out("tests/compressed.txt");
    IInputStream input(in_);
    IOutputStream compr_output(compr_out);

    Encode(input, compr_output);

    in_.close();
    compr_out.close();

    ifstream compr_in("tests/compressed.txt");
    ofstream out_("tests/output.txt");
    IInputStream compr_input(compr_in);
    IOutputStream output(out_);

    Decode(compr_input, output);

    compr_in.close();
    out_.close();

    return EXIT_SUCCESS;
}
*/
