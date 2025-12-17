#include "Matrix.h"

class KalmanFilter {
public:


    KalmanFilter(const Matrix& F, const Matrix& B, const Matrix& H,
                 const Matrix& Q, const Matrix& R, const Matrix& P,
                 const Matrix& x);


    void predict(const Matrix& u);

    void update(const Matrix& z);
    Matrix getState() const;

protected:
    Matrix F;  // macierz przejścia stanu
    Matrix B;  // macierz sterowania
    Matrix H;  // macierz obserwacji
    Matrix Q;  // szum procesu
    Matrix R;  // szum pomiaru
    Matrix P;  // kowariancja stanu
    Matrix x;  // wektor stanu
};
