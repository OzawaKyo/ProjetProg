#include "arm_conditions.h"
#include "arm_constants.h"

uint8_t arm_check_cond(arm_core p,uint32_t instruction){
    uint32_t cond = (instruction >> 28) & 0xF;
    uint32_t nzcv = (arm_read_cpsr(p) >> 28) & 0xF;
    uint32_t neg = (nzcv >> 3) & 0x1;
    uint32_t zero = (nzcv >> 2) & 0x1;
    uint32_t carry = (nzcv >> 1) & 0x1;
    uint32_t over = nzcv & 0x1;

    switch (cond)
    {
    case EQ:
        if (zero)
            return 1;
        break;
    case NE:
        if (!zero)
            return 1;
        break;    
    case CS:
        if (carry)
            return 1;
        break;
    case CC:
        if (!carry)
            return 1;
        break; 
    case MI:
        if (neg)
            return 1;
        break;
    case PL:
        if (!neg)
            return 1;
        break;
    case VS:
        if (over)
            return 1;
        break;
    case VC:
        if (!over)
            return 1;
        break;
    case HI:
        if (carry && (!zero))
            return 1;
        break;
    case LS:
        if ((!carry) || zero)
            return 1;
        break;
    case GE:
        if (neg==over)
            return 1;
        break;
    case LT:
        if (neg!=over)
            return 1;
        break;
    case GT:
        if ((!zero) && (neg==over))
            return 1;
        break;
    case LE:
        if (zero && (neg!=over))
            return 1;
        break;
    case AL:
        return 1;
        break;
    case UN:
        return UNDEFINED_INSTRUCTION; //non traité en v4, on peut s'en occuper plus tard pour notre v5
        break;
    default:
        return 0; 
    }
    return 0;
}