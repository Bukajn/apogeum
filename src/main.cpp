#include <iostream>
#include "CsvReader.h"
#include <Eigen/Dense>
#include "KalmanFilterR7.h"
#include <fstream>

int main() {
    CsvReader<float, float, float, float> barometrCsv("./out/barometer_clean.csv");
    CsvReader<float, float, float, float> accelerometerCsv("./out/accelerometer_noisy_nosecone.csv");
    std::ofstream file("output.txt");
    file << "t, h, v\n";

    KalmanFilterR7 f(0.01);
    for(int i =0; i < accelerometerCsv.rowCount(); i++) {
        auto barometrRow = barometrCsv[i/2];
        auto accelerometerRow = accelerometerCsv[i];
        auto r = f.processMeasurement(accelerometerRow.get<1>(), barometrRow.get<1>());
        file << accelerometerRow.get<0>() << ", " << r(0) << ", " << r(1) << "\n";
    }

   
    file.close();
}