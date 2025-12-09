#include "Matrix.h"
#include <memory>
#include <string>
#include <stdexcept>

Matrix::Matrix(std::initializer_list<std::initializer_list<float>> list){
    rows = list.size();
    columns = list.begin()->size();

    array = std::make_unique<std::unique_ptr<float[]>[]>(rows);
    for (size_t i = 0; i < rows; ++i) {
        array[i] = std::make_unique<float[]>(columns);
    }

    size_t r = 0;
    for (const auto& row : list) {
        size_t c = 0;
        for (double val : row) {
            array[r][c] = val;
            ++c;
        }
        ++r;
    }
}


const float& Matrix::operator ()(size_t r, size_t c) const{
        return array[r][c];
}

size_t Matrix::getRows()
{
    return rows;
}

size_t Matrix::getColumns()
{
    return columns;
}

std::string Matrix::toString()
{
    std::string str = "";
    for(size_t r=0; r < rows; r++){
        str += "|";
        for(size_t c=0; c < columns; c++){
            str += std::to_string(array[r][c]);
            if(c!=columns-1) str += ", ";
        }
        str += "|\n";
    }


    return str;
}
