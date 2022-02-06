#ifndef _PRIZMMATH_H
#ifndef _PRIZMIO_H
#include "prizmio.h"
#endif

#define _PRIZMMATH_H
#define PRIVILEGE_FIRST 0
#define PRIVILEGE_LAST 1
#define PRIVILEGE_ASK 2

int roundNum(float);
int roundUp(float);
int power(int, int);
int max(int, int);
int min(int, int);
int iOfMin(int*, int, int);
int arraySum(int*, int);

int roundNum(float num) {
    return num < 0 ? num - 0.5 : num + 0.5;
}

int roundUp(float num) {
    int inum = (int)num;

    if (num == (float)inum) {
        return inum;
    } else if (num < 0) {
        printStr("ROUNDUP ERR: 0x08\nNEGATIVE NUMBER\nCURRENTLY\nUNSUPPORTED\nEXE > EXIT TO ACK\n", TEXT_COLOR_RED);
        waitUser();
        return -1;
    }

    return inum + 1;
}

int power(int num, int exp) {
    int res = 1;

    if (exp == 0) {
        return res;
    } else if (exp < 0) {
        printStr("POW ERR: 0x09\nNEGATIVE EXP\nRESULT IS -1\nEXE > EXIT TO ACK\n", TEXT_COLOR_RED);
        waitUser();
        return -1;
    }

    res = num * power(num, exp - 1);

    return res;
}

int max(int x, int y) {
    if (y > x) {
        return y;
    } else {
        return x;
    }
}

int min(int x, int y) {
    if (y < x) {
        return y;
    } else {
        return x;
    }
}

int iOfMin(int* arr, int dim, int criteria) {
    int res = 0;

    for (int i = 0; i < dim; i++) {
        if (arr[i] < arr[res]) {
            res = i;
        } else if ((arr[i] == arr[res]) && (i != res)) {
            if (criteria == 1) {
                res = i;
            } else if (criteria == 2) {
                printStr("Two elements\nin different\npositions are\nthe same value\nDo you want to\nselect the latest?\n[0/1] 1 is yes\n", TEXT_COLOR_BLACK);
                if (readBool()) {
                    res = i;
                }
            }
        }
    }

    return res;
}

int arraySum(int* arr, int dim) {
    int res = 0;

    for (int i = 0; i < dim; i++) {
        res += arr[i];
    }

    return res;
}
#endif
