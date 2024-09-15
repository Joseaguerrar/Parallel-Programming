//  Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "heat_simulation.h"
void read_bin_plate(const char* folder, params_matrix* variables, uint64_t lines, const char* jobName){
    FILE *bin_file;
    uint64_t rows, columns;
    char direction[512];
    uint64_t* array_state_k=malloc(lines * sizeof(uint64_t));

}

