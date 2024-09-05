//  Copyright [2024] <jose.guerrarodriguez@ucr.ac.cr>
#ifndef PROCESS_JOB_FILE_H
#define PROCESS_JOB_FILE_H

#include <time.h>  // Incluir time.h para definir time_t
#include <stddef.h>  // Incluir stddef.h para definir size_t


void process_job_file(const char *job_file, const char *output_dir);
char* format_time(const time_t seconds, char* text, const size_t capacity);
#endif
