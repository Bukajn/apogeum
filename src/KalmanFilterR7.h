#include <Eigen/Dense>
#include "KalmanFilter.h"

class KalmanFilterR7 : public KalmanFilter {

public:
    KalmanFilterR7(float dt);  
    Eigen::VectorXd processMeasurement(float acceleration, float pressure);
    
private:
    Eigen::MatrixXd makeMatrix(std::initializer_list<std::initializer_list<double>> list);
    using KalmanFilter::predict; 
    using KalmanFilter::update; 
};
