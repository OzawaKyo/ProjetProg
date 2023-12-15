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
#include "registers.h"
#include "arm_constants.h"
#include <stdlib.h>
#include <stdint.h>

struct registers_data {
    uint32_t value;
    // uint8_t mode;
}s_registers_data;

registers registers_create() {
    registers r = malloc(sizeof(s_registers_data)*NB_REGISTER);
    if (r != NULL) {
        for(int i = 0; i < NB_REGISTER; i ++){
            r[i].value = 0; //pas sur que les registres soient initialisés à 0
            // r[i].mode = 0; 
        }
    }
    return r;
}

void registers_destroy(registers r) {
    free(r);
}

uint8_t registers_get_mode(registers r) {
    if (r != NULL) {
        // return r->mode;
        return SVC;
    }
    return 0;
}

static int registers_mode_has_spsr(registers r, uint8_t mode) {
    if(mode == USR || mode == SYS)
        return 0;
    return 1; 
}

int registers_current_mode_has_spsr(registers r) {
    return registers_mode_has_spsr(r, registers_get_mode(r));
}

int registers_in_a_privileged_mode(registers r) {
    if((r[16].value & PrivMask) != 0){ //r[16] = CPSR 
        return 1; //1 = priviligié
    }
    return -1;
}

//registers_read(r, mode, i);
uint32_t registers_read(registers r, uint8_t reg, uint8_t mode) {
    uint32_t value = 0;
    return value = r[reg].value;
}

uint32_t registers_read_cpsr(registers r) {
    if(r != NULL){
        uint32_t value = 0;
        return value = r[16].value & (~CPSR_Reserved_Mask);
    }
    return -1;
}

uint32_t registers_read_spsr(registers r, uint8_t mode) {
    if(r != NULL){
        return -1;
        /*if(r != NULL && mode != NULL){
        return r[18].value; // ya pas de r[18] dans le mode user, donc faut verif le mode et ecrire dans le registre correspondant MAIS POUR L'INSTANT ON FAIT PAS LES MODES 
        }*/
    }
    return -1;
}

void registers_write(registers r, uint8_t reg, uint8_t mode, uint32_t value) { // c'est quoi reg ? c'est genre le nom du registre (dans notre cas, sa position dans notre tableau) mais dans le main ca rentre le nom ou l'indice ?
    if(r != NULL){
        //r[reg]->mode = mode; 
        // dans le test ca appelle comme ca     registers_write(r, 15, mode, 0);
        r[reg].value = value;
    }
}

void registers_write_cpsr(registers r, uint32_t value) {
    if(r != NULL){
        r[16].value = value & (~CPSR_Reserved_Mask);
    }
}

void registers_write_spsr(registers r, uint8_t mode, uint32_t value) {
    if(r != NULL){
        //r[18].value = value;
    }
}
