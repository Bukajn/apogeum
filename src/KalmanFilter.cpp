#include "KalmanFilter.h"
#include <stdexcept>


KalmanFilter::KalmanFilter(const Matrix &F, const Matrix &B, const Matrix &H, const Matrix &Q, const Matrix &R, const Matrix &P, const Matrix &x)
: F(F), B(B), H(H), Q(Q), R(R), P(P), x(x) {
    if(R.getColumns()!=1 || R.getRows()!=1){
        /*Filtr kalmana nie wymaga by macierz R była 1x1, lecz teraźniejsza implemetancja działa tylko dla 1x1 przez brak metody do odwracania macierzy w klasie Matrix. 
        Jakby była potrzeba rozszerzyć macierze to należy napisać metodę oraz poprawić obliczanie macierzy K w metodzie KalmanFilter::update*/
        throw std::invalid_argument("Matrix R must be 1x1. More info in the comment");
    }
}


void KalmanFilter::predict(const Matrix &u)
{
    x = F * x + B * u;
    P = F * P * F.transpose() + Q;
}

void KalmanFilter::update(const Matrix &z)
{
    Matrix y = z - H * x;
    Matrix S = H * P * H.transpose() + R;
    Matrix K = P * H.transpose() * S.inverse1x1();
    x = x + K * y;
    Matrix I = Matrix::Identity(P.getRows());
    P = (I - K * H) * P;
         

}

Matrix KalmanFilter::getState() const
{
    return x;
}


