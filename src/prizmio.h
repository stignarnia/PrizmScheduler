#ifndef _PRIZMIO_H
#ifndef __FXCG_DISPLAY_H
#include <fxcg/display.h>
#endif
#ifndef __KEYBOARD_H__
#include <fxcg/keyboard.h>
#endif
#ifndef _STDLIB_H
#include <stdlib.h>
#endif
#ifndef _STRING_H
#include <string.h>
#endif
#ifndef _CTYPE_H
#include <ctype.h>
#endif

#define _PRIZMIO_H
#define TOTLINES 7
#define MAXCHAR 22
#define OVERFLOW 2147483647
#define MINUS -103

int printStr(char*, int);
void printAndWait(char*, int);
int readInt();
int readBool();
void readBuf(int, int, char*);
void waitUser();

int printStr(char* text, int color) {
    int i, j = 0, k = 2, len;
    char lines[TOTLINES][MAXCHAR];

    Bdisp_AllClr_VRAM();
    len = strlen(text);

    if (text[len - 1] != '\n') {
        printStr("PRINT ERR: 0x01\nNO TERMINAL CHAR\n", TEXT_COLOR_RED);
        return 0;
    }

    for (i = 0; i < TOTLINES; i++) {
        strcpy(lines[i], "  ");
    }

    for (i = 0; i < len; i++) {
        if (text[i] == '\n') {
            lines[j][k] = '\0';
            j++;
            if (j > TOTLINES) {
                printStr("PRINT ERR: 0x02\nTOO MANY LINES\n", TEXT_COLOR_RED);
                return 0;
            }
            k = 2;
        } else {
            lines[j][k] = text[i];
            k++;
            if (k >= MAXCHAR) {
                printStr("PRINT ERR: 0x03\nLINE TOO LONG\n", TEXT_COLOR_RED);
                return 0;
            }
        }
    }

    for (i = j; i < TOTLINES; i++) {
        lines[i][2] = '\0';
    }

    for (i = 0; i < TOTLINES; i++) {
        PrintXY(1, i + 1, lines[i], TEXT_MODE_TRANSPARENT_BACKGROUND, color);
    }

    return 1;
}

void printAndWait(char* text, int color) {
    printStr(text, color);
    waitUser();

    return;
}

int readInt() {
    char buf[MAXCHAR];
    int ans, isnumber, ovf, len, i, neg = 0;

    do {
        ovf = 0;
        do {
            readBuf(1, 8, buf);
            isnumber = 1;
            len = strlen(buf);

            if (!len) {
                isnumber = 0;
                printStr("INPUT ERR: 0x04\nEMPTY INPUT\nRETRY!\n", TEXT_COLOR_RED);
            }

            for (i = 0; i < len; i++) {
                if (!isdigit(buf[i])) {
                    if (i == 0 && buf[i] == MINUS && len > 1) {
                        neg = 1;
                    } else {
                        isnumber = 0;
                        printStr("INPUT ERR: 0x05\nNOT AN INTEGER\nRETRY!\n", TEXT_COLOR_RED);
                    }
                }
            }
        } while (!isnumber);

        if (neg) {
            for (i = 0; i < len; i++) {
                buf[i] = buf[i + 1];
            }
            ans = -atoi(buf);
        } else {
            ans = atoi(buf);
        }

        if (ans == OVERFLOW || ans == -OVERFLOW) {
            ovf = 1;
            printStr("INPUT ERR: 0x06\nOVERFLOW\nMAX 2.147.483.646\nRETRY!\n", TEXT_COLOR_RED);
        }
    } while (ovf);

    return ans;
}

int readBool() {
    int bool;

    do {
        bool = readInt();
    } while (bool != 0 && bool != 1);

    return bool;
}

void readBuf(int x, int y, char* buf) {
    int start = 0, cursor = 0, i = 0, key;
    buf[0] = '\0';

    DisplayMBString((unsigned char*)buf, start, cursor, x, y);

    while (1) {
        GetKey(&key);
        if (key == KEY_CTRL_EXE) {
            break;
        } else if (i == (MAXCHAR - 3)) {
            printStr("INPUT WARN: 0x07\nCHAR LIMIT REACHED\nEXE TO INPUT AS IS\nKEEP WRITING TO\nSTART OVER\n", TEXT_COLOR_YELLOW);
            GetKey(&key);
            if (key == KEY_CTRL_EXE) {
                break;
            } else {
                start = 0;
                cursor = 0;
                i = 0;
                buf[0] = '\0';
            }
        }

        if (key && key < 30000) {
            cursor = EditMBStringChar((unsigned char*)buf, MAXCHAR, cursor, key);
            DisplayMBString((unsigned char*)buf, start, cursor, x,y);
            i++;
        } else {
            EditMBStringCtrl((unsigned char*)buf, MAXCHAR, &start, &cursor, &key, x, y);
        }
    }

    return;
}

void waitUser() {
    int key;

    do {
        GetKey(&key);
    } while (key != KEY_CTRL_EXE);
    do {
        GetKey(&key);
    } while (key != KEY_CTRL_EXIT);

    return;
}
#endif
