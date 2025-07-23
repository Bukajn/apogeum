#include <iostream>
#include <fstream>
#include <random>

int main() {
    std::ofstream out("pomiary.txt");
    std::default_random_engine gen;
    std::normal_distribution<double> noise(0.0, 0.8); // szum ±0.8 hPa
    double base = 1013.0;

    for (int i = 0; i < 200; ++i) {
        double real = base + std::sin(i / 20.0) * 1.5; // powolna zmiana ciśnienia
        double noisy = real + noise(gen); // dodaj szum
        out << noisy << "\n";
    }

    return 0;
}
