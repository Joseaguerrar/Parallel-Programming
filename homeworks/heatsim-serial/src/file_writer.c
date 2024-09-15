//  Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "heat_simulation.h"

//Implementación de generate_report_file
void generate_report_file(const char* folder, const char* jobName, params_matrix* variables, uint64_t* states_k, uint64_t lines){
    char report_name[1024]; char jobName_no_txt[512]; char time_formatted[48];


    strncpy(jobName_no_txt, jobName, sizeof(jobName_no_txt)-1);
    jobName_no_txt[sizeof(jobName_no_txt)-1]= '\0'; 
    char* position =strstr(jobName_no_txt,".txt");
    if (position)
    {
        *position ='\0';
    }
    
}