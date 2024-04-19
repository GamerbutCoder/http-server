#include <stdio.h>
#include <bits/types.h>
#include <stdlib.h>
typedef struct test {
    char *c;
} test;

test* f(){
    char *body = (char *)malloc(sizeof(char) * 100);
    printf("body: %p\n", body);
    test *t = (test  *)malloc(sizeof(test));
    t->c = "abcd";
    printf("f ch: %p  %p\n", t, t->c);
    free(body);
    return t;
}
void main(){

    for(int i =0 ;i<2;i++) {
        test *ch1 = f();
    printf("main ch1: %p     %p\n", ch1, ch1->c);
    free(ch1);
    }

    test *ch2 = f();
    printf("main ch2: %p     %p\n", ch2, ch2->c);
    free(ch2);


   
}