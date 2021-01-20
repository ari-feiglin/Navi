#include "navi.h"

int main(int argc, char ** argv){
    if(argc < 2){
        file_navigator(".");
    }
    else{
        file_navigator(argv[1]);
    }

    edit_echo(true);
}
