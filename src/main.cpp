#include <iostream>
#include "CsvReader.h"
#include <Eigen/Dense>
#include "KalmanFilterR7.h"

int main() {
    CsvReader<float, float, float, float> barometrCsv("./out/barometer_clean.csv");
    CsvReader<float, float, float, float> accelerometerCsv("./out/accelerometer_noisy_nosecone.csv");

    std::cout << "Liczba wierszy: " << barometrCsv.rowCount() << std::endl;
    std::cout << "Liczba wierszy: " << accelerometerCsv.rowCount() << std::endl;
    KalmanFilterR7 f(0.01);
    for(int i =0; i < accelerometerCsv.rowCount(); i++) {
        auto barometrRow = barometrCsv[i/2];
        auto accelerometerRow = accelerometerCsv[i];
        f.processMeasurement(accelerometerRow.get<1>(), barometrRow.get<1>());
        std::cout << accelerometerRow.get<0>() << ",  " << accelerometerRow.get<1>() << ", " << barometrRow.get<1>() << ", " << f.getState()(0,0) << std::endl;
    }

   
    
}