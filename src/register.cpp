#include "register.hpp"

namespace ast {

    // todo make this tidier with some macros

    std::ostream &operator<<(std::ostream &stream, Register reg) {
        switch (reg) {
            case Register::zero:
                stream << "zero";
                break;
            case Register::ra:
                stream << "ra";
                break;
            case Register::sp:
                stream << "sp";
                break;
            case Register::gp:
                stream << "gp";
                break;
            case Register::tp:
                stream << "tp";
                break;
            case Register::t0:
                stream << "t0";
                break;
            case Register::t1:
                stream << "t1";
                break;
            case Register::t2:
                stream << "t2";
                break;
            case Register::s0:
                stream << "s0";
                break;
            case Register::s1:
                stream << "s1";
                break;
            case Register::a0:
                stream << "a0";
                break;
            case Register::a1:
                stream << "a1";
                break;
            case Register::a2:
                stream << "a2";
                break;
            case Register::a3:
                stream << "a3";
                break;
            case Register::a4:
                stream << "a4";
                break;
            case Register::a5:
                stream << "a5";
                break;
            case Register::a6:
                stream << "a6";
                break;
            case Register::a7:
                stream << "a7";
                break;
            case Register::s2:
                stream << "s2";
                break;
            case Register::s3:
                stream << "s3";
                break;
            case Register::s4:
                stream << "s4";
                break;
            case Register::s5:
                stream << "s5";
                break;
            case Register::s6:
                stream << "s6";
                break;
            case Register::s7:
                stream << "s7";
                break;
            case Register::s8:
                stream << "s8";
                break;
            case Register::s9:
                stream << "s9";
                break;
            case Register::s10:
                stream << "s10";
                break;
            case Register::s11:
                stream << "s11";
                break;
            case Register::t3:
                stream << "t3";
                break;
            case Register::t4:
                stream << "t4";
                break;
            case Register::t5:
                stream << "t5";
                break;
            case Register::t6:
                stream << "t6";
                break;
            case Register::ft0:
                stream << "ft0";
                break;
            case Register::ft1:
                stream << "ft1";
                break;
            case Register::ft2:
                stream << "ft2";
                break;
            case Register::ft3:
                stream << "ft3";
                break;
            case Register::ft4:
                stream << "ft4";
                break;
            case Register::ft5:
                stream << "ft5";
                break;
            case Register::ft6:
                stream << "ft6";
                break;
            case Register::ft7:
                stream << "ft7";
                break;
            case Register::fs0:
                stream << "fs0";
                break;
            case Register::fs1:
                stream << "fs1";
                break;
            case Register::fa0:
                stream << "fa0";
                break;
            case Register::fa1:
                stream << "fa1";
                break;
            case Register::fa2:
                stream << "fa2";
                break;
            case Register::fa3:
                stream << "fa3";
                break;
            case Register::fa4:
                stream << "fa4";
                break;
            case Register::fa5:
                stream << "fa5";
                break;
            case Register::fa6:
                stream << "fa6";
                break;
            case Register::fa7:
                stream << "fa7";
                break;
            case Register::fs2:
                stream << "fs2";
                break;
            case Register::fs3:
                stream << "fs3";
                break;
            case Register::fs4:
                stream << "fs4";
                break;
            case Register::fs5:
                stream << "fs5";
                break;
            case Register::fs6:
                stream << "fs6";
                break;
            case Register::fs7:
                stream << "fs7";
                break;
            case Register::fs8:
                stream << "fs8";
                break;
            case Register::fs9:
                stream << "fs9";
                break;
            case Register::fs10:
                stream << "fs10";
                break;
            case Register::fs11:
                stream << "fs11";
                break;
            case Register::ft8:
                stream << "ft8";
                break;
            case Register::ft9:
                stream << "ft9";
                break;
            case Register::ft10:
                stream << "ft10";
                break;
            case Register::ft11:
                stream << "ft11";
                break;
        }
        return stream;
    }

    int IndexOfIntegerTemporary(Register reg) {
        switch (reg) {
            case Register::t0:
                return 0;
            case Register::t1:
                return 1;
            case Register::t2:
                return 2;
            case Register::t3:
                return 3;
            case Register::t4:
                return 4;
            case Register::t5:
                return 5;
            case Register::t6:
                return 6;
            default:
                return -1;
        }
    }

    int IndexOfFloatTemporary(Register reg) {
        switch (reg) {
            case Register::ft0:
                return 0;
            case Register::ft1:
                return 1;
            case Register::ft2:
                return 2;
            case Register::ft3:
                return 3;
            case Register::ft4:
                return 4;
            case Register::ft5:
                return 5;
            case Register::ft6:
                return 6;
            case Register::ft7:
                return 7;
            default:
                return -1;
        }
    }

    Register IntegerTemporaryAtIndex(int index) {
        switch (index) {
            case 0:
                return Register::t0;
            case 1:
                return Register::t1;
            case 2:
                return Register::t2;
            case 3:
                return Register::t3;
            case 4:
                return Register::t4;
            case 5:
                return Register::t5;
            case 6:
                return Register::t6;
            default:
                return Register::zero;
        }
    }

    Register FloatTemporaryAtIndex(int index) {
        switch (index) {
            case 0:
                return Register::ft0;
            case 1:
                return Register::ft1;
            case 2:
                return Register::ft2;
            case 3:
                return Register::ft3;
            case 4:
                return Register::ft4;
            case 5:
                return Register::ft5;
            case 6:
                return Register::ft6;
            case 7:
                return Register::ft7;
            default:
                return Register::zero;
        }
    }

    Register IntegerPersistentAtIndex(int index) {
        switch (index) {
            case 0:
                return Register::s0;
            case 1:
                return Register::s1;
            case 2:
                return Register::s2;
            case 3:
                return Register::s3;
            case 4:
                return Register::s4;
            case 5:
                return Register::s5;
            case 6:
                return Register::s6;
            case 7:
                return Register::s7;
            case 8:
                return Register::s8;
            case 9:
                return Register::s9;
            case 10:
                return Register::s10;
            case 11:
                return Register::s11;
            default:
                return Register::zero;
        }
    }

    Register FloatPersistentAtIndex(int index) {
        switch (index) {
            case 0:
                return Register::fs0;
            case 1:
                return Register::fs1;
            case 2:
                return Register::fs2;
            case 3:
                return Register::fs3;
            case 4:
                return Register::fs4;
            case 5:
                return Register::fs5;
            case 6:
                return Register::fs6;
            case 7:
                return Register::fs7;
            case 8:
                return Register::fs8;
            case 9:
                return Register::fs9;
            case 10:
                return Register::fs10;
            case 11:
                return Register::fs11;
            default:
                return Register::zero;
        }
    }

    int IndexOfIntegerPersistent(Register reg) {
        switch (reg) {
            case Register::s0:
                return 0;
            case Register::s1:
                return 1;
            case Register::s2:
                return 2;
            case Register::s3:
                return 3;
            case Register::s4:
                return 4;
            case Register::s5:
                return 5;
            case Register::s6:
                return 6;
            case Register::s7:
                return 7;
            case Register::s8:
                return 8;
            case Register::s9:
                return 9;
            case Register::s10:
                return 10;
            case Register::s11:
                return 11;
            default:
                return -1;
        }
    }

    int IndexOfFloatPersistent(Register reg) {
        switch (reg) {
            case Register::fs0:
                return 0;
            case Register::fs1:
                return 1;
            case Register::fs2:
                return 2;
            case Register::fs3:
                return 3;
            case Register::fs4:
                return 4;
            case Register::fs5:
                return 5;
            case Register::fs6:
                return 6;
            case Register::fs7:
                return 7;
            case Register::fs8:
                return 8;
            case Register::fs9:
                return 9;
            case Register::fs10:
                return 10;
            case Register::fs11:
                return 11;
            default:
                return -1;
        }
    }

    bool IsFloatRegister(Register reg) {
        return reg >= Register::ft0;
    }

}
