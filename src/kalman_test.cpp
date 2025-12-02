#include <iostream>
#include "CsvReader.h"
#include <Eigen/Dense>
#include "KalmanFilterR7.h"
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

int main() {
    try {
        CsvReader<float, float, float, float> barometrCsv("data/barometer_clean.csv");
        CsvReader<float, float, float, float> accelerometerCsv("data/accelerometer_noisy_nosecone.csv");

        std::string outputPath = "output.txt";
        std::ofstream file(outputPath);
        file << "t, h, v\n";

        KalmanFilterR7 f(0.01);
        for(int i =0; i < accelerometerCsv.rowCount(); i++) {
            auto barometrRow = barometrCsv[i/2]; 
            auto accelerometerRow = accelerometerCsv[i]; 
            auto r = f.processMeasurement(accelerometerRow.get<1>(), barometrRow.get<1>()); 
            file << accelerometerRow.get<0>() << ", " << r(0) << ", " << r(1) << "\n"; 
        }
       
        file.close();

        std::cout << "Output successfully written to: " << fs::absolute(outputPath) << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}