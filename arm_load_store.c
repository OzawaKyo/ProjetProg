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


int arm_load_store(arm_core p, uint32_t ins) {
    uint32_t ls = (ins >> 20); // Extract bit L from instruction
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
}

int store(arm_core p, uint32_t ins){
    uint32_t x = (ins >> 22); // Extract bit 22 from instruction
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
}


int load(arm_core p, uint32_t ins){
    uint32_t x = (ins >> 22);
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
}

int str(arm_core p, uint32_t ins){
    // bon mehdi je modifie tes fonctions un peu// Tu te prend pour qui ?????????
    uint8_t source = (ins >> 16) & 0x0F;
	uint8_t dest = (ins >> 12) & 0x0F;
    uint32_t cond = (ins >> 28); // necessaire ?

    uint8_t be;
    
    mode_t mod = registers_get_mode(p->reg);
    uint32_t value = registers_read(p->reg, source, mod);
    memory_write_word(p->mem, dest, value, be);
}

int strb_strh(arm_core p, uint32_t ins){
    uint32_t x = (ins >> 26); // Extract bit L from instruction
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
}

int strb(arm_core p, uint32_t ins){
    uint8_t source = (ins >> 16) & 0x0F;
	uint8_t dest = (ins >> 12) & 0x0F;
    uint32_t cond = (ins >> 28); // necessaire ?
    
    mode_t mod = registers_get_mode(p->reg);
    uint32_t value = registers_read(p->reg, source, mod);
    memory_write_byte(p->mem, dest, value);    
}

int strh(arm_core p, uint32_t ins){
    uint8_t source = (ins >> 16) & 0x0F;
	uint8_t dest = (ins >> 12) & 0x0F;
    uint32_t cond = (ins >> 28); // necessaire ?

    uint8_t be;
    
    mode_t mod = registers_get_mode(p->reg);
    uint32_t value = registers_read(p->reg, source, mod);
    memory_write_half(p->mem, dest, value, be);
}





int ldrb_ldrh(arm_core p, uint32_t ins) {
    uint32_t x = (ins >> 26);
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
}


int ldr(arm_core p, uint32_t ins) {
    
    uint8_t source = (ins >> 16) & 0x0F;
    uint8_t dest = (ins >> 12) & 0x0F;
    //uint32_t cond = (ins >> 28);

    uint8_t mode = registers_get_mode(p->reg);
    uint8_t value;
    uint8_t be;

    memory_read_word(p->mem, source, &value, be);
    registers_write(p->reg, dest, mode, value); 

}



int ldrb(arm_core p, uint32_t ins) {
    
    uint8_t source = (ins >> 16) & 0x0F;
    uint8_t dest = (ins >> 12) & 0x0F;
    //uint32_t cond = (ins >> 28);

    uint8_t mode = registers_get_mode(p->reg);
    uint8_t value;

    memory_read_byte(p->mem, source, &value);
    registers_write(p->reg, dest, mode, value); 

}


int ldrh(arm_core p, uint32_t ins) {
    
    uint8_t source = (ins >> 16) & 0x0F;
    uint8_t dest = (ins >> 12) & 0x0F;
    //uint32_t cond = (ins >> 28);

    uint8_t mode = registers_get_mode(p->reg);
    uint8_t value;
    uint8_t be;
    
    memory_read_half(p->mem, source, &value, be);
    registers_write(p->reg, dest, mode, value); 

}



int arm_load_store_multiple(arm_core p, uint32_t ins) {
     uint32_t ls = (ins >> 20); // Extract bit L from instruction
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
}

int stm(arm_core p, uint32_t ins){
    //y a un mode d'adressage à prendre en compte MAIS JE LE TROUVE PAAAAAAAAS !!! Ok si avec P U et W ... MAIS JE SAIS PAS à QUOI çA SE REFERE !! ou alors j'ai tout faux 
   uint32_t b = (ins >> 22);
    if(b == 1){

    }
}

int ldm(arm_core p, uint32_t ins){
    uint32_t b = (ins >> 22);
    if(b == 1){
        //parcourir de liste des registre source
        //pour chaque registre lu dans la liste inserer son contenu dans le registre destination 
        //mais en faisant attention au mode de reecriture du registre 
    }
}//mais comment parcourir la liste ou même comment lire le mode, idfk 



int arm_coprocessor_load_store(arm_core p, uint32_t ins) {
    /* Not implemented */
    return UNDEFINED_INSTRUCTION;
}
