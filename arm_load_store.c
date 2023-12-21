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
#include "arm_core.h"
#include "arm_load_store.h"
#include "arm_exception.h"
#include "arm_constants.h"
#include "util.h"
#include "debug.h"
#include "registers.h"

#define ARM_INS_STORE 0
#define ARM_INS_LOAD 1

#define ARM_INS_LDR 0
#define ARM_INS_LDRB 1
//#define ARM_INS_LDRH 1
//#define ARM_INS_LDM1 0

#define ARM_INS_STR 0
#define ARM_INS_STRB_STRH 1
//#define ARM_INS_STRH

//TODO: Eliminer la redondance !


int arm_load_store(arm_core p, uint32_t ins) {
    if(ins == NULL || p == NULL){
        return -1;
    }
    uint32_t ls = (ins >> 20) & 1; // Extract bit L (Load if set || Store if unset) from instruction
    switch (ls)
    {
    case ARM_INS_STORE:
        store(p, ins);
        break;
    case ARM_INS_LOAD:
        load(p, ins);       
        break;
    default:
        break;
    }
    return 0;
}

int store(arm_core p, uint32_t ins){
    if(ins == NULL || p == NULL){
        return -1;
    }
    uint32_t x = (ins >> 22) & 1; // Extract bit B (Unsigned byte access if set || Word access if unset) from instruction
    switch (x)
    {
    case ARM_INS_STR:
        str(p, ins);
        break;
    
    case ARM_INS_STRB_STRH:
        strb_strh(p, ins);
        break;
        
    default:
        break;
    }
    return 0;
}


int load(arm_core p, uint32_t ins){
    if(ins == NULL || p == NULL){
        return -1;
    }
    uint32_t x = (ins >> 22) & 1; // Extract bit B (Unsigned byte access if set || Word access if unset) from instruction
    switch (x) {
        case 0:
            ldr(p, ins);
            break;
        case 1:
            ldrb_ldrh(p, ins);
            break;
        default:
            break;
    }
    return 0;
}

int str(arm_core p, uint32_t ins){
    if(ins == NULL || p == NULL){
        return -1;
    }
    uint8_t source = (ins >> 16) & 0x04; // Rn
	uint8_t dest = (ins >> 12) & 0x04; // Rd
    uint8_t offset = (ins >> 0) & 0x0C;
    //uint32_t cond = (ins >> 28); // necessaire ?

    uint32_t v_dest = arm_read_register(p->reg, dest); // valeur dans Rd
    uint32_t v_source = arm_read_register(p->reg, source); // valeur dans Rn

    uint32_t immediat = (ins >> 25) & 1; 
    uint32_t bitP = (ins >> 24) & 1;
    uint32_t bitW = (ins >> 21) & 1;
    uint32_t bitU = (ins >> 23) & 1;
    uint32_t scaled = (ins >> 4) & 8;

    uint32_t address;
    
    if(immediat == 0){ //adressage immédiat
        
        if (bitP == 0) {
            //post_indexé
            if (bitW == 0) {
                address = v_source;
                if (bitU == 1) {
                    v_source = v_source + offset;
                } else {
                    v_source = v_source - offset;
                }
            }          
        } else{
            // pré-indexé ou offset
            uint32_t v_source = arm_read_register(p->reg, source);
            if (bitW == 0) {
                // offset
                if (bitU == 1) {
                    address = v_source + offset; 
                } else {
                    address = v_source - offset;
                }
            } else {
                // pré-indexé
                if (bitU == 1) {
                    address = v_source + offset;
                } else {
                    address = v_source - offset;
                }
                v_source = address;
                arm_write_register(p->reg, source, v_source);
            }
        }
    } else { 
        
        uint8_t Rm = (ins >> 0) & 0x04;
        uint32_t offset = arm_read_register(p->reg, Rm);
        

        if(scaled != 0){ // on est dans le scaled register

            uint32_t shift = (ins >> 5) & 2;
            uint32_t shift_imm = (ins >> 7) & 5;
            int index;

            if(bitP == 0){ //post_indexé 
                if (bitW == 0) {
                    address = v_source;
                    switch (shift) {
                        case 0b00:
                            // LSL
                            // index = Rm Logical_Shift_Left shift_imm
                            index = (Rm << shift_imm);
                            break;
                        case 0b01:
                            // LSR
                            if (shift_imm == 0) {
                                index = 0;
                            } else {
                                //index = Rm Logical_Shift_Right shift_imm
                                index = (Rm >> shift_imm);
                            }
                            break;
                        case 0b10:
                            // ASR
                            if (shift_imm == 0) {
                                
                                /*if (p->reg->Rm[31] == 1) {
                                    index = 0xFFFFFFFF;
                                } else {
                                    index = 0;
                                }*/
                                
                            } else {
                                // index = Rm Arithmetic_Shift_Right shift_imm
                                index = asr(Rm, shift_imm);
                            }
                            break;
                        case 0b11:
                           /* ROR or RRX */
                            if (shift_imm == 0) {
                                /* RRX */
                                //index = (C Flag Logical_Shift_Left 31) OR (Rm Logical_Shift_Right 1)
                                index = (Rm >> 1);
                            } else {
                                /* ROR */
                                //index = Rm Rotate_Right shift_imm
                                index = ror(Rm, shift_imm);
                            }
                            break;
                        default:
                            break;

                        }
                    if (bitU == 1) {
                        v_source = v_source + index;
                    } else {
                        v_source = v_source - index;
                    }
                }

            } else { // bitP = 1
                // 
                if(bitW == 0){ //offset

                switch (shift) {
                        case 0b00:
                            // LSL
                            // index = Rm Logical_Shift_Left shift_imm
                            index = (Rm << shift_imm);
                            break;
                        case 0b01:
                            // LSR
                            if (shift_imm == 0) {
                                index = 0;
                            } else {
                                //index = Rm Logical_Shift_Right shift_imm
                                index = (Rm >> shift_imm);
                            }
                            break;
                        case 0b10:
                            // ASR
                            if (shift_imm == 0) {
                                
                                /*if (p->reg->Rm[31] == 1) {
                                    index = 0xFFFFFFFF;
                                } else {
                                    index = 0;
                                }*/
                                
                            } else {
                                // index = Rm Arithmetic_Shift_Right shift_imm
                                index = asr(Rm, shift_imm);
                            }
                            break;
                        case 0b11:
                           /* ROR or RRX */
                            if (shift_imm == 0) {
                                /* RRX */
                                //index = (C Flag Logical_Shift_Left 31) OR (Rm Logical_Shift_Right 1)
                                index = (Rm >> 1);
                            } else {
                                /* ROR */
                                //index = Rm Rotate_Right shift_imm
                                index = ror(Rm, shift_imm);
                            }
                            break;
                        default:
                            break;
                }
                    if (bitU == 1) {
                        address = v_source + index;
                    } else {
                        address = v_source - index;
                    }                    
                } else { // pre_indexé

                switch (shift) {
                        case 0b00:
                            // LSL
                            // index = Rm Logical_Shift_Left shift_imm
                            index = (Rm << shift_imm);
                            break;
                        case 0b01:
                            // LSR
                            if (shift_imm == 0) {
                                index = 0;
                            } else {
                                //index = Rm Logical_Shift_Right shift_imm
                                index = (Rm >> shift_imm);
                            }
                            break;
                        case 0b10:
                            // ASR
                            if (shift_imm == 0) {
                                
                                /*if (p->reg->Rm[31] == 1) {
                                    index = 0xFFFFFFFF;
                                } else {
                                    index = 0;
                                }*/
                                
                            } else {
                                // index = Rm Arithmetic_Shift_Right shift_imm
                                index = asr(Rm, shift_imm);
                            }
                            break;
                        case 0b11:
                           /* ROR or RRX */
                            if (shift_imm == 0) {
                                /* RRX */
                                //index = (C Flag Logical_Shift_Left 31) OR (Rm Logical_Shift_Right 1)
                                index = (Rm >> 1);
                            } else {
                                /* ROR */
                                //index = Rm Rotate_Right shift_imm
                                index = ror(Rm, shift_imm);
                            }
                            break;
                        default:
                            break;
                }
                    if (bitU == 1) {
                        address = v_source + index;
                    } else {
                        address = v_source - index;
                    }  
                    //la diff avec offset serait la COND 
                    //if cond passed then (rn = addr);
                } 
                         
            }
            
        } else { // register

           if(bitP == 0){ //post_indexé 
                if (bitW == 0) {
                    address = v_source;
                    if (bitU == 1) {
                        address = v_source + offset;
                    } else {
                        address = v_source - offset;
                    }
                }
            } else {
                if(bitW == 0){ //offset
                    if (bitU == 1) {
                        address = v_source + offset;
                    } else {
                        address = v_source - offset;
                    }                    
                } else { // pre_indexé
                    if (bitU == 1) {
                        address = v_source + offset;
                    } else {
                        address = v_source - offset;
                    }
                    v_source = address;
                }          
            }

        }
        
    }


    if(arm_read_word(p->mem, v_dest, v_source)){
        return -1;
    }
    return 0;
}

int strb_strh(arm_core p, uint32_t ins){
    uint32_t x = (ins >> 26) & 1; // Extract bit ???
    switch (x)
    {
    case 0:
        strb(p, ins);
        break;
    case 1:
        strh(p, ins);       
        break;
    default:
        break;
    }
    return 0;
}

int strb(arm_core p, uint32_t ins){
    if(ins == NULL || p == NULL){
        return -1;
    }
    uint8_t source = (ins >> 16) & 0x04;
	uint8_t dest = (ins >> 12) & 0x04;
    //uint32_t cond = (ins >> 28) & 0x1F; // necessaire ?
    
    mode_t mod = registers_get_mode(p->reg);
    uint32_t v_dest = arm_read_register(p->reg, dest);
    uint32_t v_source = arm_read_register(p->reg, source);

    if(arm_write_byte(p->mem, v_dest, v_source)){
        return -1;
    }
    return 0;
}

int strh(arm_core p, uint32_t ins){
    if(ins == NULL || p == NULL){
        return -1;
    }
    uint8_t source = (ins >> 16) & 0x04;
	uint8_t dest = (ins >> 12) & 0x04;
    //uint32_t cond = (ins >> 28) & 0x1F; // necessaire ?
    
    mode_t mod = registers_get_mode(p->reg);
    uint32_t v_dest = arm_read_register(p->reg, dest);
    uint32_t v_source = arm_read_register(p->reg, source);

    if(arm_write_half(p->mem, v_dest, v_source)){
        return -1;
    }
    return 0;
}

int ldrb_ldrh(arm_core p, uint32_t ins) {
    if(ins == NULL || p == NULL){
        return -1;
    }
    uint32_t x = (ins >> 26) & 1; // Extract bit ???
    switch (x) {
        case 1:
            ldrb(p, ins);
            break;
        case 0:
            ldrh(p, ins);
            break;
        default:
            break;
    }
    return 0;
}

int ldr(arm_core p, uint32_t ins) {
    if(ins == NULL || p == NULL){
        return -1;
    }
    uint8_t source = (ins >> 16) & 0x04;
    uint8_t dest = (ins >> 12) & 0x04;
    //uint32_t cond = (ins >> 28) & 0x1F;

    uint32_t value;
    uint32_t v_source = arm_read_register(p->reg, source);
 
    if(arm_read_word(p->mem, v_source, &value)){
        return -1;
    }

    arm_write_register(p->reg, dest, value); 

    return 0;
}



int ldrb(arm_core p, uint32_t ins) {
    if(ins == NULL || p == NULL){
        return -1;
    }
    uint8_t source = (ins >> 16) & 0x04;
    uint8_t dest = (ins >> 12) & 0x04;
    //uint32_t cond = (ins >> 28) & 0x1F;

    uint8_t value;

    if(arm_read_byte(p->mem, source, &value)){
        return -1;
    }

    arm_write_register(p->reg, dest, value); 

    return 0;

}


int ldrh(arm_core p, uint32_t ins) {
    if(ins == NULL || p == NULL){
        return -1;
    }
    uint8_t source = (ins >> 16) & 0x04;
    uint8_t dest = (ins >> 12) & 0x04;
    //uint32_t cond = (ins >> 28) & 0x1F;

    uint16_t value;
    
    if(arm_read_half(p->mem, source, &value)){
        return -1;
    }
    arm_write_register(p->reg, dest, value); 
    return 0;

}

int arm_load_store_multiple(arm_core p, uint32_t ins) {
    if(ins == NULL || p == NULL){
        return -1;
    }
     uint32_t ls = (ins >> 20) & 1; // Extract bit L from instruction
    switch (ls)
    {
    case ARM_INS_STORE:
        stm(p, ins);
        break;
    case ARM_INS_LOAD:
        ldm(p, ins);       
        break;
    default:
        break;
    }
    return 0;
}

int stm(arm_core p, uint32_t ins){
    if(ins == NULL || p == NULL){
        return -1;
    }
   uint32_t b = (ins >> 22) & 1;
    if(b == 0){
        uint32_t k;
        uint8_t bitP;
        uint8_t bitU;
        uint8_t dest = (ins >> 16) & 0x04;
        uint32_t addr = arm_read_register(p->reg, dest);
        uint32_t value;
            for(int i = 0; i < 15; i ++){
                k = (ins >> i) & 1;
                if(k == 1){
                bitP = (ins >> 24) & 1;
                bitU = (ins >> 23) & 1;

                if ((bitP == 0) & (bitU == 0)) {
                    // on est dans le mode DA
                    value = arm_read_register(p->reg, i);
                    if(arm_write_byte(p->mem, addr, value)){
                        return -1;
                    }
                    addr--;                    

                } else if ((bitP == 0) & (bitU == 1)) {
                    // on est dans le mode IA
                    value = arm_read_register(p->reg, i);
                    if(arm_write_byte(p->mem, addr, value)){
                        return -1;
                    }
                    addr++;

                } else if ((bitP == 1) & (bitU == 0)) {
                    // on est dans le mode DB
                    addr--;
                    value = arm_read_register(p->reg, i);
                    if(arm_write_byte(p->mem, addr, value)){
                        return -1;
                    }
                    
                } else {
                    // on est dans le mode IB
                    addr++;
                    value = arm_read_register(p->reg, i);
                    if(arm_write_byte(p->mem, addr, value)){
                        return -1;
                    }
                }  
            }
        } 
    }
    return 0;
}

int ldm(arm_core p, uint32_t ins){
    if(ins == NULL || p == NULL){
        return -1;
    }
    uint32_t b = (ins >> 22) & 1;
    
    if(b == 0){
    
        uint32_t k;
        uint8_t bitP;
        uint8_t bitU;
        uint8_t source = (ins >> 16) & 0x04;
        uint32_t addr = arm_read_register(p->reg, source);
        uint8_t value;
        if(arm_read_byte(p->mem, addr, &value)){
            return -1;
        }
        
        for(int i = 0; i < 15; i ++){
            k = (ins >> i) & 1;
            if(k == 1){
                bitP = (ins >> 24) & 1;
                bitU = (ins >> 23) & 1;

                if ((bitP == 0) & (bitU == 0)) {
                    // on est dans le mode DA
                    arm_write_register(p->reg, i, value);
                    addr--;                    

                } else if ((bitP == 0) & (bitU == 1)) {
                    // on est dans le mode IA
                    arm_write_register(p->reg, i, value);
                    addr++;

                } else if ((bitP == 1) & (bitU == 0)) {
                    // on est dans le mode DB
                    addr--;
                    arm_write_register(p->reg, i, value);
                    
                } else {
                    // mode IB
                    addr++;
                    arm_write_register(p->reg, i, value);
                }               
            }
        } 
    }
    return 0;
}



int arm_coprocessor_load_store(arm_core p, uint32_t ins) {
    /* Not implemented */
    return UNDEFINED_INSTRUCTION;
}
