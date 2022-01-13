#include <fxcg/display.h>
#include <fxcg/keyboard.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TOTLINES 6
#define MAXCHAR 22
#define OVERFLOW 2147483647
#define MINUS -103

void init(int*, int*, int*, int*);
void recurringPrint(int, int, int);
int backOff(int);
void waitUser();
int printStr(char*, int);
int readInt();
void readBuf(int, int, char*);
int roundNum(float);
int power(int, int);

int main() {
    int ltot, t0, sstresh, rcvwnd, sndwnd, cwndInt, boolOut, bOff, firstNoSS = 1, i;
    float cwnd;
    char final[MAXCHAR*TOTLINES];

    Bdisp_EnableColor(0);
    init(&ltot, &t0, &sstresh, &cwndInt);

    for (i = 0; ltot > 0; i++) {
        printStr("RCVWND [MSS]?\n-RCVND IF\nNETWORK DOWN\n", TEXT_COLOR_BLACK);
        rcvwnd = readInt();

        boolOut = 0;
        if (rcvwnd < 0) {
            boolOut = 1;
            rcvwnd = -rcvwnd;
        }

        if (cwndInt < rcvwnd) {
            sndwnd = cwndInt;
        } else {
            sndwnd = rcvwnd;
        }

        if (sndwnd > ltot) {
            sndwnd = ltot;
        }

        if (!boolOut) {
            ltot -= sndwnd;
            recurringPrint(sndwnd, sstresh, cwndInt);
            if (cwndInt < sstresh) {
                cwndInt += sndwnd;
                if (cwndInt > sstresh) {
                    cwndInt = sstresh;
                }
            } else {
                if (firstNoSS) {
                    cwnd = (float)cwndInt;
                    firstNoSS = 0;
                }
                cwnd += ((float)sndwnd / (float)cwndInt);
                if ((int)cwnd >= (cwndInt + 1)) {
                    cwndInt = (int)cwnd;
                }
            }
        } else {
            recurringPrint(-sndwnd, sstresh, cwndInt);
            bOff = backOff(t0);
            i += bOff;
            cwndInt = 1;
            firstNoSS = 1;
            if (bOff > t0) {
                sstresh = 2;
            } else {
                sstresh = roundNum((float)sndwnd / 2.0);
                if (sstresh < 2) {
                    sstresh = 2;
                }
            }
        }
    }

    sprintf(final, "%d\nIS TOTAL TIME [RTT]\nMENU > ALPHA > 7\n> MENU TO CLOSE\nSO YOU CAN RESTART\n", (i - 1));
    printStr(final, TEXT_COLOR_BLACK);

    while (1) {
        waitUser();
    }

    return 0;
}

void init(int* ltot, int* t0, int* sstresh, int* cwndInt) {

    do {
        printStr("LTOT [MSS]?\nstrictly positive\n", TEXT_COLOR_BLACK);
        *ltot = readInt();
    } while (*ltot <= 0);

    do {
        printStr("BASE TIMEOUT\nVALUE [RTT]?\npositive\n", TEXT_COLOR_BLACK);
        *t0 = readInt();
    } while (*t0 < 0);

    do {
        printStr("SSTRESH AT\nt=0 [MSS]?\npositive\n", TEXT_COLOR_BLACK);
        *sstresh = readInt();
    } while (*sstresh < 0);

    do {
        printStr("CWND AT\nt=0 [MSS]?\nstrictly positive\n", TEXT_COLOR_BLACK);
        *cwndInt = readInt();
    } while (*cwndInt <= 0);

    return;
}

void recurringPrint(int sndwnd, int sstresh, int cwndInt) {
    char answer[MAXCHAR*TOTLINES];

    sprintf(answer, "%d IS SNDWND\n%d IS SSTRESH\n%d IS CWND\nall values in MSS\n- IS NO ACK\nEXE > EXIT PROCEED\n", sndwnd, sstresh, cwndInt);
    printStr(answer, TEXT_COLOR_BLACK);
    waitUser();

    return;
}

int backOff(int t0) {
    int boolOut = 1, t = 0;
    char question[MAXCHAR*TOTLINES];

    for (int i = 1; boolOut; i++) {
        t += power(t0, i);
        sprintf(question, "%d\nIS IT STILL DOWN\nAFTER THIS MANY\nRTTs?\n0 OR 1\n1 IS DOWN\n", t);

        do {
            printStr(question, TEXT_COLOR_BLACK);
            boolOut = readInt();
        } while (boolOut != 0 && boolOut != 1);
    }

    sprintf(question, "%d\nPLEASE MOVE\nOF THIS MANY\nRTTs\nIN THE GRAPH\nEXE > EXIT PROCEED\n", t);
    printStr(question, TEXT_COLOR_BLACK);
    waitUser();
    t--;

    return t;
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
        sprintf(lines[i], "  ");
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

int readInt() {
    char buf[MAXCHAR];
    int ans, isnumber, ovf, len, i, neg = 0;

    do {
        ovf = 0;
        do {
            readBuf(1, 7, buf);
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

int roundNum(float num) {
    return num < 0 ? num - 0.5 : num + 0.5;
}

int power(int num, int exp) {
    int res = 1;

    if (exp == 0) {
        return res;
    } else if (exp < 0) {
        printStr("POW ERR: 0x08\nNEGATIVE EXP\nRESULT IS -1\nEXE > EXIT TO ACK\n", TEXT_COLOR_RED);
        waitUser();
        return -1;
    }

    res = num * power(num, exp - 1);

    return res;
}
