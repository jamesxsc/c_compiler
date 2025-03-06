/* Created by James Conway on 05/03/2025. */

int use_temporaries() { /* returns 10 */
    int a = 1;
    int b = 2;
    int c = 7;
    return a + b + c;
}

int use_temporaries2() { /* returns 3 */
    int a = 1;
    int b = 1;
    int c = 1;
    return a + b + c;
}

int clobber_multiplicative() {
    return use_temporaries() * use_temporaries();
}

int clobber_divide() {
    return use_temporaries() / use_temporaries2();
}

int clobber_modulo() {
    return use_temporaries() % use_temporaries2();
}

int clobber_additive() {
    return use_temporaries() + use_temporaries();
}

int clobber_subtractive() {
    return use_temporaries() - use_temporaries2();
}

int clobber_left_shift() {
    return use_temporaries() << use_temporaries2();
}

int clobber_right_shift() {
    return use_temporaries() >> use_temporaries2();
}

int clobber_less_than() {
    return use_temporaries() < use_temporaries2();
}

int clobber_greater_than() {
    return use_temporaries() > use_temporaries2();
}

int clobber_equal() {
    return use_temporaries() == use_temporaries();
}

int clobber_bitwise_and() {
    return use_temporaries() & use_temporaries();
}

int clobber_bitwise_xor() {
    return use_temporaries() ^ use_temporaries2();
}

int clobber_bitwise_or() {
    return use_temporaries() | use_temporaries2();
}

int clobber_logical_and() {
    return use_temporaries() && use_temporaries2();
}

int clobber_logical_or() {
    return use_temporaries() || use_temporaries2();
}
