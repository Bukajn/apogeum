#include "KalmanFilterR7.h"
#include <cmath>

KalmanFilterR7::KalmanFilterR7(float dt):KalmanFilter(
    {{1, dt}, {0, 1}},      // F - macierz przejścia stanu
    {{0},{dt}},             // B - macierz sterowania
    {{1, 0}},               // H - macierz obserwacji
    {{0.01, 0}, {0, 0.01}}, // Q - szum procesu
    {{19}},                 // R - szum pomiaru
    Matrix::Identity(2),    // P - kowariancja stanu
    {{0},{0}}               // x - wektor stanu (pozycja, prędkość)
    ){}

Matrix KalmanFilterR7::processMeasurement(float acceleration, float pressure)
{
    Matrix u(1,1);
    u(0,0) = acceleration;
     
    predict(u);

    Matrix z(1,1);
    z(0,0) = pressureToAltitude(pressure);
    update(z);
    return getState();
}


double KalmanFilterR7::pressureToAltitude(double p) {
    const double p0 = 101325.0,  // standardowe ciśnienie na poziomie morza (Pa)
                    T = 288.15,     // standardowa temperatura na poziomie morza (Kelwiny)
                    L = 0.0065,     // spadek temp. na 1 metr, K/m
                    R = 8.31447,    // uniwersalna stała gazowa, J/(mol·K)
                    g = 9.80665,    // przyspieszenie ziemskie, m/s^2
                    M = 0.0289644;  // molowa masa powietrza, kg/mol

    // h = (T / L) * (1 - (p / p0) ^ ((R * L) / (g * M)))
    return (T / L) * (1 - pow(p / p0, (R * L) / (g * M)));
}