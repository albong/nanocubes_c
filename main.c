#include "nanocube.h"
#include <stdio.h>

int main(int argc, const char *argv[]){
    printf("Start\n");
    Nanocube *nc = newNanocube(1, 1);
    printf("Made a nanocube!\n");
    addToNanocube(nc, 1, 5, 1, 2);
    printf("Added to the nanocube!\n");
    printNanocube(nc);
}