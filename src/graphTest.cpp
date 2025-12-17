#include <iostream>
#include "CsvReader.h"
#include "KalmanFilterR7.h"
#include <fstream>



extern "C" void filter(const char* output, const char* barometr, const char* accelerometr){
    CsvReader<float, float, float, float> barometrCsv(barometr);
    CsvReader<float, float, float, float> accelerometerCsv(accelerometr);
    std::ofstream file(output);
    file << "t,z,v\n";

    KalmanFilterR7 f(0.01);
    for(int i =0; i < accelerometerCsv.rowCount(); i++) {
        auto barometrRow = barometrCsv[i/2]; 
        auto accelerometerRow = accelerometerCsv[i]; 

        //Plik akcelometru musi być w formacie t,ax,ay,az
        auto r = f.processMeasurement(accelerometerRow.get<3>(), barometrRow.get<1>()); 
        file << accelerometerRow.get<0>() << ", " << r(0,0) << ", " << r(1,0) << "\n"; 
    }

   
    file.close();
}