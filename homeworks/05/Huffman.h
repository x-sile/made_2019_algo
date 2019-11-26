#include <iostream>
#include <fstream>
#include <vector>

using byte = unsigned char;

class IInputStream {
public:
    IInputStream(std::ifstream &stream_) : stream(stream_) {}

    bool Read(byte &value) {
        if (!stream.eof()) {
            stream.get(reinterpret_cast<char &>(value));
            // нужно проверить что мы читаем не ерунду, иначе запишем последний символ дважды
            return !stream.fail();
        }
        return false;
    }

    ~IInputStream() {
        stream.close();
    }

private:
    std::ifstream &stream;
};

class IOutputStream {
public:
    IOutputStream(std::ofstream &stream_) : stream(stream_) {}

    void Write(byte value) {
        stream << value;
    }

    ~IOutputStream() {
        stream.close();
    }

private:
    std::ofstream &stream;
};

// Метод архивирует данные из потока original
void Encode(IInputStream &original, IOutputStream &compressed);

// Метод восстанавливает оригинальные данные
void Decode(IInputStream &compressed, IOutputStream &original);
