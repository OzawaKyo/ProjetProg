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
#include "arm_data_processing.h"
#include "arm_exception.h"
#include "arm_constants.h"
#include "arm_branch_other.h"
#include "util.h"
#include "debug.h"
#include "registers.h"
#include "arm_core.h"

/* Decoding functions for different classes of instructions */
int arm_data_processing_shift(arm_core p, uint32_t ins) {
	uint8_t opcode = (ins >> 21) & 0x0F;
    switch (opcode) {
		case 0x0:
		return And(p, ins, 0);
		case 0x1:
		return Eor(p, ins);
		case 0x2:
		return Sub(p, ins);
		case 0x3:
		return Rsb(p, ins);
		case 0x4:
		return Add(p, ins);
		case 0x5:
		return Adc(p, ins);
		case 0x6:
		return Sbc(p, ins);
		case 0x7:
		return Rsc(p, ins);
		case 0x8:
		return Tst(p, ins);
		case 0x9:
		return Teq(p, ins);
		case 0xA:
		return Cmp(p, ins);
		case 0xB:
		return Cmn(p, ins);
		case 0xC:
		return Orr(p, ins);
		case 0xD:
		return Mov(p, ins);
		case 0xE:
		return Bic(p, ins);
		case 0xF:
		return Mvn(p, ins);
		default:
		return UNDEFINED_INSTRUCTION;
}
}
int arm_data_processing_immediate_msr(arm_core p, uint32_t ins) {
    return UNDEFINED_INSTRUCTION;
}

/* Data processing instructions */	
int And(arm_core p, uint32_t ins, uint8_t i) {
	uint8_t s = (ins >> 20) & 0x01;
	uint8_t rn = (ins >> 16) & 0x0F;
	uint8_t rd = (ins >> 12) & 0x0F;
	uint8_t shift = (ins >> 4) & 0x01;
	uint8_t rm = ins & 0x0F;
	uint32_t op2;
	if (i==0)
	{
		if(shift == 0) {
			op2 = registers_read(p->reg, rm, registers_get_mode(p->reg));
		}
		else {
			
		}
	}
	else
	{
		uint8_t rotate_imm = (ins >> 8) & 0x0F;
		if (!rotate_imm)
		{
			op2 = ins & 0xFF;
			// y'a un c flag dans shifter_operand
		}
		else{
			//shifter_operand = (ins & 0xFF) << (rotate_imm*2);
			op2 = (ins & 0xFF) >> (rotate_imm*2);
			// y'a shifter_operand[31] dans shifter_operand
		}
		
	}
	

	rd = rn + op2;
}



/*int Eor(arm_core p, uint32_t ins) {
    uint8_t s = (ins >> 20) & 0x01;    
    uint8_t rn = (ins >> 16) & 0x0F;   
    uint8_t rd = (ins >> 12) & 0x0F;    
    uint8_t i = (ins >> 25) & 0x01;     
    uint8_t shift = (ins >> 4) & 0x01;  
    uint8_t rm = ins & 0x0F;             

    uint32_t op2;  

    // Obtention de la valeur de op2 en fonction du mode d'adressage
    if (!i && !shift) {
        // Si pas de décalage immédiat, op2 est la valeur du registre rm
        op2 = p->reg[rm];

    } else if (!i && shift) {
        // Si décalage immédiat, appliquer la fonction de décalage
        uint8_t shift_type = (ins >> 5) & 0x03;     
        uint8_t shift_amount = (ins >> 7) & 0x1F; 
         if (shift_type == 0b00) {
            // Décalage logique à gauche (LSL)
            op2 = p->reg[rm] << shift_amount;
        } else {
            // Autres types de décalage non pris en charge
            // Gérer les autres cas si nécessaire
            return UNDEFINED_INSTRUCTION;
        }
	} else {
        op2 = ins & 0xFF;
    }

    p->reg[rd] = p->reg[rn] ^ op2;

    return OK;
}*/
