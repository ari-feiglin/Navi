#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define STRING_LEN (256)

#define BUFFER_SIZE (1024)

#define true (1)
#define false (0)

typedef char bool;

typedef enum error_code_e{
    ERROR_UNINITIALIZED = -1,
    ERROR_SUCCESS = 0,

    ERROR_COULDNT_ALLOCATE_MEMORY,

    ERROR_COULDNT_OPEN,
    ERROR_COULDNT_READ,

    ERROR_UNDEFINED
}error_code_t;

static inline int print_error(char * prompt, int return_value){
    perror(prompt);
    printf("(Errno: %i)\n", errno);

    return return_value;
}

static inline void edit_echo(bool on){
    struct termios attr = {0};

    tcgetattr(STDIN_FILENO, &attr);

    if(on){
        attr.c_lflag |= (ECHO | ICANON);
    }
    else{
        attr.c_lflag &= ~(ECHO | ICANON);
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &attr);
}