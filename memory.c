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

struct memory_data {
    uint8_t value8;
    uint16_t value16;
    uint32_t value32;
    int fin;
};

memory memory_create(size_t size) {
    memory mem = malloc(sizeof(struct memory_data) * (size + 1));
    if (mem != NULL) {
        for (size_t i = 0; i < size; i++) {
            mem[i].value8 = NULL;
            mem[i].value16 = NULL;
            mem[i].value32 = NULL;
            mem[i].fin = 0;
        }
        mem[size].fin = 1;
    }
    return mem;
}

size_t memory_get_size(memory mem) {
    size_t i = 0;
    while (mem[i].fin != 1) {
        i++;
    }
    return i;
}

void memory_destroy(memory mem) {
    free(mem);
}

int memory_read_byte(memory mem, uint32_t address, uint8_t *value) {
   if (address + 1 >= memory_get_size(mem)) {
        return -1;
    }
    value = mem[address].value8;
    return 0; 
}

int memory_read_half(memory mem, uint32_t address, uint16_t *value, uint8_t be) { // gpt le frero 
    // if (address + 1 >= memory_get_size(mem)) {
    //     return -1;
    // }
    // if (be) {
    //     // Big-endian byte order
    //     *value = (uint16_t)(mem[address].value[0] << 8) | mem[address + 1].value[0];
    // } else {
    //     // Little-endian byte order
    //     *value = (uint16_t)(mem[address + 1].value[0] << 8) | mem[address].value[0];
    // }
    return 0;
}

int memory_read_word(memory mem, uint32_t address, uint32_t *value, uint8_t be) { //weeeeeeeeeeeesh // (gpt) ça charbonne ici wsh
    if (mem == NULL || value == NULL) {
        return -1;
    }
    if (address +3 >= memory_get_size(mem)) {
        return -1; 
    }
    if (be) {
        *value = (uint32_t)(mem[address].value[0] << 24) |
                    (uint32_t)(mem[address + 1].value[0] << 16) |
                        (uint32_t)(mem[address + 2].value[0] << 8) |
                            (uint32_t)(mem[address + 3].value[0]);
    } else {
        *value = (uint32_t)(mem[address + 3].value[0] << 24) |
                            (uint32_t)(mem[address + 2].value[0] << 16) |
                            (uint32_t)(mem[address + 1].value[0] << 8) |
                            (uint32_t)(mem[address].value[0]);
    }
    return 0;

}

int memory_write_byte(memory mem, uint32_t address, uint8_t value) {
    if (address + 1 >= memory_get_size(mem)) {
        return -1;
    }
    mem[address].value8 = value; //cest pas bon// :thmubs down: //go essayer ça// c'est bon //non //si ça y est //non // normalement c'est bon // c'est toujours la cas ?
    return 0;
}

int memory_write_half(memory mem, uint32_t address, uint16_t value, uint8_t be) {
    // if (address + 1 >= memory_get_size(mem)) {
    //     return -1; // Out of bounds
    // }

    // if (be) {
    //     // Big-endian byte order
    //     mem[address].value = (value >> 8) & 0xFF;
    //     mem[address + 1].value = value & 0xFF;
    // } else {
    //     // Little-endian byte order
    //     mem[address + 1].value = (value >> 8) & 0xFF;
    //     mem[address].value = value & 0xFF;
    // }

    return 0;
}

int memory_write_word(memory mem, uint32_t address, uint32_t value, uint8_t be) {
    return -1;
}
