#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include "CsvReader.h"

class RealTimeApogee {
public:
    RealTimeApogee(size_t bufferSize = 5, double speedThreshold = 0.05)
        : bufferSize(bufferSize), speedThreshold(speedThreshold),
          maxHeight(-1e9), detectedApogee(0), apogeeReached(false) {}

    void update(double height, double velocity) {
        heightBuffer.push_back(height);
        speedBuffer.push_back(velocity);
        if (heightBuffer.size() > bufferSize) {
            heightBuffer.erase(heightBuffer.begin());
            speedBuffer.erase(speedBuffer.begin());
        }

        if (!apogeeReached && speedBuffer[speedBuffer.size() - 1] <= speedThreshold) {
            apogeeReached = true;
            detectedApogee = *std::max_element(heightBuffer.begin(), heightBuffer.end());
        }

        if (!apogeeReached && height > maxHeight) {
            maxHeight = height;
        }
    }

    bool isApogeeReached() const { return apogeeReached; }
    double getApogee() const { return detectedApogee; }

private:
    size_t bufferSize;
    double speedThreshold;
    std::vector<double> heightBuffer;
    std::vector<double> speedBuffer;
    double maxHeight;
    double detectedApogee;
    bool apogeeReached;
};