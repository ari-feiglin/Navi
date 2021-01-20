#ifndef NAVI_HEADER
#define NAVI_HEADER

#include "misc.h"
#include <stdio.h>
#include <stdlib.h>

typedef enum file_type_e {DIRECTORY=1, TEXT_FILE, EXECUTABLE}file_type_t;

typedef struct file_node_s{
    char * name;
    file_type_t file_type;
}file_node_t;

error_code_t file_navigator(char * dir_path);

#endif