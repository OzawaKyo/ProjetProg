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
#include "arm_instruction.h"
#include "arm_exception.h"
#include "arm_data_processing.h"
#include "arm_load_store.h"
#include "arm_branch_other.h"
#include "arm_constants.h"
#include "util.h"

#define ARM_TYPE_DATA_PROCESSING 0x0
#define ARM_TYPE_DATA_IMMEDIATE_MSR 0x1
#define ARM_TYPE_LOAD_STORE 0x2
#define ARM_TYPE_LOAD_STORE_MULTIPLE 0x4
#define ARM_TYPE_BRANCH_OTHER 0x5
#define ARM_TYPE_COPROCESSOR_LOAD_STORE 0x6
// #define ARM_TYPE_SWI 0x7
// #define ARM_TYPE_ 0x


static int arm_execute_instruction(arm_core p) {
    uint32_t instruction;
    int result = arm_fetch(p, &instruction);
    if (result != 0) {
        return result;
    }
    uint32_t type = (instruction >> 25) & 0x07; // Extract type from instruction

    switch (type) {
        case ARM_TYPE_DATA_PROCESSING:
            result = arm_data_processing_shift(p, instruction);
            break;
        case ARM_TYPE_DATA_IMMEDIATE_MSR:
            result = arm_data_processing_immediate_msr(p, instruction);
            break;
        case ARM_TYPE_LOAD_STORE:
            result = arm_load_store(p, instruction);
            break;
        case ARM_TYPE_LOAD_STORE_MULTIPLE:
            result = arm_load_store_multiple(p, instruction);
            break;
        case ARM_TYPE_COPROCESSOR_LOAD_STORE:
            result = arm_coprocessor_load_store(p, instruction);
            break;
        case ARM_TYPE_BRANCH_OTHER:
            result = arm_branch(p, instruction);
            break;
        default:
            return UNDEFINED_INSTRUCTION;
    }

    return result;
}

int arm_step(arm_core p) {
    int result;

    result = arm_execute_instruction(p);
    if (result) {
        return arm_exception(p, result);
    }
    return result;
}
