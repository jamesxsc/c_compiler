#include "register.hpp"

#define REGISTER_CASE(reg) case Register::reg: stream << #reg; break;

#define INDEX_REGISTER_CASE(reg, index) case Register::reg: return index;

#define REGISTER_INDEX_CASE(index, reg) case index: return Register::reg;

namespace ast {

    std::ostream &operator<<(std::ostream &stream, Register reg) {
        switch (reg) {
            REGISTER_CASE(zero)
            REGISTER_CASE(ra)
            REGISTER_CASE(sp)
            REGISTER_CASE(gp)
            REGISTER_CASE(tp)
            REGISTER_CASE(t0)
            REGISTER_CASE(t1)
            REGISTER_CASE(t2)
            REGISTER_CASE(s0)
            REGISTER_CASE(s1)
            REGISTER_CASE(a0)
            REGISTER_CASE(a1)
            REGISTER_CASE(a2)
            REGISTER_CASE(a3)
            REGISTER_CASE(a4)
            REGISTER_CASE(a5)
            REGISTER_CASE(a6)
            REGISTER_CASE(a7)
            REGISTER_CASE(s2)
            REGISTER_CASE(s3)
            REGISTER_CASE(s4)
            REGISTER_CASE(s5)
            REGISTER_CASE(s6)
            REGISTER_CASE(s7)
            REGISTER_CASE(s8)
            REGISTER_CASE(s9)
            REGISTER_CASE(s10)
            REGISTER_CASE(s11)
            REGISTER_CASE(t3)
            REGISTER_CASE(t4)
            REGISTER_CASE(t5)
            REGISTER_CASE(t6)
            REGISTER_CASE(ft0)
            REGISTER_CASE(ft1)
            REGISTER_CASE(ft2)
            REGISTER_CASE(ft3)
            REGISTER_CASE(ft4)
            REGISTER_CASE(ft5)
            REGISTER_CASE(ft6)
            REGISTER_CASE(ft7)
            REGISTER_CASE(fs0)
            REGISTER_CASE(fs1)
            REGISTER_CASE(fa0)
            REGISTER_CASE(fa1)
            REGISTER_CASE(fa2)
            REGISTER_CASE(fa3)
            REGISTER_CASE(fa4)
            REGISTER_CASE(fa5)
            REGISTER_CASE(fa6)
            REGISTER_CASE(fa7)
            REGISTER_CASE(fs2)
            REGISTER_CASE(fs3)
            REGISTER_CASE(fs4)
            REGISTER_CASE(fs5)
            REGISTER_CASE(fs6)
            REGISTER_CASE(fs7)
            REGISTER_CASE(fs8)
            REGISTER_CASE(fs9)
            REGISTER_CASE(fs10)
            REGISTER_CASE(fs11)
            REGISTER_CASE(ft8)
            REGISTER_CASE(ft9)
            REGISTER_CASE(ft10)
            REGISTER_CASE(ft11)
        }
        return stream;
    }

    int IndexOfIntegerTemporary(Register reg) {
        switch (reg) {
            INDEX_REGISTER_CASE(t0, 0)
            INDEX_REGISTER_CASE(t1, 1)
            INDEX_REGISTER_CASE(t2, 2)
            INDEX_REGISTER_CASE(t3, 3)
            INDEX_REGISTER_CASE(t4, 4)
            INDEX_REGISTER_CASE(t5, 5)
            INDEX_REGISTER_CASE(t6, 6)
            default: return -1;
        }
    }

    int IndexOfFloatTemporary(Register reg) {
        switch (reg) {
            INDEX_REGISTER_CASE(ft0, 0)
            INDEX_REGISTER_CASE(ft1, 1)
            INDEX_REGISTER_CASE(ft2, 2)
            INDEX_REGISTER_CASE(ft3, 3)
            INDEX_REGISTER_CASE(ft4, 4)
            INDEX_REGISTER_CASE(ft5, 5)
            INDEX_REGISTER_CASE(ft6, 6)
            INDEX_REGISTER_CASE(ft7, 7)
            default: return -1;
        }
    }

    Register IntegerTemporaryAtIndex(int index) {
        switch (index) {
            REGISTER_INDEX_CASE(0, t0)
            REGISTER_INDEX_CASE(1, t1)
            REGISTER_INDEX_CASE(2, t2)
            REGISTER_INDEX_CASE(3, t3)
            REGISTER_INDEX_CASE(4, t4)
            REGISTER_INDEX_CASE(5, t5)
            REGISTER_INDEX_CASE(6, t6)
            default: return Register::zero;
        }
    }

    Register FloatTemporaryAtIndex(int index) {
        switch (index) {
            REGISTER_INDEX_CASE(0, ft0)
            REGISTER_INDEX_CASE(1, ft1)
            REGISTER_INDEX_CASE(2, ft2)
            REGISTER_INDEX_CASE(3, ft3)
            REGISTER_INDEX_CASE(4, ft4)
            REGISTER_INDEX_CASE(5, ft5)
            REGISTER_INDEX_CASE(6, ft6)
            REGISTER_INDEX_CASE(7, ft7)
            default: return Register::zero;
        }
    }

    Register IntegerPersistentAtIndex(int index) {
        switch (index) {
            REGISTER_INDEX_CASE(0, s0)
            REGISTER_INDEX_CASE(1, s1)
            REGISTER_INDEX_CASE(2, s2)
            REGISTER_INDEX_CASE(3, s3)
            REGISTER_INDEX_CASE(4, s4)
            REGISTER_INDEX_CASE(5, s5)
            REGISTER_INDEX_CASE(6, s6)
            REGISTER_INDEX_CASE(7, s7)
            REGISTER_INDEX_CASE(8, s8)
            REGISTER_INDEX_CASE(9, s9)
            REGISTER_INDEX_CASE(10, s10)
            REGISTER_INDEX_CASE(11, s11)
            default: return Register::zero;
        }
    }

    Register FloatPersistentAtIndex(int index) {
        switch (index) {
            REGISTER_INDEX_CASE(0, fs0)
            REGISTER_INDEX_CASE(1, fs1)
            REGISTER_INDEX_CASE(2, fs2)
            REGISTER_INDEX_CASE(3, fs3)
            REGISTER_INDEX_CASE(4, fs4)
            REGISTER_INDEX_CASE(5, fs5)
            REGISTER_INDEX_CASE(6, fs6)
            REGISTER_INDEX_CASE(7, fs7)
            REGISTER_INDEX_CASE(8, fs8)
            REGISTER_INDEX_CASE(9, fs9)
            REGISTER_INDEX_CASE(10, fs10)
            REGISTER_INDEX_CASE(11, fs11)
            default: return Register::zero;
        }
    }

    int IndexOfIntegerPersistent(Register reg) {
        switch (reg) {
            INDEX_REGISTER_CASE(s0, 0)
            INDEX_REGISTER_CASE(s1, 1)
            INDEX_REGISTER_CASE(s2, 2)
            INDEX_REGISTER_CASE(s3, 3)
            INDEX_REGISTER_CASE(s4, 4)
            INDEX_REGISTER_CASE(s5, 5)
            INDEX_REGISTER_CASE(s6, 6)
            INDEX_REGISTER_CASE(s7, 7)
            INDEX_REGISTER_CASE(s8, 8)
            INDEX_REGISTER_CASE(s9, 9)
            INDEX_REGISTER_CASE(s10, 10)
            INDEX_REGISTER_CASE(s11, 11)
            default: return -1;
        }
    }

    int IndexOfFloatPersistent(Register reg) {
        switch (reg) {
            INDEX_REGISTER_CASE(fs0, 0)
            INDEX_REGISTER_CASE(fs1, 1)
            INDEX_REGISTER_CASE(fs2, 2)
            INDEX_REGISTER_CASE(fs3, 3)
            INDEX_REGISTER_CASE(fs4, 4)
            INDEX_REGISTER_CASE(fs5, 5)
            INDEX_REGISTER_CASE(fs6, 6)
            INDEX_REGISTER_CASE(fs7, 7)
            INDEX_REGISTER_CASE(fs8, 8)
            INDEX_REGISTER_CASE(fs9, 9)
            INDEX_REGISTER_CASE(fs10, 10)
            INDEX_REGISTER_CASE(fs11, 11)
            default: return -1;
        }
    }

    bool IsFloatRegister(Register reg) {
        return reg >= Register::ft0;
    }

}