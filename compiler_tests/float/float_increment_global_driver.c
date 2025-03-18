//
// Created by James Conway on 18/03/2025.
//

float float_inc();

double double_dec();

int main() {
    float f_delta = float_inc() - 13.0f;
    double d_delta = double_dec() - 5.0;
    if (f_delta > 0.001 || f_delta < -0.001) return 1;
    if (d_delta > 0.001 || d_delta < -0.001) return 2;
    return 0;
}
