#include <stdio.h>


int main(int argc, char *argv[]){
    char DELIMITER = ',';
    char buffer[1024];
    int i;
    while(1){
        if (fgets(buffer, 1024, stdin) == NULL){
            printf("WE DED\n");
            return 1;
        }
        for (i = 0; i < 1024; i++){
            if (buffer[i] == '\n'){
                buffer[i] = '\0';
            }
        }
        puts(buffer);
    }

    return 0;
}
