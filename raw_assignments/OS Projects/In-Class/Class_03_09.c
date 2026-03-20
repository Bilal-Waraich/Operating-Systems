#include <stdio.h>
#include <stdlib.h>

void print_sequence(int first, int incr, int last) {
    int current = first;
    if (incr > 0) {
        while (current <= last) {
            printf("%d\n", current);
            current += incr;
        }
    } else if (incr < 0) {
        while (current >= last) {
            printf("%d\n", current);
            current += incr;
        }
    }
}
