/*
Дано множество точек на плоскости (x, y). Постройте выпуклую оболочку этого множества и вычислите ее периметр.
Вариант 2. С помощью алгоритма Джарвиса.
Обратите внимание: три и более точки из множества могут лежать на одной прямой.
Формат входного файла
        Количество точек, далее на каждой строке координаты точек x, y.
Общее количество точек не превосходит 100000.
Координаты точек - числа с плавающей точкой в диапазоне [-1000, 1000].
Формат выходного файла
Выведите периметр выпуклой оболочки.
*/

#include <algorithm>
#include <iostream>
#include <vector>
#include <cmath>

struct Vector {
    Vector() = default;;

    Vector(double x_, double y_) : x(x_), y(y_) {};

    double operator*(const Vector &other) const {
        return (x * other.y) - (y * other.x);
    }

    double length() const {
        return sqrt(pow(x, 2) + pow(y, 2));
    }

    double x;
    double y;
};

struct Point {
    Point(double x_, double y_) : x(x_), y(y_) {};

    bool operator==(const Point &other) const {
        return (x == other.x) && (y == other.y);
    }

    bool operator!=(const Point &other) const {
        return !(*this == other);
    }

    Vector operator-(const Point &other) const {
        return {x - other.x, y - other.y};
    }

    double x;
    double y;
    bool added = false;
};


struct Comparator {
    bool operator()(const Point &left, const Point &right) const {
        return (left.y < right.y) || ((left.y == right.y) && (left.x < right.x));
    }
};

double find_perimeter(std::vector<Point> &points) {
    if (points.size() == 1) return 0.0;

    double perimeter = 0.0;
    auto first_point = *std::min_element(points.begin(), points.end(), Comparator());
    auto current_point = first_point;

    while ((current_point != first_point) || (perimeter == 0.0)) {
        size_t next_point_index = -1;
        Vector next_vector{};
        for (size_t i = 0; i < points.size(); i++) {
            if (points[i].added || (points[i] == current_point)) continue;
            if (next_point_index == -1) {
                next_point_index = i;
                next_vector = points[next_point_index] - current_point;
                continue;
            }
            Vector candidate_vector = points[i] - current_point;
            double vector_product = candidate_vector * next_vector;
            if (vector_product >= 0) {
                if (vector_product == 0) {
                    if (candidate_vector.length() > next_vector.length()) {
                        points[next_point_index].added = true;
                        next_point_index = i;
                        next_vector = candidate_vector;
                    } else {
                        points[i].added = true;
                    }
                } else {
                    next_point_index = i;
                    next_vector = candidate_vector;
                }
            }
        }
        current_point = points[next_point_index];
        perimeter += next_vector.length();
    }

    return perimeter;
}

int main() {
    int n_points = 0;
    std::cin >> n_points;

    std::vector<Point> points;
    double x = 0;
    double y = 0;
    for (size_t i = 0; i < n_points; i++) {
        std::cin >> x >> y;
        points.emplace_back(Point(x, y));
    }

    std::cout.precision(10);
    std::cout << find_perimeter(points);

    return EXIT_SUCCESS;
}
