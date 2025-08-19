#include "KalmanFilterR7.h"

KalmanFilterR7::KalmanFilterR7(float dt):KalmanFilter(
    makeMatrix({{1, dt}, {0, 1}}),      // F - macierz przejścia stanu
    makeMatrix({{0},{dt}}),             // B - macierz sterowania
    makeMatrix({{1, 0}}),               // H - macierz obserwacji
    makeMatrix({{0.01, 0}, {0, 0.01}}), // Q - szum procesu
    makeMatrix({{19}}),                 // R - szum pomiaru
    Eigen::MatrixXd::Identity(2, 2),    // P - kowariancja stanu
    makeMatrix({{0},{0}})               // x - wektor stanu (pozycja, prędkość)
    ){}

Eigen::VectorXd KalmanFilterR7::processMeasurement(float acceleration, float pressure)
{
    return getState();
}

Eigen::MatrixXd KalmanFilterR7::makeMatrix(std::initializer_list<std::initializer_list<double>> list) {
    size_t rows = list.size();
    size_t cols = list.begin()->size();

    Eigen::MatrixXd m(rows, cols);

    size_t r = 0;
    for (const auto& row : list) {
        size_t c = 0;
        for (double val : row) {
            m(r, c) = val;
            ++c;
        }
        ++r;
    }
    return m;
}