#include "prizmio.h"
#include <stdio.h>
#include "prizmmath.h"

#define MAXPROCESSES 32
#define MAXCHARFORADD 124
#define SIZEOFINT 12
#define MIN_COMPUTE_UNITS 20

typedef struct Process {
    int a, d, C, T;
} Process;

void EDF();
int ask(int, Process*, char*, char*, char*, char*, char*);
int add(int, int, char*);
float TBS(int, int, Process*);
void TBS_Star(int, int, int, float, Process*, Process*);
void aux_problem(int, Process*);
int aux_problem_helper(int, int, char*, char*);
void RTA();
void clone(int, Process*, Process*);
void makeNumeral(int, char*);
void selectionSort(int, int, int, int, int, int, Process*);
void printSortMessage(int, int, int, int, int, Process*);
void swap(int*, int*);
int sumAperiodic(Process*, int, int);

int main() {
    int key, ans = 0;

    printStr("Choose\nan algorithm:\n1. EDF(*)/TBS(*)\n2. RTA\n3. Dover\n", TEXT_COLOR_BLACK);
    while (!ans) {
        ans = readInt();
        switch (ans) {
            case 1:
                EDF();
                break;
            case 2:
                RTA();
                break;
            case 3:
                //Dover();
                break;
            
            default:
                printStr("Not an option:\n1. EDF(*)/TBS(*)\n2. RTA\n3. Dover\n", TEXT_COLOR_BLACK);
                ans = 0;
                break;
        }
    }

    for (;;) {
        GetKey(&key);
    }

    return 0;
}

void EDF() {
    int i, k, anyAperiodic = 1, noTBS = 1, toExe, numOfProcesses = 0, numOfProcessesPer = 0, atLeastOneActive, computeUnits, d_ready[MAXPROCESSES];
    float Us = 0.0;
    Process p[MAXPROCESSES], pBkp[MAXPROCESSES];
    char lines[MAXCHAR*TOTLINES];

    printStr("Are there any\nperiodic\nprocesses?\n[0/1] 1 is yes\n", TEXT_COLOR_BLACK);
    if (readBool()) {
        printStr("Do you have\ndeadlines or\ndo I sync them\nwith T?\n[0/1] 1 is yes\n0 is sync\n", TEXT_COLOR_BLACK);
        if (readBool()) {
            numOfProcessesPer = ask(0, p, "How many periodic\nprocesses\nare there?\nstrictly positive\n< 32\n", "When does process\n%d\nbecome ready?\npositive\n", "What is process\n%d\ncompute time?\nstrictly positive\n", "When is process\n%d\ndeadline?\ngreater than\nits a + C\n", "What is process\n%d\nperiod?\nstrictly positive\ngreater than\nits a + d\n");
        } else {
            numOfProcessesPer = ask(0, p, "How many periodic\nprocesses\nare there?\nstrictly positive\n< 32\n", "When does process\n%d\nbecome ready?\npositive\n", "What is process\n%d\ncompute time?\nstrictly positive\n", "", "What is process\n%d\nperiod?\nstrictly positive\ngreater than\nits a + d\n");
        }

        clone(numOfProcessesPer, p, pBkp);
        
        printStr("Are there any\naperiodic\nprocesses?\n[0/1] 1 is yes\n", TEXT_COLOR_BLACK);
        anyAperiodic = readBool();
        if (anyAperiodic) {
            printStr("Do you have\ndeadlines or\ndo I make them\nwith TBS?\n[0/1] 1 is yes\n0 is TBS\n", TEXT_COLOR_BLACK);
            noTBS = readBool();
            if (noTBS) {
                numOfProcesses = ask(numOfProcessesPer, p, "How many aperiodic\nprocesses\nare there?\nstrictly positive\n< 32\n", "When does process\n%d\nbecome ready?\npositive\n", "What is process\n%d\ncompute time?\nstrictly positive\n", "When is process\n%d\ndeadline?\ngreater than\nits a + C\n", "");
            } else {
                numOfProcesses = ask(numOfProcessesPer, p, "How many aperiodic\nprocesses\nare there?\nstrictly positive\n< 32\n", "When does process\n%d\nbecome ready?\npositive\n", "What is process\n%d\ncompute time?\nstrictly positive\n", "", "");
            }

            for (i = 1; i <= numOfProcesses; i++) {
                sprintf(lines, "Aperiodic process\n%d -> %d\nremapped to\nprocess\nEXE > EXIT\nto proceed\n", i, i + numOfProcessesPer);
                printAndWait(lines, TEXT_COLOR_BLACK);
            }

            lines[0] = '\0';
            numOfProcesses += numOfProcessesPer;
            computeUnits = sumAperiodic(p, numOfProcessesPer, numOfProcesses);
            if (computeUnits < MIN_COMPUTE_UNITS) {
                computeUnits += MIN_COMPUTE_UNITS;
                anyAperiodic = 0;
            }

            if (!noTBS) {
                Us = TBS(numOfProcessesPer, numOfProcesses, p);
            }
        } else {
            numOfProcesses = numOfProcessesPer;
            computeUnits = MIN_COMPUTE_UNITS;
        }
    } else {
        numOfProcesses = ask(0, p, "How many aperiodic\nprocesses\nare there?\nstrictly positive\n< 32\n", "When does process\n%d\nbecome ready?\npositive\n", "What is process\n%d\ncompute time?\nstrictly positive\n", "When is process\n%d\ndeadline?\ngreater than\nits a + C\n", "");
        printStr("Are there any\nprecedences?\n[0/1] 1 is yes\n", TEXT_COLOR_BLACK);
        if (readBool()) {
            aux_problem(numOfProcesses, p);
        }
        computeUnits = sumAperiodic(p, numOfProcessesPer, numOfProcesses);
    }

    strcpy(lines, "The scheduling is:\n");
    for (i = 0; computeUnits; i++) {
        atLeastOneActive = 0;

        for (k = 0; k < numOfProcessesPer; k++) {
            if (i == p[k].T) {
                p[k].a = i;
                p[k].d = i + pBkp[k].d;
                p[k].C = pBkp[k].C;
                p[k].T = i + pBkp[k].T;
            }
        }

        for (k = 0; k < numOfProcesses; k++) {
            //sprintf(lines, "time: %d\nprocess: %d\na: %d\nC: %d\nd: %d\n", i, k + 1, p[k].a, p[k].C, p[k].d);
            //printAndWait(lines, TEXT_COLOR_BLACK);
            if (p[k].a <= i && p[k].C > 0) {
                if (Us && (k >= numOfProcessesPer) && (p[k].a == i)) {
                    TBS_Star(k, numOfProcessesPer, numOfProcesses, Us, p, pBkp);
                }
                d_ready[k] = p[k].d - i;
                atLeastOneActive = 1;
            } else {
                d_ready[k] = OVERFLOW;
            }
        }

        if (atLeastOneActive) {
            if (!noTBS) {
                toExe = iOfMin(d_ready, numOfProcesses, PRIVILEGE_LAST);
            } else {
                toExe = iOfMin(d_ready, numOfProcesses, PRIVILEGE_ASK);
            }
            if ((toExe >= numOfProcessesPer) || !anyAperiodic) {
                computeUnits--;
            }
            p[toExe].C--;
            toExe++;
        } else {
            toExe = 0;
        }

        if (!add(toExe, i, lines)) {
            break;
        }
    }

    strcat(lines, "\n");
    printStr(lines, TEXT_COLOR_BLACK);

    return;
}

int ask(int start, Process* p, char* question_1, char* question_2, char* question_3, char* question_4, char* question_5) {
    int j, numOfProcesses, aTmp = 0;
    char lines[MAXCHAR*TOTLINES];

    if (question_1[0] == '\0' || question_3[0] == '\0') {
        return 0;
    }

    printStr(question_1, TEXT_COLOR_BLACK);
    do {
        numOfProcesses = readInt();
    } while (numOfProcesses <= 0 || numOfProcesses > MAXPROCESSES);

    for (int i = start; i < (numOfProcesses + start); i++) {
        j = i + 1 - start;

        if (question_2[0] != '\0') {
            sprintf(lines, question_2, j);
            printStr(lines, TEXT_COLOR_BLACK);
            do {
                p[i].a = readInt();
            } while (p[i].a < 0);
            aTmp = p[i].a;
        }

        sprintf(lines, question_3, j);
        printStr(lines, TEXT_COLOR_BLACK);
        do {
            p[i].C = readInt();
        } while (p[i].C <= 0);

        if (question_4[0] != '\0') {
            sprintf(lines, question_4, j);
            printStr(lines, TEXT_COLOR_BLACK);

            do {
                p[i].d = readInt();
            } while (p[i].d < (aTmp + p[i].C));
        }

        if (question_5[0] != '\0') {
            sprintf(lines, question_5, j);
            printStr(lines, TEXT_COLOR_BLACK);
            do {
                p[i].T = readInt();
            } while ((p[i].T < (aTmp + p[i].C)) || (question_4[0] != '\0' && (p[i].T < (aTmp + p[i].d))));

            if (question_4[0] == '\0') {
                p[i].d = p[i].T;
            }
        } else if (question_4[0] == '\0') {
            p[i].d = 0;
        }
    }

    return numOfProcesses;
}

int add(int num, int i, char* lines) {
    int len, howManyLines = 1;
    char res[SIZEOFINT + 2];

    for (len = 0; lines[len] != '\0'; len++) {
        if (lines[len] == '\n') {
            howManyLines++;
        }
    }

    if (len < MAXCHARFORADD) {
        if ((len + 4 + howManyLines) >= (howManyLines * (MAXCHAR - 3))) {                          
            if (num < 10) {
                sprintf(res, " %d\n", num);
            } else {
                sprintf(res, "%d\n", num);
            }
        } else {
            if (num < 10) {
                sprintf(res, " %d|", num);
            } else {
                sprintf(res, "%d|", num);
            }
        }

        strcat(lines, res);
        return 1;
    } else if (len == MAXCHARFORADD) {
        if (num < 10) {
            sprintf(res, " %d", num);
        } else {
            sprintf(res, "%d", num);
        }

        strcat(lines, res);
        return 1;
    } else {
        lines[len - 1] = '.';
        lines[len - 2] = '.';
        lines[len - 3] = '.';
        return 0;
    }
}

float TBS(int numOfProcessesPer, int numOfProcesses, Process* p) {
    int i, UsNum, UsDen;
    float Us = 1.0;
    char lines[MAXCHAR*TOTLINES];

    selectionSort(numOfProcesses, 1, 1, 1, 0, 0, p);

    printStr("Do you have\nUs or do I\nput the max?\n[0/1]\n0 is auto\n", TEXT_COLOR_BLACK);
        if (readBool()) {
        printStr("What is Us?\nnumerator\nstrictly positive\n", TEXT_COLOR_BLACK);
        do {
            UsNum = readInt();
        } while (UsNum <= 0);
        printStr("What is Us?\ndenominator\ngreater than\nnumerator\n", TEXT_COLOR_BLACK);
        do {
            UsDen = readInt();
        } while (UsDen < UsNum);
        Us = ((float)UsNum) / ((float)UsDen);
    } else {
        for (i = 0; i < numOfProcessesPer; i++) {
            Us -= ((float)p[i].C) / ((float)p[i].T);
        }

        sprintf(lines, "Us = 1 -\nsum(C/T)\n= %d/1000\nEXE > EXIT\nto proceed\n", (int)(Us * 1000));
        printAndWait(lines, TEXT_COLOR_BLACK);
    }

    p[numOfProcessesPer].d = p[numOfProcessesPer].a + roundUp(((float)p[numOfProcessesPer].C) / Us);
    sprintf(lines, "d[%d] =\nmax{a[%d], 0}\n+ roundUP(C[%d]/Us)\n= %d\nEXE > EXIT\nto proceed\n", numOfProcessesPer + 1, numOfProcessesPer + 1, numOfProcessesPer + 1, p[numOfProcessesPer].d);
    printAndWait(lines, TEXT_COLOR_BLACK);
    for (i = (numOfProcessesPer + 1); i < numOfProcesses; i++) {
        p[i].d = max(p[i].a, p[i - 1].d) + roundUp(((float)p[i].C) / Us);
        sprintf(lines, "d[%d] =\nmax{a[%d], d[%d]}\n+ roundUP(C[%d]/Us)\n= %d\nEXE > EXIT\nto proceed\n", i + 1, i + 1, i, i + 1, p[i].d);
        printAndWait(lines, TEXT_COLOR_BLACK);
    }

    printStr("Do you want\nto run TBS*?\n[0/1] 1 is yes\n", TEXT_COLOR_BLACK);
    if (!readBool()) {
        Us = 0.0;
    }

    return Us;
}

void TBS_Star(int i, int numOfProcessesPer, int numOfProcesses, float Us, Process* p, Process* pBkp) {
    int k, If, Ia, next[MAXPROCESSES], dBkp;
    float IfDiv;
    char lines[MAXCHAR * TOTLINES];

    dBkp = p[i].d;
    for (k = 0; k < numOfProcessesPer; k++) {
        next[k] = roundUp(((float)(p[i].a + 1)) / ((float)(pBkp[k].T))) * pBkp[k].T;
        sprintf(lines, "next[%d][%d] =\nroundUP((a[%d] + 1)\n/T[%d])\n* T[%d] = %d\nEXE > EXIT\nto proceed\n", i + 1, k + 1, i + 1, k + 1, k + 1, next[k]);
        printAndWait(lines, TEXT_COLOR_BLACK);
    }

    while (1) {
        If = 0;
        Ia = 0;
        for (k = 0; k < numOfProcessesPer; k++) {
            IfDiv = ((float)(p[i].d - next[k])) / ((float)(pBkp[k].T));
            sprintf(lines, "Doing If[%d]:\npart of sum:\nmax{0, x-1}*%d\nx =\nroundUP((%d-%d)/%d)\nEXE > EXIT\nto proceed\n", i + 1, pBkp[k].C, p[i].d, next[k], pBkp[k].T);
            printAndWait(lines, TEXT_COLOR_BLACK);
            if (IfDiv > 0) {
                If += (roundUp(IfDiv) - 1) * pBkp[k].C;
            }
            if ((p[k].a <= p[i].a) && (p[k].d < p[i].d) && (p[k].C > 0)) {
                Ia += pBkp[k].C;
                sprintf(lines, "Doing Ia[%d]:\npart of sum:\nC[%d]=%d\nEXE > EXIT\nto proceed\n", i + 1, k + 1, pBkp[k].C);
                printAndWait(lines, TEXT_COLOR_BLACK);
            }
        }

        p[i].d = p[i].a + p[i].C + Ia + If;
        sprintf(lines, "If[%d] = %d\nIa[%d] = %d\nd[%d] = %d\nEXE > EXIT\nto proceed\n", i + 1, If, i + 1, Ia, i + 1, p[i].d);
        printAndWait(lines, TEXT_COLOR_BLACK);

        if ((p[i].d == dBkp) || (p[i].d == (p[i].a + p[i].C))) {
            break;
        }
        dBkp = p[i].d;
    }

    return;
}

void aux_problem(int numOfProcesses, Process* p) {
    int i, PID;
    char ans[MAXCHAR*TOTLINES];

    for (i = 0; i < numOfProcesses; i++) {
        PID = aux_problem_helper(i, numOfProcesses, "How many processes\nneed to be\ncompleted before\nprocess %d?\npositive\nless than total\n", "previous process\nof process %d?\nstrictly positive\nnot itself\nmust exist\n");
        if (PID >= 0) {
            p[i].a = max(p[i].a, p[PID].a + p[PID].C);
        }
    }

    for (int i = (numOfProcesses - 1); i >= 0; i--) {
        PID = aux_problem_helper(i, numOfProcesses, "How many processes\nwait for\nprocess %d\nto start?\npositive\nless than total\n", "following process\nof process %d?\nstrictly positive\nnot itself\nmust exist\n");
        if (PID >= 0) {
            p[i].d = min(p[i].d, p[PID].d - p[PID].C);
        }
    }

    for (i = 0; i < numOfProcesses; i++) {
        sprintf(ans, "Process %d:\na = %d\nd = %d\nC = %d\nEXE > EXIT\nto proceed\n", i + 1, p[i].a, p[i].d, p[i].C);
        printAndWait(ans, TEXT_COLOR_BLACK);
    }

    return;
}

int aux_problem_helper(int i, int numOfProcesses, char* question_1, char* question_2) {
    int howMany, PID = -1;
    char question[MAXCHAR*TOTLINES], question_3[MAXCHAR*TOTLINES], numeral[MAXCHAR];

    sprintf(question, question_1, i + 1);
    printStr(question, TEXT_COLOR_BLACK);
    do {
        howMany = readInt();
    } while (howMany < 0 || howMany >= numOfProcesses);

    for (int k = 1; k <= howMany; k++) {
        makeNumeral(k, numeral);
        question[0] = '\0';
        strcpy(question, numeral);
        sprintf(question_3, question_2, i + 1);
        strcat(question, question_3);
        printStr(question, TEXT_COLOR_BLACK);

        do {
            PID = readInt();
            PID--;
        } while (PID < 0 || PID == i || PID >= numOfProcesses);
    }

    return PID;
}

void RTA() {
    int k, numOfProcesses, Rbkp, interference, Ibkp, toAdd, R[MAXPROCESSES];
    Process p[MAXPROCESSES];
    char lines[MAXCHAR*TOTLINES], lines_expl[MAXCHAR*TOTLINES];

    numOfProcesses = ask(0, p, "How many periodic\nprocesses\nare there?\nstrictly positive\n< 32\n", "", "What is process\n%d\ncompute time?\nstrictly positive\n", "When is process\n%d\ndeadline?\ngreater than\nits a + C\n", "What is process\n%d\nperiod?\nstrictly positive\ngreater than\nits a + d\n");
    
    selectionSort(numOfProcesses, 0, 1, 1, 1, 1, p);

    strcpy(lines, "Each process R is:\n");
    for (int i = 0; i < numOfProcesses; i++) {
        R[i] = p[i].C;

        for (k = 0; 1; k++) {
            Rbkp = R[i];

            sprintf(lines_expl, "R[%d](%d) = C[%d]\n+ I[%d](%d) = %d\nEXE > EXIT\nto proceed\n", i + 1, k, i + 1, i + 1, k, R[i]);
            printAndWait(lines_expl, TEXT_COLOR_BLACK);

            interference = 0;
            for (int j = 1; j <= i; j++) {
                Ibkp = interference;
                toAdd = roundUp(((float)Rbkp) / ((float)p[j - 1].T)) * p[j - 1].C;
                interference += toAdd;
                sprintf(lines_expl, "I[%d](%d){%d} =\nI[%d](%d){%d} +\nroundUP(%d/%d)*%d\n= %d + %d = %d\nEXE > EXIT\nto proceed\n", i + 1, k, j, i + 1, k, j - 1, Rbkp, p[j - 1].T, p[j - 1].C, Ibkp, toAdd, interference);
                printAndWait(lines_expl, TEXT_COLOR_BLACK);
            }
            R[i] = p[i].C + interference;

            if (R[i] == Rbkp) {
                add(Rbkp, i, lines);
                break;
            }
        }

        if (i > 0) {
            sprintf(lines_expl, "R[%d](%d) = C[%d]\n+ I[%d](%d) = %d\nEXE > EXIT\nto proceed\n", i + 1, k + 1, i + 1, i + 1, k + 1, R[i]);
            printAndWait(lines_expl, TEXT_COLOR_BLACK);
        }
    }

    strcat(lines, "\n");
    printStr(lines, TEXT_COLOR_BLACK);

    return;
}

void clone(int dim, Process* source, Process* dest) {
    for (int i = 0; i < dim; i++) {
        dest[i].a = source[i].a;
        dest[i].d = source[i].d;
        dest[i].C = source[i].C;
        dest[i].T = source[i].T;

        source[i].d += source[i].a;
        source[i].T += source[i].a;
    }

    return;
}

void makeNumeral(int num, char* ans) {

    if (num == 1) {
        sprintf(ans, "%dst\n", num);
    } else if (num == 2) {
        sprintf(ans, "%dnd\n", num);
    } else if (num == 3) {
        sprintf(ans, "%drd\n", num);
    } else {
        sprintf(ans, "%dth\n", num);
    }

    return;
}
 
void selectionSort(int dim, int a, int d, int C, int T, int source, Process* p) {
    int i, min_idx, bool = 0, source_arr[MAXPROCESSES];
 
    if (!source) {
        for (i = 0; i < dim; i++) {
            source_arr[i] = p[i].a;
        }
    } else if (source == 1) {
        for (i = 0; i < dim; i++) {
            source_arr[i] = p[i].d;
        }
    } else if (source == 2) {
        for (i = 0; i < dim; i++) {
            source_arr[i] = p[i].C;
        }
    } else {
        for (i = 0; i < dim; i++) {
            source_arr[i] = p[i].T;
        }
    }

    for (i = 0; i < (dim - 1); i++) {
        min_idx = i;
        for (int j = (i + 1); j < dim; j++) {
            if (source_arr[j] < source_arr[min_idx]) {
                min_idx = j;
                bool = 1;
            }
        }
        swap(&source_arr[min_idx], &source_arr[i]);

        if (a) {
            swap(&p[min_idx].a, &p[i].a);
        }
        if (d) {
            swap(&p[min_idx].d, &p[i].d);
        }
        if (C) {
            swap(&p[min_idx].C, &p[i].C);
        }
        if (T) {
            swap(&p[min_idx].T, &p[i].T);
        }
    }

    if (bool) {
        printSortMessage(dim, a, d, C, T, p);
    }

    return;
}

void printSortMessage(int numOfProcesses, int a, int d, int C, int T, Process* p) {
    char lines[MAXCHAR * TOTLINES], line[MAXCHAR];

    printStr("I have\nrearranged the\nprocesses\naccording to the\nalghorithm\nEXE > EXIT to\nshow new order\n", TEXT_COLOR_BLACK);
    waitUser();
    for (int i = 0; i < numOfProcesses; i++) {
        sprintf(line, "Process %d:\n", i + 1);
        strcpy(lines, line);
        if (a) {
            sprintf(line, "a = %d\n", p[i].a);
            strcat(lines, line);
        }
        if (d) {
            sprintf(line, "d = %d\n", p[i].d);
            strcat(lines, line);
        }
        if (C) {
            sprintf(line, "C = %d\n", p[i].C);
            strcat(lines, line);
        }
        if (T) {
            sprintf(line, "T = %d\n", p[i].T);
            strcat(lines, line);
        }
        strcat(lines, "EXE > EXIT\nto proceed\n");
        printAndWait(lines, TEXT_COLOR_BLACK);
        lines[0] = '\0';
    }

    return;
}

void swap(int* xp, int* yp) {
    int temp = *xp;

    *xp = *yp;
    *yp = temp;

    return;
}

int sumAperiodic(Process* p, int start, int end) {
    int res = 0;

    for (int i = start; i < end; i++) {
        res += p[i].C;
    }

    return res;
}
