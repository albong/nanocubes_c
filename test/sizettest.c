#include <stdio.h>
#include <stddef.h>

int main(){
    size_t test = 0;
    printf("%zu\n", test);
    test = -1;
    printf("%zu\n", test);


    printf("Size of pointer %d\n", sizeof(void *));
    printf("Size of size_t %d\n", sizeof(size_t));


    return 0;
}
