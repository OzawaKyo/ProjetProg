/*
Armator - simulateur de jeu d'instruction ARMv5T � but p�dagogique
Copyright (C) 2011 Guillaume Huard
Ce programme est libre, vous pouvez le redistribuer et/ou le modifier selon les
termes de la Licence Publique G�n�rale GNU publi�e par la Free Software
Foundation (version 2 ou bien toute autre version ult�rieure choisie par vous).

Ce programme est distribu� car potentiellement utile, mais SANS AUCUNE
GARANTIE, ni explicite ni implicite, y compris les garanties de
commercialisation ou d'adaptation dans un but sp�cifique. Reportez-vous � la
Licence Publique G�n�rale GNU pour plus de d�tails.

Vous devez avoir re�u une copie de la Licence Publique G�n�rale GNU en m�me
temps que ce programme ; si ce n'est pas le cas, �crivez � la Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,
�tats-Unis.

Contact: Guillaume.Huard@imag.fr
	 B�timent IMAG
	 700 avenue centrale, domaine universitaire
	 38401 Saint Martin d'H�res
*/
#include <stdlib.h>
#include <string.h>
#include "arm_constants.h"

static char *arm_mode_names[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, "USR", "FIQ", "IRQ", "SVC", 0, 0, 0,
    "ABT", 0, 0, 0, "UND", 0, 0, 0, "SYS"
};

static char *arm_register_names[] = { "R00", "R01", "R02", "R03", "R04", "R05",
    "R06", "R07", "R08", "R09", "R10", "R11",
    "R12", "SP", "LR", "PC", "CPSR", "SPSR"
};

static char *arm_exception_names[] = { NULL,
    "reset",
    "data abort",
    "fast interrupt",
    "interrupt",
    "imprecise abort",
    "prefetch abort",
    "undefined instruction",
    "software interrupt",
};

uint8_t arm_get_cond_field_check(uint32_t instruction){
    uint32_t cond = (instruction >> 28) & 0xF;
    uint32_t nzcv = (registers_read_cpsr(p->reg) >> 28) & 0xF;
    uint32_t neg = (nzcv >> 3) & 0x1;
    uint32_t zero = (nzcv >> 2) & 0x1;
    uint32_t carry = (nzcv >> 1) & 0x1;
    uint32_t over = nzcv & 0x1;

    switch (cond)
    {
    case EQ:
        if (zero)
            return 1;
    case NE:
        if (!zero)
            return 1;
    case CS:
        if (carry)
            return 1;
    case CC:
        if (!carry)
            return 1;
    case MI:
        if (neg)
            return 1;
    case PL:
        if (!neg)
            return 1;
    case VS:
        if (over)
            return 1;
    case VC:
        if (!over)
            return 1;
    case HI:
        if (carry && (!zero))
            return 1;
    case LS:
        if ((!carry) || zero)
            return 1;
    case GE:
        if (neg==over)
            return 1;
    case LT:
        if (neg!=over)
            return 1;
    case GT:
        if ((!zero) && (neg==over))
            return 1;
    case LE:
        if (zero && (neg!=over))
            return 1;
    case AL:
        return 1;
    case UN:
        return UNDEFINED_INSTRUCTION;//non traité en v4, on peut s'en occuper plus tard pour notre v5
    default:
        return 0; 
    }
}

char *arm_get_exception_name(unsigned char exception) {
    if (exception < 8)
        return arm_exception_names[exception];
    else
        return NULL;
}

char *arm_get_mode_name(uint8_t mode) {
    return arm_mode_names[mode];
}

int8_t arm_get_mode_number(char *name) {
    for (int i = 0; i < 32; i++)
        if (arm_mode_names[i] && strcmp(name, arm_mode_names[i]) == 0)
            return i;
    return -1;
}

char *arm_get_register_name(uint8_t reg) {
    return arm_register_names[reg];
}
