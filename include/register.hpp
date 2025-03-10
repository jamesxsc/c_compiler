#pragma once

#include <iostream>

namespace ast {

    // Note that alignment of the integral values with x... aliases is coincidental and NOT to be relied upon
    enum class Register {
        // Integer registers
        zero = 0,
        ra = 1,
        sp = 2,
        gp = 3,
        tp = 4,
        t0 = 5, t1 = 6, t2 = 7,
        s0 = 8, s1 = 9,
        a0 = 10, a1 = 11, a2 = 12, a3 = 13, a4 = 14, a5 = 15, a6 = 16, a7 = 17,
        s2 = 18, s3 = 19, s4 = 20, s5 = 21, s6 = 22, s7 = 23, s8 = 24, s9 = 25, s10 = 26, s11 = 27,
        t3 = 28, t4 = 29, t5 = 30, t6 = 31,
        // Floating point registers
        ft0 = 100, ft1 = 101, ft2 = 102, ft3 = 103, ft4 = 104, ft5 = 105, ft6 = 106, ft7 = 107,
        fs0 = 108, fs1 = 109,
        fa0 = 110, fa1 = 111, fa2 = 112, fa3 = 113, fa4 = 114, fa5 = 115, fa6 = 116, fa7 = 117,
        fs2 = 118, fs3 = 119, fs4 = 120, fs5 = 121, fs6 = 122, fs7 = 123, fs8 = 124, fs9 = 125, fs10 = 126, fs11 = 127,
        ft8 = 128, ft9 = 129, ft10 = 130, ft11 = 131

    };

    std::ostream &operator<<(std::ostream &stream, Register reg);

    // Integer registers
    Register IntegerTemporaryAtIndex(int index);

    int IndexOfIntegerTemporary(Register reg);

    Register IntegerPersistentAtIndex(int index);

    int IndexOfIntegerPersistent(Register reg);

    // Floating point registers
    Register FloatTemporaryAtIndex(int index);

    int IndexOfFloatTemporary(Register reg);

    Register FloatPersistentAtIndex(int index);

    int IndexOfFloatPersistent(Register reg);

    bool IsFloatRegister(Register reg);

}
