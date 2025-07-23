#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include "CsvReader.h"
#include "tools/cpp/runfiles/runfiles.h"

using namespace std;

class ApogeeFinder {
public:
    struct Point {
        double time;
        double altitude;
    };

    ApogeeFinder(double windowDuration = 2.0)
        : windowDur(windowDuration), found(false), apogee({0, 0}) {}

    bool findApogee(const std::string& filename) {
        try {
            CsvReader<double, double> reader(filename);

            vector<Point> window;

            for (int i = 0; i < reader.rowCount(); ++i) {
                double time = reader[i].template get<0>();
                double pressure = reader[i].template get<1>();
                double altitude = pressureToAltitude(pressure);

                window.push_back({time, altitude});

                while (!window.empty() && (time - window.front().time) > windowDur)
                    window.erase(window.begin());

                if (window.size() < 3) continue;

                for (int j = 1; j < (int)window.size() - 1; j++) {
                    if (isLocalMax(window, j)) {
                        bool afterFalls = true;
                        for (int k = j + 1; k < (int)window.size() && k < j + 4; k++) {
                            if (window[k].altitude > window[j].altitude - 0.5) {
                                afterFalls = false;
                                break;
                            }
                        }
                        if (afterFalls && window[j].altitude > apogee.altitude) {
                            apogee = window[j];
                            found = true;
                        }
                    }
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Blad podczas wczytywania pliku " << e.what() << "\n";
            return false;
        }
        return found;
    }

    Point getApogee(){
        return apogee;
    }

private:
    double windowDur;
    bool found;
    Point apogee;

    double pressureToAltitude(double p) {
    const double p0 = 101325.0;    // ciśnienie na poziomie morza, Pa
    const double h0 = 0.0;         // wysokość odniesienia, m
    const double T = 288.15;       // temperatura, K
    const double R = 8.31447;      // stała gazowa, J/(mol·K)
    const double g = 9.80665;      // przyspieszenie grawitacyjne, m/s²
    const double M = 0.0289644;    // masa molowa powietrza, kg/mol
    
    return h0 - (R * T) / (g * M) * log(p / p0);
}



    bool isLocalMax(const vector<Point>& w, int mid) {
        double midAlt = w[mid].altitude;
        for (int i = 0; i < (int)w.size(); i++) {
            if (i != mid && w[i].altitude > midAlt)
                return false;
        }
        return true;
    }
};

int main(int argc, char** argv) {
    
    std::string error;
    auto runfiles = bazel::tools::cpp::runfiles::Runfiles::Create(argv[0], &error);
    std::string data_path = runfiles->Rlocation("_main/src/apogeum-barometer/data.csv");

    ApogeeFinder finder;
    if (finder.findApogee(data_path)) {
        auto apogee = finder.getApogee();
        std::cout << "Apogeum znalezione\nCzas: " << apogee.time << " s, Wysokość: " << apogee.altitude << " m\n";
    } else {
        std::cout << "Apogeum nie zostało znalezione\n";
    }
    return 0;
}
