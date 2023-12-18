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
#include "memory.h"
#include "util.h"
#include <stdio.h>

struct memory_data{
  uint8_t *value; 
  size_t taille; 
}s_memory_data;

memory memory_create(size_t size) {
    memory mem = malloc(sizeof(s_memory_data));
    if (mem != NULL) {
        mem->value = malloc(sizeof(uint8_t) * size);
        mem->taille = size;
    }
    return mem;
}

size_t memory_get_size(memory mem) {
    return mem->taille;
}

void memory_destroy(memory mem) {
    free(mem->value);
    free(mem);
}

int memory_read_byte(memory mem, uint32_t address, uint8_t *value) {
   if (address > memory_get_size(mem)) {
        return -1;
    }else{
        *value = mem->value[address];
    }
    return 0; 
}

int memory_read_half(memory mem, uint32_t address, uint16_t *value, uint8_t be) { 
    if (address + 1 > memory_get_size(mem)){
        return -1;
    }
    for (int i = 0; i < 2; i++) {
        *value = *value << 8;
        *value = *value | mem->value[address+ i];
    }
    if (be == 0)
        *value = reverse_2(*value);
    return 0;
}

int memory_read_word(memory mem, uint32_t address, uint32_t *value, uint8_t be) { 
    if (address + 3 > memory_get_size(mem)){
        return -1;
    }
    for (int i = 0; i < 4; i++) {
        *value = *value << 8;
        *value = *value | mem->value[address+ i];
    }
    if (be == 0)
        *value = reverse_4(*value);
    return 0;

}

int memory_write_byte(memory mem, uint32_t address, uint8_t value) {
   if (address > memory_get_size(mem)) {
        return -1;
    }else{
        mem->value[address] = value;
    }
    return 0; 
}

int memory_write_half(memory mem, uint32_t address, uint16_t value, uint8_t be) {
    if (address + 1 > memory_get_size(mem)){
        return -1;
    }
    if (be) {
            mem->value[address] = value >> 8;
            mem->value[address + 1] = value;
        } else {
            mem->value[address] = value;
            mem->value[address + 1] = value >> 8;
        }

    return 0;
}

int memory_write_word(memory mem, uint32_t address, uint32_t value, uint8_t be) {
        if (address + 3 > memory_get_size(mem)){
        return -1;
    }

    if (be) {
        mem->value[address] = value >> 24;
        mem->value[address + 1] = value >> 16;
        mem->value[address + 2] = value >> 8;
        mem->value[address + 3] = value;
    } else {
        mem->value[address] = value;
        mem->value[address + 1] = value >> 8;
        mem->value[address + 2] = value >> 16;
        mem->value[address + 3] = value >> 24;
    }

    return 0;
}
