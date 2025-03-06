//
// Created by James on 05/03/2025.
//

int clobber_multiplicative();

int clobber_divide();

int clobber_modulo();

int clobber_additive();

int clobber_subtractive();

int clobber_left_shift();

int clobber_right_shift();

int clobber_less_than();

int clobber_greater_than();

int clobber_equal();

int clobber_bitwise_and();

int clobber_bitwise_xor();

int clobber_bitwise_or();

int clobber_logical_and();

int clobber_logical_or();

int main() {

    if (clobber_multiplicative() != 100) return 1;
    if (clobber_divide() != 3) return 2;
    if (clobber_modulo() != 1) return 3;
    if (clobber_additive() != 20) return 4;
    if (clobber_subtractive() != 7) return 5;
    if (clobber_left_shift() != 80) return 6;
    if (clobber_right_shift() != 1) return 7;
    if (clobber_less_than() != 0) return 8;
    if (clobber_greater_than() != 1) return 9;
    if (clobber_equal() != 1) return 10;
    if (clobber_bitwise_and() != 10) return 11;
    if (clobber_bitwise_xor() != (3^10)) return 12;
    if (clobber_bitwise_or() != (3|10)) return 13;
    if (clobber_logical_and() != 1) return 14;
    if (clobber_logical_or() != 1) return 15;

    return 0;

}
