#include "register.hpp"

namespace ast {
    
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
        }
        return stream;
    }

    int indexOfTemporary(Register reg) {
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

    Register temporaryAtIndex(int index) {
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

}
