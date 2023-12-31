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

void UpdateCpsr(arm_core p, uint32_t dest, uint8_t s, uint8_t rd, uint8_t c, uint8_t v){
	uint8_t n, z;

	n = (dest >> 31) & 0x01;
	z = (dest == 0) ? 1 : 0;
	if (s == 1 && rd == 15){
		if (arm_current_mode_has_spsr(p))
			arm_write_cpsr(p, arm_read_spsr(p));
	}
	else if (s == 1){
		uint32_t new_cpsr;
		if (v == 255)// si v = -1 on ne modifie pas le flag v / 225 = 0xFF	= -1 
			new_cpsr = (arm_read_cpsr(p) & 0x0FFFFFFF) | (c << 29) | (n << 31) | (z << 30);
		else 
			new_cpsr = (arm_read_cpsr(p) & 0x0FFFFFFF) | (c << 29) | (n << 31) | (z << 30) | (v << 28);
		arm_write_cpsr(p, new_cpsr);
	}
}
uint64_t CarryFromAdd(uint32_t a, uint32_t b){
	int64_t a2 = a;
	int64_t b2 = b;
	int64_t result = a2 + b2;
	return get_bit(result, 32);
}
uint32_t And(arm_core p, uint8_t s, uint8_t rn, uint8_t rd, uint32_t op2, uint8_t carry_out){
	uint32_t src = arm_read_register(p, rn);
	uint32_t dest = arm_read_register(p, rd);
	uint8_t c = carry_out;
	// flag v = unnaffected donc -1
	UpdateCpsr(p, dest, s, rd, c, -1);
	dest = src & op2;
	return dest;
}
uint32_t Eor(arm_core p, uint8_t s, uint8_t rn, uint8_t rd, uint32_t op2, uint8_t carry_out){
	uint32_t src = arm_read_register(p, rn);
	uint32_t dest = arm_read_register(p, rd);
	uint8_t c = carry_out;
	// flag v = unnaffected donc -1
	UpdateCpsr(p, dest, s, rd, c, -1);

	dest = src ^ op2;
	return dest;
}
uint32_t Sub(arm_core p, uint8_t s, uint8_t rn, uint8_t rd, uint32_t op2, uint8_t carry_out){
	uint32_t src = arm_read_register(p, rn);
	uint32_t dest = arm_read_register(p, rd);

	// C Flag = NOT BorrowFrom(Rn - shifter_operand)
	uint8_t c = (src < op2) ? 0 : 1;
	// V Flag = OverflowFrom(Rn - shifter_operand)
	uint8_t v = __builtin_sub_overflow(src, op2, &dest);
	UpdateCpsr(p, dest, s, rd, c, v);

	return dest;
}

uint32_t Rsb(arm_core p, uint8_t s, uint8_t rn, uint8_t rd, uint32_t op2, uint8_t carry_out){
	uint32_t src = arm_read_register(p, rn);
	uint32_t dest = arm_read_register(p, rd);

	dest = op2 - src;

	// C Flag = NOT BorrowFrom( Shifter_operand - Rn)
	uint8_t c = (op2 < src) ? 0 : 1;
	// V Flag = OverflowFrom(shifter_operand - Rn)
	uint8_t v = __builtin_sub_overflow(op2, src, &dest);
	UpdateCpsr(p, dest, s, rd, c, v);

	return dest;
}
uint32_t Add(arm_core p, uint8_t s, uint8_t rn, uint8_t rd, uint32_t op2, uint8_t carry_out){
	uint32_t src = arm_read_register(p, rn);
	uint32_t dest = arm_read_register(p, rd);

	dest = src + op2;

	// C Flag = CarryFrom(Rn + shifter_operand)
	uint8_t c = CarryFromAdd(src, op2);
	// V Flag = OverflowFrom(Rn - shifter_operand)
	uint8_t v = __builtin_add_overflow(src, op2, &dest);
	UpdateCpsr(p, dest, s, rd, c, v);

	return dest;
}
uint32_t Adc(arm_core p, uint8_t s, uint8_t rn, uint8_t rd, uint32_t op2, uint8_t carry_out){
	uint32_t src = arm_read_register(p, rn);
	uint32_t dest = arm_read_register(p, rd);
	uint8_t carry = (arm_read_cpsr(p) >> 29) & 0x01;

	// C Flag = CarryFrom(Rn + shifter_operand + C Flag)
	uint8_t c = CarryFromAdd(src, op2 + carry);
	// V Flag = OverflowFrom(Rn + shifter_operand + C Flag)
	uint8_t v = __builtin_add_overflow(src, op2 + carry, &dest);
	UpdateCpsr(p, dest, s, rd, c, v);

	dest = src + op2 + carry;

	return dest;
}
uint32_t Sbc(arm_core p, uint8_t s, uint8_t rn, uint8_t rd, uint32_t op2, uint8_t carry_out){
	uint32_t src = arm_read_register(p, rn);
	uint32_t dest = arm_read_register(p, rd);
	uint8_t notCarry = (arm_read_cpsr(p) >> 29) & 0x01;
	notCarry = ~notCarry & 0x01;

	// C Flag = NOT BorrowFrom(Rn - shifter_operand - NOT(C Flag))
	uint8_t c = (src < op2 - notCarry) ? 0 : 1;
	// V Flag = OverflowFrom(Rn - shifter_operand - NOT(C Flag))
	uint8_t v = __builtin_sub_overflow(src, op2 - ~notCarry, &dest);
	UpdateCpsr(p, dest, s, rd, c, v);

	dest = src - op2 - notCarry;
	return dest;
}
uint32_t Rsc(arm_core p, uint8_t s, uint8_t rn, uint8_t rd, uint32_t op2, uint8_t carry_out){
	uint32_t src = arm_read_register(p, rn);
	uint32_t dest = arm_read_register(p, rd);
	uint8_t NotCarry = (arm_read_cpsr(p) >> 29) & 0x01;
	NotCarry = ~NotCarry & 0x01;

	// C Flag = NOT BorrowFrom(shifter_operand - Rn - NOT(C Flag))
	uint8_t c = (op2 < src - NotCarry) ? 0 : 1;
	// V Flag = OverflowFrom(shifter_operand - Rn - NOT(C Flag))
	uint8_t v = __builtin_sub_overflow(op2, src - ~NotCarry, &dest);
	UpdateCpsr(p, dest, s, rd, c, v);

	dest = op2 - src - NotCarry;
	return dest;
}
void Tst(arm_core p, uint8_t s, uint8_t rn, uint8_t rd, uint32_t op2, uint8_t carry_out){
	uint32_t src = arm_read_register(p, rn);
	uint32_t alu_out = src & op2;

	uint8_t c = carry_out;
	// flag v = unnaffected donc -1
	UpdateCpsr(p, alu_out, 1, 0, c, -1);
}
void Teq(arm_core p, uint8_t s, uint8_t rn, uint8_t rd, uint32_t op2, uint8_t carry_out){
	uint32_t src = arm_read_register(p, rn);
	uint32_t alu_out = src ^ op2;

	uint8_t c = carry_out;
	// flag v = unnaffected donc -1
	UpdateCpsr(p, alu_out, 1, 0, c, -1);
}
void Cmp(arm_core p, uint8_t s, uint8_t rn, uint8_t rd, uint32_t op2, uint8_t carry_out){ 
	uint32_t src = arm_read_register(p, rn);
	uint32_t alu_out = src - op2;

	// C Flag = NOT BorrowFrom(Rn - shifter_operand)
	uint8_t c = (src < op2) ? 0 : 1;
	// V Flag = OverflowFrom(Rn - shifter_operand)
	uint8_t v = __builtin_sub_overflow(src, op2, &alu_out);
	UpdateCpsr(p, alu_out, 1, 0, c, v);
}
void Cmn(arm_core p, uint8_t s, uint8_t rn, uint8_t rd, uint32_t op2, uint8_t carry_out){
	uint32_t src = arm_read_register(p, rn);
	uint32_t alu_out = src + op2;

	uint8_t c = CarryFromAdd(src, op2);
	uint8_t v = __builtin_add_overflow(src, op2, &alu_out);

	UpdateCpsr(p, alu_out, 1, 0, c, v);
}
uint32_t Orr(arm_core p, uint8_t s, uint8_t rn, uint8_t rd, uint32_t op2, uint8_t carry_out){ 
	uint32_t src = arm_read_register(p, rn);
	uint32_t dest = arm_read_register(p, rd);

	dest = src | op2;

	uint8_t c = carry_out;
	// flag v = unnaffected donc -1
	UpdateCpsr(p, dest, s, rd, c, -1);
	return dest;
}
uint32_t Mov(arm_core p, uint8_t s, uint8_t rn, uint8_t rd, uint32_t op2, uint8_t carry_out){ 
	uint32_t dest = arm_read_register(p, rd);
	dest = op2;

	uint8_t c = carry_out;
	// flag v = unnaffected donc -1
	UpdateCpsr(p, dest, s, rd, c, -1);

	return dest;
}
uint32_t Bic(arm_core p, uint8_t s, uint8_t rn, uint8_t rd, uint32_t op2, uint8_t carry_out){
	uint32_t src = arm_read_register(p, rn);
	uint32_t dest = arm_read_register(p, rd);

	dest = src & (~op2);

	uint8_t c = carry_out;
	// flag v = unnaffected donc -1
	UpdateCpsr(p, dest, s, rd, c, -1);

	return dest;
}
uint32_t Mvn(arm_core p, uint8_t s, uint8_t rn, uint8_t rd, uint32_t op2, uint8_t carry_out){ 
	uint32_t dest = arm_read_register(p, rd);
	dest = ~op2;
	uint8_t c = carry_out;
	// flag v = unnaffected donc -1
	UpdateCpsr(p, dest, s, rd, c, -1);
	return dest;
}
uint32_t shift(arm_core p, uint32_t ins, uint8_t *carry_out){
	uint32_t value;
	uint32_t rm = ins & 0x0F;
	uint8_t shift = (ins >> 4) & 0x01;
	uint8_t mode;
	uint8_t reg_imm;
	uint8_t carry = (arm_read_cpsr(p) >> 29) & 0x01;
	if (shift == 0){
		mode = (ins >> 5) & 0x03;
		reg_imm = (ins >> 7) & 0x1F;	  // shift_imm
		value = arm_read_register(p, rm); // on prend la valeur du registre dans rm
		if (mode == 0 && reg_imm == 0){
			*carry_out = carry;
			return value;
		}
		switch (mode){
		case LSL:
			value = value << reg_imm;
			if (reg_imm == 0){
				value = 0;
				*carry_out = carry;
			}
			else
				*carry_out = get_bit(arm_read_register(p, rm), 32 - reg_imm);
			break;
		case LSR:
			value = value >> reg_imm;
			if (reg_imm == 0)
				*carry_out = get_bit(arm_read_register(p, rm), 31);
			else
				*carry_out = get_bit(arm_read_register(p, rm), reg_imm - 1);
			break;
		case ASR:
			value = asr(value, reg_imm);
			if (reg_imm == 0){
				if (get_bit(arm_read_register(p, rm), 31) == 0){
					value = 0;
					*carry_out = get_bit(arm_read_register(p, rm), 31);
				}
				else{
					value = 0xFFFFFFFF;
					*carry_out = get_bit(arm_read_register(p, rm), 31);
				}
			}
			else
				*carry_out = get_bit(arm_read_register(p, rm), reg_imm - 1);
			break;
		case ROR:
			if (reg_imm != 0){
				value = ror(value, reg_imm);
				*carry_out = get_bit(arm_read_register(p, rm), reg_imm - 1);
			}
			else{
				value = value >> 1 | carry << 31;
				*carry_out = get_bit(arm_read_register(p, rm), 0);
			}
			break;
		default:
			value = 0;
			*carry_out = 0;
			break;
		}
	}
	else{
		mode = (ins >> 5) & 0x03;
		reg_imm = (ins >> 8) & 0x0F; // rs
		value = arm_read_register(p, rm);
		reg_imm = arm_read_register(p, reg_imm);
		switch (mode)
		{
		case LSL:
			value = value << reg_imm;
			if (reg_imm == 0)
				*carry_out = carry;
			else if (reg_imm < 32)
				*carry_out = get_bit(arm_read_register(p, rm), 32 - reg_imm);
			else if (reg_imm == 32)
				*carry_out = get_bit(arm_read_register(p, rm), 0);
			else
				*carry_out = 0;
			break;
		case LSR:
			value = value >> reg_imm;
			if (reg_imm == 0)
				*carry_out = carry;
			else if (reg_imm < 32)
				*carry_out = get_bit(arm_read_register(p, rm), reg_imm - 1);
			else if (reg_imm == 32)
				*carry_out = get_bit(arm_read_register(p, rm), 31);
			else
				*carry_out = 0;
			break;
		case ASR:
			value = asr(value, reg_imm);
			if (reg_imm == 0)
				*carry_out = carry;
			else if (reg_imm < 32)
				*carry_out = get_bit(arm_read_register(p, rm), reg_imm - 1);
			else
				*carry_out = 0;
			break;
		case ROR:
			value = ror(value, reg_imm);
			if (reg_imm == 0)
				*carry_out = carry;
			else if (reg_imm & (0x1F == 0))
				*carry_out = get_bit(arm_read_register(p, rm), 31);
			else
				*carry_out = get_bit(arm_read_register(p, rm), (reg_imm & 0x0F) - 1);
			break;
		default:
			value = 0;
			*carry_out = 0;
			break;
		}
	}
	return value;
}
/* Decoding functions for different classes of instructions */
int arm_data_processing_shift(arm_core p, uint32_t ins){
	uint8_t opcode = (ins >> 21) & 0x0F;
	uint8_t rn = (ins >> 16) & 0x0F;
	uint8_t rd = (ins >> 12) & 0x0F;
	uint8_t s = (ins >> 20) & 0x01;
	uint8_t carry_out = 0;
	uint32_t rm = shift(p, ins, &carry_out);

	switch (opcode)
	{
	case 0x0:
		arm_write_register(p, rd, And(p, s, rn, rd, rm, carry_out));
		break;
	case 0x1:
		arm_write_register(p, rd, Eor(p, s, rn, rd, rm, carry_out));
		break;
	case 0x2:
		arm_write_register(p, rd, Sub(p, s, rn, rd, rm, carry_out));
		break;
	case 0x3:
		arm_write_register(p, rd, Rsb(p, s, rn, rd, rm, carry_out));
		break;
	case 0x4:
		arm_write_register(p, rd, Add(p, s, rn, rd, rm, carry_out));
		break;
	case 0x5:
		arm_write_register(p, rd, Adc(p, s, rn, rd, rm, carry_out));
		break;
	case 0x6:
		arm_write_register(p, rd, Sbc(p, s, rn, rd, rm, carry_out));
		break;
	case 0x7:
		arm_write_register(p, rd, Rsc(p, s, rn, rd, rm, carry_out));
		break;
	case 0x8:
		Tst(p, s, rn, rd, rm, carry_out);
		break;
	case 0x9:
		Teq(p, s, rn, rd, rm, carry_out);
		break;
	case 0xA:
		Cmp(p, s, rn, rd, rm, carry_out);
		break;
	case 0xB:
		Cmn(p, s, rn, rd, rm, carry_out);
		break;
	case 0xC:
		arm_write_register(p, rd, Orr(p, s, rn, rd, rm, carry_out));
		break;
	case 0xD:
		arm_write_register(p, rd, Mov(p, s, rn, rd, rm, carry_out));
		break;
	case 0xE:
		arm_write_register(p, rd, Bic(p, s, rn, rd, rm, carry_out));
		break;
	case 0xF:
		arm_write_register(p, rd, Mvn(p, s, rn, rd, rm, carry_out));
		break;

	default:
		return UNDEFINED_INSTRUCTION;
	}
	return 0;
}
uint32_t rotate(arm_core p, uint32_t ins, uint8_t *carry_out){
	uint8_t immed_8 = ins & 0xFF;
	uint8_t rotate_imm = (ins >> 8) & 0x0F;
	uint32_t immediate = (immed_8 >> (2 * rotate_imm)) | (immed_8 << (32 - 2 * rotate_imm));
	if (rotate_imm == 0)
		*carry_out = get_bit(arm_read_cpsr(p), 29);
	else
		*carry_out = get_bit(immediate, 31);
	return immediate;
}
int arm_data_processing_immediate_msr(arm_core p, uint32_t ins){
	uint8_t opcode = (ins >> 21) & 0x0F;
	uint8_t rn = (ins >> 16) & 0x0F;
	uint8_t rd = (ins >> 12) & 0x0F;
	uint8_t carry_out = 0;
	uint8_t s = (ins >> 20) & 0x01;

	uint32_t immediate = rotate(p, ins, &carry_out);

	switch (opcode)
	{
	case 0x0:
		arm_write_register(p, rd, And(p, s, rn, rd, immediate, carry_out));
		break;
	case 0x1:
		arm_write_register(p, rd, Eor(p, s, rn, rd, immediate, carry_out));
		break;
	case 0x2:
		arm_write_register(p, rd, Sub(p, s, rn, rd, immediate, carry_out));
		break;
	case 0x3:
		arm_write_register(p, rd, Rsb(p, s, rn, rd, immediate, carry_out));
		break;
	case 0x4:
		arm_write_register(p, rd, Add(p, s, rn, rd, immediate, carry_out));
		break;
	case 0x5:
		arm_write_register(p, rd, Adc(p, s, rn, rd, immediate, carry_out));
		break;
	case 0x6:
		arm_write_register(p, rd, Sbc(p, s, rn, rd, immediate, carry_out));
		break;
	case 0x7:
		arm_write_register(p, rd, Rsc(p, s, rn, rd, immediate, carry_out));
		break;
	case 0x8:
		Tst(p, s, rn, rd, immediate, carry_out);
		break;
	case 0x9:
		Teq(p, s, rn, rd, immediate, carry_out);
		break;
	case 0xA:
		Cmp(p, s, rn, rd, immediate, carry_out);
		break;
	case 0xB:
		Cmn(p, s, rn, rd, immediate, carry_out);
		break;
	case 0xC:
		arm_write_register(p, rd, Orr(p, s, rn, rd, immediate, carry_out));
		break;
	case 0xD:
		arm_write_register(p, rd, Mov(p, s, rn, rd, immediate, carry_out));
		break;
	case 0xE:
		arm_write_register(p, rd, Bic(p, s, rn, rd, immediate, carry_out));
		break;
	case 0xF:
		arm_write_register(p, rd, Mvn(p, s, rn, rd, immediate, carry_out));
		break;

	default:
		return UNDEFINED_INSTRUCTION;
	}
	return 0;
}