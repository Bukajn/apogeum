#pragma once
#include <initializer_list>
#include <memory>
#include <string>

class Matrix {
private:
    size_t rows = 0;
    size_t columns = 0;
    std::unique_ptr<std::unique_ptr<float[]>[]> array;
public:
    Matrix(std::initializer_list<std::initializer_list<float>> list);
    const float& operator ()(size_t r, size_t c) const;
    size_t getRows();
    size_t getColumns();
    std::string toString();
};