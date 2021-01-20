#include "navi.h"

#include <dirent.h>
#include <unistd.h>

#define FILES_PER_ROW (15)

error_code_t get_file_nodes(char * dir_path, file_node_t ** file_array, int * num_of_files){
    error_code_t return_value = ERROR_UNINITIALIZED;
    int number_of_files = -1;
    int i = 0;
    int file_name_len = 0;
    DIR * curr_dir = NULL;
    struct dirent * node = NULL;

    curr_dir = opendir(dir_path);
    if(NULL == curr_dir){
        return_value = print_error("GET_FILE_NODES: Opendir error", ERROR_COULDNT_OPEN);
        goto cleanup;
    }

    do{
        errno = 0;
        node = readdir(curr_dir);
        number_of_files ++;
    }while(node != NULL);

    if(errno != 0){
        return_value = print_error("GET_FILE_NODES: Readdir error", ERROR_COULDNT_READ);
        goto cleanup;
    }

    rewinddir(curr_dir);

    *file_array = calloc(number_of_files, sizeof(file_node_t));
    if(NULL == file_array){
        return_value = print_error("GET_FILE_NODES: Calloc error", ERROR_COULDNT_ALLOCATE_MEMORY);
        goto cleanup;
    }
    *num_of_files = number_of_files;

    for(i=0; i<number_of_files; i++){
        node = readdir(curr_dir);
        if(NULL == node){
            return_value = print_error("GET_FILE_NODES: Readdir error", ERROR_COULDNT_READ);
            goto cleanup;
        }

        (*file_array)[i].file_type = node->d_type;

        file_name_len = strnlen(node->d_name, STRING_LEN);
        (*file_array)[i].name = malloc(file_name_len);
        if(NULL == (*file_array)[i].name){
            return_value = print_error("GET_FILE_NODES: Malloc error", ERROR_COULDNT_ALLOCATE_MEMORY);
            goto cleanup;
        }

        memcpy((*file_array)[i].name, node->d_name, file_name_len);
        (*file_array)[i].name[file_name_len] = 0;
    }

    closedir(curr_dir);

    return_value = ERROR_SUCCESS;

cleanup:
    return return_value;
}

int print_dir_file(char * name){
    int i = 0;
    int num_lines = 5;
    int len = 0;

    fputs("\e[48;2;0;255;0m    \e[0;97m\e[0m\e[1B\e[4D"
         "\e[48;2;200;150;50m          \e[0m\e[1B\e[10D"
         "\e[48;2;200;150;50m          \e[0m\e[1B\e[10D"
         "\e[48;2;200;150;50m          \e[0m\e[1B\e[10D"
         "\e[48;2;200;150;50m          \e[0m\e[1B\e[10D", stdout);
    
    for(i=0; i<strnlen(name, STRING_LEN); i++){
        if(i % 10 == 0 && i > 0){
            fputs("\e[1B\e[10D", stdout);
            num_lines ++;
            len = 0;
        }
        putchar(name[i]);
        len ++;
    }
    printf("\e[%iA\e[%iC", num_lines, 12 - len);
    
    return num_lines;
}

int print_file(char * name){
    int i = 0;
    int num_lines = 5;
    int len = 0;

    fputs("\e[107;31m|\e[107;96m_________\e[0m\e[1B\e[10D"
         "\e[107;31m|\e[107;96m_________\e[0m\e[1B\e[10D"
         "\e[107;31m|\e[107;96m_________\e[0m\e[1B\e[10D"
         "\e[107;31m|\e[107;96m_________\e[0m\e[1B\e[10D"
         "\e[107;31m|\e[107;96m_________\e[0m\e[1B\e[10D", stdout);

    for(i=0; i<strnlen(name, STRING_LEN); i++){
        if(i % 10 == 0 && i > 0){
            fputs("\e[1B\e[10D", stdout);
            num_lines ++;
            len = 0;
        }
        putchar(name[i]);
        len ++;
    }
    printf("\e[%iA\e[%iC", num_lines, 12 - len);

    return num_lines;
}

int print_unknown(char * name){
    char * nums[] = {"1011101010", "1110101010", "1001111011", "1111101001","1110101011"};
    int i = 0;
    int num_lines = 5;
    int len = 0;

    for(i=0; i<sizeof(nums)/sizeof(nums[0]); i++){
        printf("\e[38;2;0;0;0;107;1m%*s\e[0m\e[1B\e[10D", 10, nums[i]);
    }

    for(i=0; i<strnlen(name, STRING_LEN); i++){
        if(i % 10 == 0 && i > 0){
            fputs("\e[1B\e[10D", stdout);
            num_lines ++;
            len = 0;
        }
        putchar(name[i]);
        len ++;
    }
    printf("\e[%iA\e[%iC", num_lines, 12 - len);

    return num_lines;
}

error_code_t print_dir(char * dir_path, file_node_t * file_array, int num_of_files, int * line_lens, int * num_of_lines){
    error_code_t return_value = ERROR_UNINITIALIZED;
    int i = 0;
    int rows = 0;
    int num_lines = 0;
    int max_num_lines = 0;

    system("clear");

    fputs("\e[1m", stdout);
    for(i=0; i<num_of_files; i++){
        if(i % FILES_PER_ROW == 0 && i > 0){
            printf("\e[%iB\n", max_num_lines+2);
            line_lens[rows] = max_num_lines + 2;
            rows ++;
            max_num_lines = 0;
        }

        switch(file_array[i].file_type){
            case DT_DIR:
                num_lines = print_dir_file(file_array[i].name);
                break;
            
            case DT_REG:
                num_lines = print_file(file_array[i].name);
                break;
            
            default:
                num_lines = print_unknown(file_array[i].name);
                break;
        }

        //free(file_array[i].name);

        if(num_lines > max_num_lines){
            max_num_lines = num_lines;
        }
    }

    line_lens[rows] = max_num_lines+2;
    rows ++;

    *num_of_lines = rows;

    return_value = ERROR_SUCCESS;

cleanup:
    return return_value;
}

error_code_t file_navigator(char * input_dir_path){
    error_code_t return_value = ERROR_UNINITIALIZED;
    int i = 0;
    char input = 0;
    int num_of_files = 0;
    int line = 1;
    int column = 1;
    int cursor_x = 0;
    int cursor_y = 0;
    int num_of_lines = 0;
    int selected_file_index = 0;
    int old_dir_len = 0;
    char * relative_path = NULL;
    char * dir_path = NULL;
    char * command = NULL;
    int line_lens[BUFFER_SIZE] = {0};
    struct stat statbuf = {0};
    file_node_t * file_array = NULL;

    edit_echo(false);

    relative_path = calloc(strnlen(input_dir_path, STRING_LEN) + 1, sizeof(char));
    if(NULL == relative_path){
        return_value = print_error("FILE_NAVIGATOR: Calloc error", ERROR_COULDNT_ALLOCATE_MEMORY);
        goto cleanup;
    }
    memcpy(relative_path, input_dir_path, strnlen(input_dir_path, STRING_LEN));
    dir_path = realpath(relative_path, NULL);
    free(relative_path);

    return_value = get_file_nodes(dir_path, &file_array, &num_of_files);
    if(ERROR_SUCCESS != return_value){
        goto cleanup;
    }

    return_value = print_dir(dir_path, file_array, num_of_files, line_lens, &num_of_lines);
    if(ERROR_SUCCESS != return_value){
        goto cleanup;
    }

    cursor_x = 5;
    cursor_y = line_lens[0];

    printf("\e[%i;%iH\e[41m  \e[0m", cursor_y, cursor_x);

    while(input != 'q'){
        input = getchar();

        if('a' == input && ((line > 1 && column >= 1) || (line == 1 && column > 1))){
            column--;
            cursor_x -= 12;

            if(line != 1 && column < 1){
                column = FILES_PER_ROW;
                cursor_x = 12 * FILES_PER_ROW - 7;
                cursor_y -= line_lens[line-1] + 1;
                line --;
            }

            printf("\e[2D\e[0m  \e[%i;%iH\e[41m  \e[0m", cursor_y, cursor_x);
        }
        else if('d' == input && ((line != num_of_lines && column <= FILES_PER_ROW) || (line == num_of_lines && column < num_of_files - FILES_PER_ROW * (num_of_lines-1)))){
            column ++;
            cursor_x += 12;

            if(line != num_of_lines && column > FILES_PER_ROW){
                column = 1;
                cursor_x = 4;
                cursor_y += line_lens[line] + 1;
                line ++;
            }
            printf("\e[2D\e[0m  \e[%i;%iH\e[41m  \e[0m", cursor_y, cursor_x);
        }

        else if('\n' == input){
            selected_file_index = (line - 1) * FILES_PER_ROW + column - 1;

            old_dir_len = strnlen(dir_path, STRING_LEN);
            relative_path = calloc(old_dir_len + strnlen(file_array[selected_file_index].name, STRING_LEN) + 2, sizeof(char));
            if(NULL == relative_path){
                return_value = print_error("FILE_NAVIGATOR: Calloc error", ERROR_COULDNT_ALLOCATE_MEMORY);
                goto cleanup;
            }

            sprintf(relative_path, "%s/%s", dir_path, file_array[selected_file_index].name);

            if(file_array[selected_file_index].file_type == DT_DIR){
                free(dir_path);
                dir_path = realpath(relative_path, NULL);
                free(relative_path);

                for(i=0; i<num_of_files; i++){
                    free(file_array[i].name);
                }

                system("clear");
                free(file_array);
                return_value = get_file_nodes(dir_path, &file_array, &num_of_files);
                if(ERROR_SUCCESS != return_value){
                    goto cleanup;
                }

                return_value = print_dir(dir_path, file_array, num_of_files, line_lens, &num_of_lines);
                if(ERROR_SUCCESS != return_value){
                    goto cleanup;
                }
                line = 1;
                column = 1;
                cursor_x = 5;
                cursor_y = line_lens[0];

                printf("\e[%i;%iH\e[41m  \e[0m", cursor_y, cursor_x);
            }
            else if(file_array[selected_file_index].file_type == DT_REG){
                return_value = stat(relative_path, &statbuf);
                if(-1 == return_value){
                    return_value = print_error("FILE_NAVIGATOR: Stat error", ERROR_COULDNT_STAT);
                    goto cleanup;
                }

                if(statbuf.st_mode & S_IXGRP){
                    command = calloc(strlen("gedit") + strnlen(relative_path, STRING_LEN) + 2, sizeof(char));
                    if(NULL == command){
                        return_value = print_error("FILE_NAVIGATOR: Calloc error", ERROR_COULDNT_ALLOCATE_MEMORY);
                        goto cleanup;
                    }
                    sprintf(command, "gedit '%s'", relative_path);
                }
                else{
                    command = calloc(strlen("xdg-open") + strnlen(realpath(relative_path, NULL), STRING_LEN) + 2, sizeof(char));
                    if(NULL == command){
                        return_value = print_error("FILE_NAVIGATOR: Calloc error", ERROR_COULDNT_ALLOCATE_MEMORY);
                        goto cleanup;
                    }
                    sprintf(command, "xdg-open '%s'", realpath(relative_path, NULL));
                }

                system(command);

                free(command);
                free(relative_path);
            }
            else{
                command = calloc(strlen("gedit") + strnlen(relative_path, STRING_LEN) + 2, sizeof(char));
                if(NULL == command){
                    return_value = print_error("FILE_NAVIGATOR: Calloc error", ERROR_COULDNT_ALLOCATE_MEMORY);
                    goto cleanup;
                }
                sprintf(command, "gedit '%s'", relative_path);
                system(command);

                free(command);
                free(relative_path);
            }
        }
    }

    edit_echo(true);

cleanup:
    if(NULL != file_array){
        for(i=0; i<num_of_files; i++){
            if(NULL != file_array[i].name){
                free(file_array[i].name);
            }
        }
        free(file_array);
    }

    if(NULL != dir_path){
        free(dir_path);
    }
    return return_value;
}
