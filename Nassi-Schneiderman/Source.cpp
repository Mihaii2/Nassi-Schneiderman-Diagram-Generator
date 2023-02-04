#include <iostream>
#include "graphics.h"
#include "winbgim.h"
#include <Windows.h>
#include <cstring>

/*Bibliotecile-folosite*/

#pragma comment(lib,"graphics.lib")   //O directiva exclusiva compiler-ului Visual c++ pentru a da link la o biblioteca din compiler

using namespace std;

FILE* reg = fopen("Reguli_De_Sintaxa.txt", "r");
void start();
void afisare();
char Path[300], img[100];
int maxH = getmaxheight() / 1.15, maxW = getmaxwidth() / 1.15;
int diagRowHeight = 0.055 * maxH, diagWidth = 0.8 * maxW, rowsInTotal = 0;
char randuri[200][200];

/*Variabile-globale-folosite*/

void copiereRanduri()
{
    int cnt = 0;
    FILE* fptr = fopen(Path, "r");
    while (fgets(randuri[cnt], 199, fptr))
    {
        cnt++;
    }
    rowsInTotal = cnt;
}

/*Numararea-randurilor-din-fisier*/

int howManySpaces(char rand[])
{
    int spaces = 0;
    while (rand[spaces] == 32)
        spaces++;
    return spaces;
}

/*Numararea-spatiilor*/

int tipOperatie(char rand[]) {
    int spaces = howManySpaces(rand);
    char cuvInceput[10]{ 0 };
    strncpy(cuvInceput, rand + spaces, 3);
    cuvInceput[3] = 0;
    if (strcmp(cuvInceput, "if(") == 0) {
        return 1;
    }
    strncpy(cuvInceput, rand + spaces, 3);
    cuvInceput[3] = 0;
    if (strcmp(cuvInceput, "for") == 0) {
        return 2;
    }
    strncpy(cuvInceput, rand + spaces, 6);
    cuvInceput[6] = 0;
    if (strcmp(cuvInceput, "while(") == 0) {
        return 3;
    }
    strncpy(cuvInceput, rand + spaces, 5);
    cuvInceput[5] = 0;
    if (strcmp(cuvInceput, "read(") == 0) {
        return 4;
    }
    strncpy(cuvInceput, rand + spaces, 6);
    cuvInceput[6] = 0;
    if (strcmp(cuvInceput, "print(") == 0) {
        return 5;
    }
    strncpy(cuvInceput, rand + spaces, 7);
    cuvInceput[7] = 0;
    if (strcmp(cuvInceput, "return(") == 0) {
        return 6;
    }
    strncpy(cuvInceput, rand + spaces, 2);
    cuvInceput[2] = 0;
    if (strstr(rand + spaces, "<-") != NULL) {
        return 7;
    }
    strncpy(cuvInceput, rand + spaces, 4);
    cuvInceput[4] = 0;
    if (strcmp(cuvInceput, "else") == 0) {
        return 8;
    }
    return 0;
}

/*Determinarea-tipului-de-operatie*/

void printRow(int Left, int Right, int currLine, int rowNumber, int ypoz, int xpoz)
{
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    int spaces = howManySpaces(randuri[rowNumber]);
    outtextxy((Right + Left) / 2 + xpoz, diagRowHeight * currLine + 0.7 * diagRowHeight + ypoz, randuri[rowNumber] + spaces);
}

/*Afisarea-randurilor*/

void skipElseOrIf(int& row, int& linesInBrackets, int& linesToDraw)
{
    int rowAux = row - 1;
    row++, linesInBrackets++;
    int bracketsStack = 0;
    int linesCntElse = 0;
    int linesCntIf = 0;
    do {        // find lines to draw in else
        int tip = tipOperatie(randuri[row]);
        if (tip >= 2 && tip <= 7) {
            linesCntElse++;
        }
        else if (tip == 1) {
            linesCntElse += 2;
        }
        else if (tip == 8) {
            skipElseOrIf(row, linesInBrackets, linesCntElse);
            tip = tipOperatie(randuri[row]);
            if (tip >= 2 && tip <= 7)
            {
                linesCntElse++;
            }
            else if (tip == 1)
            {
                linesCntElse += 2;
            }
        }
        if (strstr(randuri[row], "{") != 0)
            bracketsStack++;
        else if (strstr(randuri[row], "}") != 0)
            bracketsStack--;
        row++, linesInBrackets++;
    } while (bracketsStack > 0);
    bracketsStack = 0;// find if starting line
    int ifPosition = 0;
    do {
        if (strstr(randuri[rowAux], "{") != 0)
            bracketsStack++;
        else if (strstr(randuri[rowAux], "}") != 0)
            bracketsStack--;
        rowAux--;
    } while (bracketsStack < 0);
    ifPosition = rowAux;
    rowAux++, bracketsStack = 0;   // find number of lines to draw in if
    int notUsed = 0;
    do {
        int tip = tipOperatie(randuri[rowAux]);
        if (tip >= 2 && tip <= 7) {
            linesCntIf++;
        }
        else if (tip == 1) {
            linesCntIf += 2;
        }
        else if (tip == 8) {
            skipElseOrIf(rowAux, notUsed, linesCntIf);
            tip = tipOperatie(randuri[rowAux]);
            if (tip >= 2 && tip <= 7)
            {
                linesCntIf++;
            }
            else if (tip == 1)
            {
                linesCntIf += 2;
            }
        }
        if (strstr(randuri[rowAux], "{") != 0)
            bracketsStack++;
        else if (strstr(randuri[rowAux], "}") != 0)
            bracketsStack--;
        rowAux++;
    } while (bracketsStack > 0);
    if (linesCntElse > linesCntIf) //if lines in else > lines in if, consider else number of lines to draw
    {
        linesToDraw = linesToDraw + linesCntElse - linesCntIf;
    }
}

/*Determinarea-saltului-peste-instructiunile-din-if-sau-else*/

void lastBracket(int row, int& linesInBrackets, int& linesToDraw)
{
    row++;
    int bracketsStack = 0;
    do {
        int tip = tipOperatie(randuri[row]);
        if (tip >= 2 && tip <= 7)
        {
            linesToDraw++;
        }
        else if (tip == 1)
        {
            linesToDraw += 2;
        }
        else if (tip == 8)
        {
            skipElseOrIf(row, linesInBrackets, linesToDraw);
            tip = tipOperatie(randuri[row]);
            if (tip >= 2 && tip <= 7)
            {
                linesToDraw++;
            }
            else if (tip == 1)
            {
                linesToDraw += 2;
            }
        }
        if (strstr(randuri[row], "{") != 0)
            bracketsStack++;
        else if (strstr(randuri[row], "}") != 0)
            bracketsStack--;
        row++, linesInBrackets++;
    } while (bracketsStack > 0);
}

/*Gasirea-ultimei-acolade*/

void ifAndElseBracket(int row, int& linesToDrawFB, int& linesInIfBrackets, int& linesToDrawSecondB, int& linesInElseBrackets, bool& existaElse)
{
    row++;
    int bracketsStack = 0;
    do {
        int tip = tipOperatie(randuri[row]);
        if (tip >= 2 && tip <= 7)
        {
            linesToDrawFB++;
        }
        else if (tip == 1)
        {
            linesToDrawFB += 2;
        }
        else if (tip == 8) {
            skipElseOrIf(row, linesInIfBrackets, linesToDrawFB);
            tip = tipOperatie(randuri[row]);
            if (tip >= 2 && tip <= 7)
            {
                linesToDrawFB++;
            }
            else if (tip == 1)
            {
                linesToDrawFB += 2;
            }
        }
        if (strstr(randuri[row], "{") != 0)
            bracketsStack++;
        else if (strstr(randuri[row], "}") != 0)
            bracketsStack--;
        row++, linesInIfBrackets++;
    } while (bracketsStack > 0);
    if (tipOperatie(randuri[row]) == 8)
        existaElse = TRUE;
    if (existaElse) {
        row++;
        bracketsStack = 0;
        do {
            int tip = tipOperatie(randuri[row]);
            if (tip >= 2 && tip <= 7)
            {
                linesToDrawSecondB++;
            }
            else if (tip == 1)
            {
                linesToDrawSecondB += 2;
            }
            else if (tip == 8)
            {
                skipElseOrIf(row, linesInElseBrackets, linesToDrawSecondB);
                tip = tipOperatie(randuri[row]);
                if (tip >= 2 && tip <= 7) {
                    linesToDrawSecondB++;
                }
                else if (tip == 1) {
                    linesToDrawSecondB += 2;
                }
            }
            if (strstr(randuri[row], "{") != 0)
                bracketsStack++;
            else if (strstr(randuri[row], "}") != 0)
                bracketsStack--;
            row++, linesInElseBrackets++;
        } while (bracketsStack > 0);
    }
}

/*Pozitionarea-intructiunilor-pe-baza-numarului-de-intructiuni-din-if-si-else*/

void drawInstructions(int currLeft, int currRight, int& row, int& currLine, int& xpoz, int& ypoz, int rowLimFromPrev)
{
    int rowLimit = 9999, rowLimitIf = 9999, rowLimitElse = 9999;
    for (; row < rowsInTotal; row++)
    {
        int tip = tipOperatie(randuri[row]);
        if (tip >= 4 && tip <= 7)
        {
            rectangle(currLeft + xpoz, diagRowHeight * currLine + ypoz, currRight + xpoz, diagRowHeight * (currLine + 1) + ypoz);
            // the next if is for hover effect
            int xMouse = mousex(), yMouse = mousey();
            if ((xMouse >= currLeft + xpoz + 2) && (xMouse <= currRight + xpoz - 1) && (yMouse >= diagRowHeight * currLine + ypoz + 2 + 0.123 * maxH)
                && (yMouse <= diagRowHeight * (currLine + 1) + ypoz - 1 + 0.123 * maxH)) {
                setfillstyle(SOLID_FILL, COLOR(124, 171, 230));
                bar(currLeft + xpoz + 2, diagRowHeight * currLine + ypoz + 2, currRight + xpoz - 1, diagRowHeight * (currLine + 1) + ypoz - 1);
                setbkcolor(COLOR(124, 171, 230));
                printRow(currLeft, currRight, currLine, row, ypoz, xpoz);
            }
            else {
                setfillstyle(SOLID_FILL, COLOR(166, 252, 255));
                bar(currLeft + xpoz + 2, diagRowHeight * currLine + ypoz + 2, currRight + xpoz - 1, diagRowHeight * (currLine + 1) + ypoz - 1);
                setbkcolor(COLOR(166, 252, 255));
                printRow(currLeft, currRight, currLine, row, ypoz, xpoz);
            }
            currLine++;
        }
        else if (tip == 2 || tip == 3)
        {
            int linesInBrackets = 0;  //lines from current row to bracket (the closing bracket)
            int linesToDraw = 0;
            lastBracket(row, linesInBrackets, linesToDraw);  //lines to draw from first bracket to last
            rowLimit = row + linesInBrackets;

            //draw
            if (tip == 2) {
                rectangle(currLeft + xpoz, diagRowHeight * currLine + ypoz, currRight + xpoz, diagRowHeight * (currLine + linesToDraw + 1) + ypoz);
                int xMouse = mousex(), yMouse = mousey();
                if ((xMouse >= currLeft + xpoz + 2) && (xMouse <= currRight + xpoz - 1) && (yMouse >= diagRowHeight * currLine + ypoz + 2 + 0.123 * maxH)
                    && (yMouse <= diagRowHeight * (currLine + linesToDraw + 1) + ypoz - 1 + 0.123 * maxH)) {
                    setfillstyle(SOLID_FILL, COLOR(255, 213, 0));
                    bar(currLeft + xpoz + 2, diagRowHeight * currLine + ypoz + 2, currRight + xpoz - 1, diagRowHeight * (currLine + linesToDraw + 1) + ypoz - 1);
                    setbkcolor(COLOR(255, 213, 0));
                    printRow(currLeft, currRight, currLine, row, ypoz, xpoz);
                }
                else {
                    setfillstyle(SOLID_FILL, COLOR(248, 255, 117));
                    bar(currLeft + xpoz + 2, diagRowHeight * currLine + ypoz + 2, currRight + xpoz - 1, diagRowHeight * (currLine + linesToDraw + 1) + ypoz - 1);
                    setbkcolor(COLOR(248, 255, 117));
                    printRow(currLeft, currRight, currLine, row, ypoz, xpoz);
                }
            }
            else {
                rectangle(currLeft + xpoz, diagRowHeight * currLine + ypoz, currRight + xpoz, diagRowHeight * (currLine + linesToDraw + 1) + ypoz);
                int xMouse = mousex(), yMouse = mousey();
                if ((xMouse >= currLeft + xpoz + 2) && (xMouse <= currRight + xpoz - 1) && (yMouse >= diagRowHeight * currLine + ypoz + 2 + 0.123 * maxH)
                    && (yMouse <= diagRowHeight * (currLine + linesToDraw + 1) + ypoz - 1 + 0.123 * maxH)) {
                    setfillstyle(SOLID_FILL, COLOR(247, 149, 74));
                    bar(currLeft + xpoz + 2, diagRowHeight * currLine + ypoz + 2, currRight + xpoz - 1, diagRowHeight * (currLine + linesToDraw + 1) + ypoz - 1);
                    setbkcolor(COLOR(247, 149, 74));
                    printRow(currLeft, currRight, currLine, row, ypoz, xpoz);
                }
                else {
                    setfillstyle(SOLID_FILL, COLOR(255, 187, 115));
                    bar(currLeft + xpoz + 2, diagRowHeight * currLine + ypoz + 2, currRight + xpoz - 1, diagRowHeight * (currLine + linesToDraw + 1) + ypoz - 1);
                    setbkcolor(COLOR(255, 187, 115));
                    printRow(currLeft, currRight, currLine, row, ypoz, xpoz);
                }
            }

            //recursion
            row++, currLine++;
            drawInstructions(currLeft + maxW * 0.06, currRight, row, currLine, xpoz, ypoz, rowLimit);  // paint instructionss with this left this right until last bracket (that is what rowLimit is for)
        }
        else if (tip == 1)
        {
            int linesToDrawFirstB = 0;             // Find first bracket limit
            int linesInIfBrackets = 0;             // find if else exists
            int linesToDrawSecondB = 0;            // if else exists find else brackets and limits
            int linesInElseBrackets = 0;
            bool existaElse = 0;
            ifAndElseBracket(row, linesToDrawFirstB, linesInIfBrackets, linesToDrawSecondB, linesInElseBrackets, existaElse);
            rowLimitIf = row + linesInIfBrackets;
            rowLimitElse = row + linesInIfBrackets + linesInElseBrackets;

            //draw
            char tru[] = "TRUE";
            char fals[] = "FALSE";
            int linesInRectangle = max(linesToDrawFirstB, linesToDrawSecondB);
            int firstTriangle[6]{ currLeft + xpoz, diagRowHeight * currLine + ypoz, (currRight + currLeft) / 2 + xpoz, diagRowHeight * (currLine + 2) + ypoz,
                                  currLeft + xpoz, diagRowHeight * (currLine + 2) + ypoz };
            int secondTriangle[6]{ (currRight + currLeft) / 2 + xpoz, diagRowHeight * (currLine + 2) + ypoz, currRight + xpoz, diagRowHeight * currLine + ypoz,
                                  currRight + xpoz, diagRowHeight * (currLine + 2) + ypoz };
            int xMouse = mousex(), yMouse = mousey();
            if ((xMouse >= currLeft + xpoz + 2) && (xMouse <= currRight + xpoz - 1) && (yMouse >= diagRowHeight * currLine + ypoz + 2 + 0.123 * maxH)
                && (yMouse <= diagRowHeight * (currLine + linesInRectangle + 2) + ypoz - 1 + 0.123 * maxH)) {
                setfillstyle(SOLID_FILL, COLOR(100, 127, 250));
                bar(currLeft + xpoz + 2, diagRowHeight * currLine + ypoz + 2, currRight + xpoz - 1, diagRowHeight * (currLine + linesInRectangle + 2) + ypoz - 1);
                setfillstyle(SOLID_FILL, COLOR(66, 219, 20));
                fillpoly(3, firstTriangle);
                setfillstyle(SOLID_FILL, COLOR(240, 48, 38));
                fillpoly(3, secondTriangle);
                rectangle(currLeft + xpoz, diagRowHeight * currLine + ypoz, currRight + xpoz, diagRowHeight * (currLine + linesInRectangle + 2) + ypoz);
                line(currLeft + xpoz, diagRowHeight * (currLine + 2) + ypoz, currRight + xpoz, diagRowHeight * (currLine + 2) + ypoz);
                setbkcolor(COLOR(100, 127, 250));
                printRow(currLeft, currRight, currLine, row, ypoz, xpoz);
                setbkcolor(COLOR(66, 219, 20));
                settextjustify(BOTTOM_TEXT, LEFT_TEXT);
                outtextxy(currLeft + 0.01 * maxW + xpoz, diagRowHeight * (currLine + 2) - 0.01 * maxH + ypoz, tru);
                setbkcolor(COLOR(240, 48, 38));
                settextjustify(RIGHT_TEXT, BOTTOM_TEXT);
                outtextxy(currRight - 0.01 * maxW + xpoz, diagRowHeight * (currLine + 2) - 0.01 * maxH + ypoz, fals);
            }
            else {
                setfillstyle(SOLID_FILL, COLOR(128, 212, 255));
                bar(currLeft + xpoz + 2, diagRowHeight * currLine + ypoz + 2, currRight + xpoz - 1, diagRowHeight * (currLine + linesInRectangle + 2) + ypoz - 1);
                setfillstyle(SOLID_FILL, COLOR(160, 255, 105));
                fillpoly(3, firstTriangle);
                setfillstyle(SOLID_FILL, COLOR(255, 161, 161));
                fillpoly(3, secondTriangle);
                rectangle(currLeft + xpoz, diagRowHeight * currLine + ypoz, currRight + xpoz, diagRowHeight * (currLine + linesInRectangle + 2) + ypoz);
                line(currLeft + xpoz, diagRowHeight * (currLine + 2) + ypoz, currRight + xpoz, diagRowHeight * (currLine + 2) + ypoz);
                setbkcolor(COLOR(128, 212, 255));
                printRow(currLeft, currRight, currLine, row, ypoz, xpoz);
                setbkcolor(COLOR(160, 255, 105));
                settextjustify(BOTTOM_TEXT, LEFT_TEXT);
                outtextxy(currLeft + 0.01 * maxW + xpoz, diagRowHeight * (currLine + 2) - 0.01 * maxH + ypoz, tru);
                setbkcolor(COLOR(255, 161, 161));
                settextjustify(RIGHT_TEXT, BOTTOM_TEXT);
                outtextxy(currRight - 0.01 * maxW + xpoz, diagRowHeight * (currLine + 2) - 0.01 * maxH + ypoz, fals);
            }

            //recursion
            row++; currLine += 2;               // drawInstructions recursively for every if and else
            int currLineElse = currLine;
            drawInstructions(currLeft, (currRight + currLeft) / 2, row, currLine, xpoz, ypoz, rowLimitIf);
            drawInstructions((currRight + currLeft) / 2, currRight, row, currLineElse, xpoz, ypoz, rowLimitElse);
        }
        if (row >= rowLimFromPrev) break;
    }
}

/*Desenarea-casutelor-din-diagrama*/

void diagram() {
    int page = 0, ypoz = 0, xpoz = 0, xt = 0, yt = 0;
    char word3[] = "Inapoi";
    while (1)
    {
        setvisualpage(page);
        setactivepage(1 - page);
        clearviewport();
        int currLine = 1;                                                        // currLine reprezinta nr liniei orizontale la care am ajuns sa desenam
        int currLeft = maxW / 2 - diagWidth / 2, currRight = currLeft + diagWidth;       // row reprezinta randul din vectorul cu pseudocodul
        int rowLimit = 9999, rowLimitIf = 9999, rowLimitElse = 9999;
        for (int row = 0; row < rowsInTotal; row++)
        {
            int tip = tipOperatie(randuri[row]);
            if (tip >= 4 && tip <= 7)
            {
                rectangle(currLeft + xpoz, diagRowHeight * currLine + ypoz, currRight + xpoz, diagRowHeight * (currLine + 1) + ypoz);
                // the next if is for hover effect
                int xMouse = mousex(), yMouse = mousey();
                if ((xMouse >= currLeft + xpoz + 2) && (xMouse <= currRight + xpoz - 1) && (yMouse >= diagRowHeight * currLine + ypoz + 2 + 0.123 * maxH)
                    && (yMouse <= diagRowHeight * (currLine + 1) + ypoz - 1 + 0.123 * maxH)) {
                    setfillstyle(SOLID_FILL, COLOR(124, 171, 230));
                    bar(currLeft + xpoz + 2, diagRowHeight * currLine + ypoz + 2, currRight + xpoz - 1, diagRowHeight * (currLine + 1) + ypoz - 1);
                    setbkcolor(COLOR(124, 171, 230));
                    printRow(currLeft, currRight, currLine, row, ypoz, xpoz);
                }
                else {
                    setfillstyle(SOLID_FILL, COLOR(166, 252, 255));
                    bar(currLeft + xpoz + 2, diagRowHeight * currLine + ypoz + 2, currRight + xpoz - 1, diagRowHeight * (currLine + 1) + ypoz - 1);
                    setbkcolor(COLOR(166, 252, 255));
                    printRow(currLeft, currRight, currLine, row, ypoz, xpoz);
                }
                currLine++;
            }
            else if (tip == 2 || tip == 3)
            {
                int linesInBrackets = 0;                         //lines from current row to bracket (the closing bracket)
                int linesToDraw = 0;                            //horizontal lines to draw in while/for
                lastBracket(row, linesInBrackets, linesToDraw);
                rowLimit = row + linesInBrackets;

                //draw
                if (tip == 2) {
                    rectangle(currLeft + xpoz, diagRowHeight * currLine + ypoz, currRight + xpoz, diagRowHeight * (currLine + linesToDraw + 1) + ypoz);
                    int xMouse = mousex(), yMouse = mousey();
                    if ((xMouse >= currLeft + xpoz + 2) && (xMouse <= currRight + xpoz - 1) && (yMouse >= diagRowHeight * currLine + ypoz + 2 + 0.123 * maxH)
                        && (yMouse <= diagRowHeight * (currLine + linesToDraw + 1) + ypoz - 1 + 0.123 * maxH)) {
                        setfillstyle(SOLID_FILL, COLOR(255, 213, 0));
                        bar(currLeft + xpoz + 2, diagRowHeight * currLine + ypoz + 2, currRight + xpoz - 1, diagRowHeight * (currLine + linesToDraw + 1) + ypoz - 1);
                        setbkcolor(COLOR(255, 213, 0));
                        printRow(currLeft, currRight, currLine, row, ypoz, xpoz);
                    }
                    else {
                        setfillstyle(SOLID_FILL, COLOR(248, 255, 117));
                        bar(currLeft + xpoz + 2, diagRowHeight * currLine + ypoz + 2, currRight + xpoz - 1, diagRowHeight * (currLine + linesToDraw + 1) + ypoz - 1);
                        setbkcolor(COLOR(248, 255, 117));
                        printRow(currLeft, currRight, currLine, row, ypoz, xpoz);
                    }
                }
                else {
                    rectangle(currLeft + xpoz, diagRowHeight * currLine + ypoz, currRight + xpoz, diagRowHeight * (currLine + linesToDraw + 1) + ypoz);
                    int xMouse = mousex(), yMouse = mousey();
                    if ((xMouse >= currLeft + xpoz + 2) && (xMouse <= currRight + xpoz - 1) && (yMouse >= diagRowHeight * currLine + ypoz + 2 + 0.123 * maxH)
                        && (yMouse <= diagRowHeight * (currLine + linesToDraw + 1) + ypoz - 1 + 0.123 * maxH)) {
                        setfillstyle(SOLID_FILL, COLOR(247, 149, 74));
                        bar(currLeft + xpoz + 2, diagRowHeight * currLine + ypoz + 2, currRight + xpoz - 1, diagRowHeight * (currLine + linesToDraw + 1) + ypoz - 1);
                        setbkcolor(COLOR(247, 149, 74));
                        printRow(currLeft, currRight, currLine, row, ypoz, xpoz);
                    }
                    else {
                        setfillstyle(SOLID_FILL, COLOR(255, 187, 115));
                        bar(currLeft + xpoz + 2, diagRowHeight * currLine + ypoz + 2, currRight + xpoz - 1, diagRowHeight * (currLine + linesToDraw + 1) + ypoz - 1);
                        setbkcolor(COLOR(255, 187, 115));
                        printRow(currLeft, currRight, currLine, row, ypoz, xpoz);
                    }
                }

                //recursion
                row++, currLine++;
                drawInstructions(currLeft + maxW * 0.06, currRight, row, currLine, xpoz, ypoz, rowLimit);   // paint instructionss this with left this right until last bracket (that is what rowLimit is for)            
            }
            else if (tip == 1)
            {
                int linesToDrawFirstB = 0;             // Find first bracket limit
                int linesInIfBrackets = 0;             // find if else exists
                int linesToDrawSecondB = 0;            // if else exists find else brackets and limits
                int linesInElseBrackets = 0;
                bool existaElse = 0;
                ifAndElseBracket(row, linesToDrawFirstB, linesInIfBrackets, linesToDrawSecondB, linesInElseBrackets, existaElse);
                rowLimitIf = row + linesInIfBrackets;
                rowLimitElse = row + linesInIfBrackets + linesInElseBrackets;

                //draw
                char tru[] = "TRUE";
                char fals[] = "FALSE";
                int linesInRectangle = max(linesToDrawFirstB, linesToDrawSecondB);
                int firstTriangle[6]{ currLeft + xpoz, diagRowHeight * currLine + ypoz, (currRight + currLeft) / 2 + xpoz, diagRowHeight * (currLine + 2) + ypoz,
                                      currLeft + xpoz, diagRowHeight * (currLine + 2) + ypoz };
                int secondTriangle[6]{ (currRight + currLeft) / 2 + xpoz, diagRowHeight * (currLine + 2) + ypoz, currRight + xpoz, diagRowHeight * currLine + ypoz,
                                      currRight + xpoz, diagRowHeight * (currLine + 2) + ypoz };
                int xMouse = mousex(), yMouse = mousey();
                if ((xMouse >= currLeft + xpoz + 2) && (xMouse <= currRight + xpoz - 1) && (yMouse >= diagRowHeight * currLine + ypoz + 2 + 0.123 * maxH)
                    && (yMouse <= diagRowHeight * (currLine + linesInRectangle + 2) + ypoz - 1 + 0.123 * maxH)) {
                    setfillstyle(SOLID_FILL, COLOR(100, 127, 250));
                    bar(currLeft + xpoz + 2, diagRowHeight * currLine + ypoz + 2, currRight + xpoz - 1, diagRowHeight * (currLine + linesInRectangle + 2) + ypoz - 1);
                    setfillstyle(SOLID_FILL, COLOR(66, 219, 20));
                    fillpoly(3, firstTriangle);
                    setfillstyle(SOLID_FILL, COLOR(240, 48, 38));
                    fillpoly(3, secondTriangle);
                    rectangle(currLeft + xpoz, diagRowHeight * currLine + ypoz, currRight + xpoz, diagRowHeight * (currLine + linesInRectangle + 2) + ypoz);
                    line(currLeft + xpoz, diagRowHeight * (currLine + 2) + ypoz, currRight + xpoz, diagRowHeight * (currLine + 2) + ypoz);
                    setbkcolor(COLOR(100, 127, 250));
                    printRow(currLeft, currRight, currLine, row, ypoz, xpoz);
                    setbkcolor(COLOR(66, 219, 20));
                    settextjustify(BOTTOM_TEXT, LEFT_TEXT);
                    outtextxy(currLeft + 0.01 * maxW + xpoz, diagRowHeight * (currLine + 2) - 0.01 * maxH + ypoz, tru);
                    setbkcolor(COLOR(240, 48, 38));
                    settextjustify(RIGHT_TEXT, BOTTOM_TEXT);
                    outtextxy(currRight - 0.01 * maxW + xpoz, diagRowHeight * (currLine + 2) - 0.01 * maxH + ypoz, fals);
                }
                else {
                    setfillstyle(SOLID_FILL, COLOR(128, 212, 255));
                    bar(currLeft + xpoz + 2, diagRowHeight * currLine + ypoz + 2, currRight + xpoz - 1, diagRowHeight * (currLine + linesInRectangle + 2) + ypoz - 1);
                    setfillstyle(SOLID_FILL, COLOR(160, 255, 105));
                    fillpoly(3, firstTriangle);
                    setfillstyle(SOLID_FILL, COLOR(255, 161, 161));
                    fillpoly(3, secondTriangle);
                    rectangle(currLeft + xpoz, diagRowHeight * currLine + ypoz, currRight + xpoz, diagRowHeight * (currLine + linesInRectangle + 2) + ypoz);
                    line(currLeft + xpoz, diagRowHeight * (currLine + 2) + ypoz, currRight + xpoz, diagRowHeight * (currLine + 2) + ypoz);
                    setbkcolor(COLOR(128, 212, 255));
                    printRow(currLeft, currRight, currLine, row, ypoz, xpoz);
                    setbkcolor(COLOR(160, 255, 105));
                    settextjustify(BOTTOM_TEXT, LEFT_TEXT);
                    outtextxy(currLeft + 0.01 * maxW + xpoz, diagRowHeight * (currLine + 2) - 0.01 * maxH + ypoz, tru);
                    setbkcolor(COLOR(255, 161, 161));
                    settextjustify(RIGHT_TEXT, BOTTOM_TEXT);
                    outtextxy(currRight - 0.01 * maxW + xpoz, diagRowHeight * (currLine + 2) - 0.01 * maxH + ypoz, fals);
                }

                //recursion
                row++; currLine += 2;               // drawInstructions recursively for every if and else
                int currLineElse = currLine;
                drawInstructions(currLeft, (currRight + currLeft) / 2, row, currLine, xpoz, ypoz, rowLimitIf);
                drawInstructions((currRight + currLeft) / 2, currRight, row, currLineElse, xpoz, ypoz, rowLimitElse);
            }
        }
        setfillstyle(SOLID_FILL, COLOR(128, 212, 255));
        rectangle(maxW - 0.1 * maxW, 0.05 * maxH - 0.045 * maxH, maxW - 0.05 * maxW, 0.1 * maxH - 0.045 * maxH);
        bar(maxW - 0.1 * maxW + 2, 0.05 * maxH - 0.045 * maxH + 2, maxW - 0.05 * maxW - 1, 0.1 * maxH - 0.045 * maxH - 1);
        line(maxW - 0.1 * maxW, 0.05 * maxH - 0.045 * maxH, maxW - 0.05 * maxW, 0.1 * maxH - 0.045 * maxH);
        line(maxW - 0.05 * maxW, 0.05 * maxH - 0.045 * maxH, maxW - 0.1 * maxW, 0.1 * maxH - 0.045 * maxH);
        rectangle(maxW - 0.05 * maxW, maxH - 0.1 * maxH - 0.123 * maxH, maxW - 0.1 * maxW, maxH - 0.05 * maxH - 0.123 * maxH);
        bar(maxW - 0.1 * maxW + 2, maxH - 0.1 * maxH - 0.123 * maxH + 2, maxW - 0.05 * maxW - 1, maxH - 0.05 * maxH - 0.123 * maxH - 1);
        line(maxW - 0.1 * maxW, maxH - 0.05 * maxH - 0.123 * maxH, maxW - 0.05 * maxW, maxH - 0.1 * maxH - 0.123 * maxH);
        line(maxW - 0.05 * maxW, maxH - 0.05 * maxH - 0.123 * maxH, maxW - 0.1 * maxW, maxH - 0.1 * maxH - 0.123 * maxH);
        settextjustify(CENTER_TEXT, CENTER_TEXT);
        setbkcolor(COLOR(128, 212, 255));
        setfillstyle(SOLID_FILL, COLOR(128, 212, 255));
        rectangle(0.05 * maxW, 0.85 * maxH - 0.123 * maxH, 0.15 * maxW, 0.9 * maxH - 0.123 * maxH);
        bar(0.05 * maxW + 2, 0.85 * maxH - 0.123 * maxH + 2, 0.15 * maxW - 1, 0.9 * maxH - 0.123 * maxH - 1);
        outtextxy((0.05 * maxW + 0.15 * maxW) / 2, (0.85 * maxH + 0.9 * maxH) / 2 + 0.005 * maxH - 0.123 * maxH, word3);
        setbkcolor(WHITE);
        settextjustify(LEFT_TEXT, TOP_TEXT);
        xt = mousex();
        yt = mousey();
        if (GetAsyncKeyState(VK_UP))
            ypoz -= 5;
        if (GetAsyncKeyState(VK_DOWN))
            ypoz += 5;
        if (GetAsyncKeyState(VK_LEFT))
            xpoz -= 5;
        if (GetAsyncKeyState(VK_RIGHT))
            xpoz += 5;
        if (GetAsyncKeyState(VK_RBUTTON))
        {
            if (xt >= (maxW - 0.1 * maxW) && xt <= (maxW - 0.05 * maxW) && yt >= (0.05 * maxH + 0.077 * maxH) && yt <= (0.1 * maxH + 0.077 * maxH))
                diagWidth -= 4;
            else if (xt >= (maxW - 0.1 * maxW) && xt <= (maxW - 0.05 * maxW) && yt >= (maxH - 0.1 * maxH) && yt <= (maxH - 0.05 * maxH))
            {
                diagRowHeight -= 1;
            }
        }
        if (xt >= (maxW - 0.1 * maxW) && xt <= (maxW - 0.05 * maxW) && yt >= (0.05 * maxH + 0.077 * maxH) && yt <= (0.1 * maxH + 0.077 * maxH))
        {
            setfillstyle(SOLID_FILL, COLOR(79, 129, 188));
            floodfill(maxW - 0.1 * maxW + 4, 0.05 * maxH - 0.045 * maxH + 0.01 * maxH, 1);
            floodfill(maxW - 0.05 * maxW - 4, 0.1 * maxH - 0.045 * maxH - 0.01 * maxH, 1);
        }
        if (xt >= (maxW - 0.1 * maxW) && xt <= (maxW - 0.05 * maxW) && yt >= (maxH - 0.1 * maxH) && yt <= (maxH - 0.05 * maxH))
        {
            setfillstyle(SOLID_FILL, COLOR(79, 129, 188));
            floodfill(maxW - 0.1 * maxW + 0.02 * maxW, maxH - 0.1 * maxH - 0.123 * maxH + 4, 1);
            floodfill(maxW - 0.1 * maxW + 0.02 * maxW, maxH - 0.05 * maxH - 0.123 * maxH - 4, 1);
        }
        if ((xt >= 0.05 * maxW) && (xt <= 0.15 * maxW) && (yt > 0.85 * maxH) && (yt < 0.9 * maxH))
        {
            setfillstyle(SOLID_FILL, COLOR(79, 129, 188));
            floodfill(0.05 * maxW + 4, 0.85 * maxH - 0.123 * maxH + 10, 1);
            setbkcolor(COLOR(79, 129, 188));
            settextjustify(CENTER_TEXT, CENTER_TEXT);
            outtextxy((0.05 * maxW + 0.15 * maxW) / 2, (0.85 * maxH + 0.9 * maxH) / 2 + 0.005 * maxH - 0.123 * maxH, word3);
            setbkcolor(WHITE);
            settextjustify(LEFT_TEXT, TOP_TEXT);
        }
        if (GetAsyncKeyState(VK_LBUTTON))
        {
            if (xt >= (maxW - 0.1 * maxW) && xt <= (maxW - 0.05 * maxW) && yt >= (0.05 * maxH + 0.077 * maxH) && yt <= (0.1 * maxH + 0.077 * maxH))
                diagWidth += 4;
            else if (xt >= (maxW - 0.1 * maxW) && xt <= (maxW - 0.05 * maxW) && yt >= (maxH - 0.1 * maxH) && yt <= (maxH - 0.05 * maxH))
                diagRowHeight += 1;
            else if ((xt >= 0.05 * maxW) && (xt <= 0.15 * maxW) && (yt > 0.85 * maxH) && (yt < 0.9 * maxH))
            {
                delay(200);
                afisare();
            }
        }

        page = 1 - page;
    }
}

/*Generarea-Diagramei*/

void afisare()
{
    FILE* fptr = fopen(Path, "r");                   //Descidem fisierul primit ca input
    clearviewport();
    int nrRanduri = 0, maxRand = 0;
    char rand[300], maxRandSir[300];
    while (fgets(rand, sizeof(rand), fptr))          //Parcurgem fisierul
    {
        nrRanduri++;                                 //Numaram randurile pentru generarea casutei in care se va afla
        if (strlen(rand) > maxRand)
        {
            maxRand = strlen(rand);                  //Retinem lungimea celui mai lung rand
            strcpy(maxRandSir, rand);                //Retinem cel mai lung rand
        }
    }
    int page = 0, ypoz = 0, xpoz = 0, xt = 0, yt = 0;
    int maxWidthRow = textwidth(maxRandSir);             //Retinem lungimea celui mai lung rand in pixeli
    int textHeight = textheight(rand);                    //Retinem inaltimea unui rand in pixeli
    char word3[] = "Inapoi";
    char word4[] = "APASATI ENTER";
    char word5[] = "PENTRU GENERAREA DIAGRAMEI";
    while (1)
    {
        setvisualpage(page);
        setactivepage(1 - page);                   //Afisam visual page-ul curent si generam pe cel care nu este afisat
        clearviewport();
        int r = 1;
        rewind(fptr);                           //Mutam citirea din fisier inapoi la (0,0)
        //settextstyle(3, HORIZ_DIR, 3);                                                          !!! Memory Leak
        setcolor(COLOR(45, 65, 196));
        outtextxy(0.05 * maxW, 0.06 * maxH, word4);
        outtextxy(0.05 * maxW, 0.09 * maxH, word5);             //Text cu instructiuni
        //settextstyle(DEFAULT_FONT, HORIZ_DIR, 0);                                               !!! Memory Leak
        setcolor(1);
        setfillstyle(SOLID_FILL, COLOR(128, 212, 255));
        rectangle(maxW - 0.1 * maxW, 0.05 * maxH - 0.045 * maxH, maxW - 0.05 * maxW, 0.1 * maxH - 0.045 * maxH);
        bar(maxW - 0.1 * maxW + 2, 0.05 * maxH - 0.045 * maxH + 2, maxW - 0.05 * maxW - 1, 0.1 * maxH - 0.045 * maxH - 1);
        rectangle(maxW - 0.05 * maxW, maxH - 0.1 * maxH - 0.123 * maxH, maxW - 0.1 * maxW, maxH - 0.05 * maxH - 0.123 * maxH);
        bar(maxW - 0.1 * maxW + 2, maxH - 0.1 * maxH - 0.123 * maxH + 2, maxW - 0.05 * maxW - 1, maxH - 0.05 * maxH - 0.123 * maxH - 1);
        line(maxW - 0.075 * maxW, 0.05 * maxH - 0.045 * maxH, maxW - 0.1 * maxW, 0.1 * maxH - 0.045 * maxH);
        line(maxW - 0.075 * maxW, 0.05 * maxH - 0.045 * maxH, maxW - 0.05 * maxW, 0.1 * maxH - 0.045 * maxH);
        line(maxW - 0.075 * maxW, maxH - 0.05 * maxH - 0.123 * maxH, maxW - 0.1 * maxW, maxH - 0.1 * maxH - 0.123 * maxH);
        line(maxW - 0.075 * maxW, maxH - 0.05 * maxH - 0.123 * maxH, maxW - 0.05 * maxW, maxH - 0.1 * maxH - 0.123 * maxH);
        settextjustify(CENTER_TEXT, CENTER_TEXT);
        setbkcolor(COLOR(128, 212, 255));
        setfillstyle(SOLID_FILL, COLOR(128, 212, 255));
        rectangle(0.05 * maxW, 0.85 * maxH - 0.123 * maxH, 0.15 * maxW, 0.9 * maxH - 0.123 * maxH);
        bar(0.05 * maxW + 2, 0.85 * maxH - 0.123 * maxH + 2, 0.15 * maxW - 1, 0.9 * maxH - 0.123 * maxH - 1);
        outtextxy((0.05 * maxW + 0.15 * maxW) / 2, (0.85 * maxH + 0.9 * maxH) / 2 + 0.005 * maxH - 0.123 * maxH, word3);
        setbkcolor(WHITE);
        settextjustify(CENTER_TEXT, CENTER_TEXT);
        setbkcolor(COLOR(128, 212, 255));
        setfillstyle(SOLID_FILL, COLOR(128, 212, 255));
        rectangle(0.05 * maxW, 0.85 * maxH - 0.123 * maxH, 0.15 * maxW, 0.9 * maxH - 0.123 * maxH);                                  //Butonul de inapoi                
        outtextxy((0.05 * maxW + 0.15 * maxW) / 2, (0.85 * maxH + 0.9 * maxH) / 2 + 0.005 * maxH - 0.123 * maxH, word3);
        setbkcolor(WHITE);
        settextjustify(LEFT_TEXT, TOP_TEXT);
        xt = mousex();
        yt = mousey();
        if (xt >= (maxW - 0.1 * maxW) && xt <= (maxW - 0.05 * maxW) && yt >= (0.05 * maxH + 0.077 * maxH) && yt <= (0.1 * maxH + 0.077 * maxH))
        {
            setfillstyle(SOLID_FILL, COLOR(79, 129, 188));
            floodfill(maxW - 0.1 * maxW + 0.02 * maxW, 0.1 * maxH - 0.045 * maxH - 2, 1);                       //Coloram parte din butonul de scroll in sus daca mouse-ul trece peste acesta
        }
        if (xt >= (maxW - 0.1 * maxW) && xt <= (maxW - 0.05 * maxW) && yt >= (maxH - 0.1 * maxH) && yt <= (maxH - 0.05 * maxH))
        {
            setfillstyle(SOLID_FILL, COLOR(79, 129, 188));
            floodfill(maxW - 0.1 * maxW + 0.02 * maxW, maxH - 0.1 * maxH - 0.123 * maxH + 2, 1);                 //Coloram parte din butonul de scroll in jos daca mouse-ul trece peste acesta
        }
        if ((xt >= 0.05 * maxW) && (xt <= 0.15 * maxW) && (yt > 0.85 * maxH) && (yt < 0.9 * maxH))
        {
            setfillstyle(SOLID_FILL, COLOR(79, 129, 188));
            floodfill(0.05 * maxW + 4, 0.85 * maxH - 0.123 * maxH + 10, 1);
            setbkcolor(COLOR(79, 129, 188));
            settextjustify(CENTER_TEXT, CENTER_TEXT);
            outtextxy((0.05 * maxW + 0.15 * maxW) / 2, (0.85 * maxH + 0.9 * maxH) / 2 + 0.005 * maxH - 0.123 * maxH, word3);
            setbkcolor(WHITE);
            settextjustify(LEFT_TEXT, TOP_TEXT);
        }
        if (GetAsyncKeyState(VK_LBUTTON))
        {
            if (xt >= (maxW - 0.1 * maxW) && xt <= (maxW - 0.05 * maxW) && yt >= (0.05 * maxH + 0.077 * maxH) && yt <= (0.1 * maxH + 0.077 * maxH))
                ypoz += 5;                    //Crestem offset-ul ypoz daca se apasa butonul de scroll in sus
            else if (xt >= (maxW - 0.1 * maxW) && xt <= (maxW - 0.05 * maxW) && yt >= (maxH - 0.1 * maxH) && yt <= (maxH - 0.05 * maxH))
                ypoz -= 5;                      //Scadem offset-ul ypoz daca se apasa butonul de scroll in jos
            else if ((xt >= 0.05 * maxW) && (xt <= 0.15 * maxW) && (yt > 0.85 * maxH) && (yt < 0.9 * maxH))
            {
                delay(200);                    //Ne intoarcem la ecranul principal daca butonul inapoi este apasat, un delay este necesar pentru a nu citi click-uri continuu
                start();
            }
        }
        if (GetAsyncKeyState(VK_UP))
            ypoz -= 5;                             //Crestem offset-ul ypoz daca se apasa tasta sus
        if (GetAsyncKeyState(VK_DOWN))
            ypoz += 5;                              //Scadem offset-ul ypoz daca se apasa tasta jos
        rectangle(maxW / 2 - (maxWidthRow / 2) - maxW * 0.01 + xpoz, maxH * 0.1 + ypoz, maxW / 2 + (maxWidthRow / 2) + maxW * 0.01 + xpoz, maxH * 0.1 + (textHeight * (nrRanduri + 1)) + ypoz); //Generam casuta in care se afla continutul fisierului pe baza numarului de randuri, al dstandeti dinte randuri verticala in pixeli si al offset-ului ypoz ce ii confera abilitatea de scroll 
        while (fgets(rand, sizeof(rand), fptr))
        {
            outtextxy(maxW / 2 - (maxWidthRow / 2) + xpoz, maxH * 0.1 + (textHeight * r) + ypoz, rand); //Afisam rand cu rand continutul fisierului
            r++;
        }
        page = 1 - page;
        if (GetAsyncKeyState(VK_RETURN))
        {
            copiereRanduri();   //Retinem numarul de randuri intr-o variabila globala prin functia copiereRanduri
            diagram();            //Trecem la generarea diagramei daca tasata enter a fost apasata
        }
    }
}

/*Initializarea-Ferestrei-si-a-catorva-modificari-globale*/

void alegeFisier()
{
    OPENFILENAMEA ofn;                  //Declaram o variabila de tipul OPENFILENAMEA, o structura ce se regaseste in biblioteca Windows.h
    ZeroMemory(&ofn, sizeof(ofn));      //Golim o zona de memorie de marimea variabilei structurii pentru a putea seta anume parametri in aceasta
    ofn.lStructSize = sizeof(ofn);      //Setam memoria folosita de catre variabila structurii si variabilele din structura sa nu depaseasca tipul OPENFILENAMEA
    ofn.lpstrFile = Path;               //I sirul declarat global Path salvam locatia fisierului ales
    *(ofn.lpstrFile) = NULL;            //Umplem locatia Path-ului din structura cu NULL
    ofn.nMaxFile = sizeof(Path);        //Adaugam inca un NULL la (Se cere in socumentatia functiei)
    if (GetOpenFileNameA(&ofn) == 0)    //Verificam daca am primit sau nu un fisier valid
    {
        start();                        //Daca fisierul nu este valid, ne intoarcem la ecranul principal
    }
    afisare();                          //Daca fisierul este valid, continuam la afisarea acestuia
}

/*Alegerea-fisierului-din-Windows-exporer*/

void reguli()
{
    clearviewport();
    int nrRanduri = 0, maxRand = 0;
    char rand[300], maxRandSir[300];
    while (fgets(rand, sizeof(rand), reg))   //Parcurgem fisierul rand cu rand
    {
        nrRanduri++;                      //Numaram cate randuri sunt in fisier
        if (strlen(rand) > maxRand)
        {
            maxRand = strlen(rand);            //Memoram lungimea celui mai lung rand pentru a scala fereasta in care acesta se va afla
            strcpy(maxRandSir, rand);            //Memoram cel mai lung sir
        }
    }
    int page = 0, ypoz = 0, xpoz = 0, xt = 0, yt = 0;
    int maxWidthRow = textwidth(maxRandSir);         //Pe baza randului memorat setam o variabila ce va reprezenta un "offset" pentru generarea ferestrei, in pixeli
    int textHeight = textheight(rand);               //Asemanator retinem o valoare pentru delimitarea verticala a textului pe baza inaltimii acestuia, in pixeli
    char word3[] = "Inapoi";
    while (1)
    {
        setvisualpage(page);
        setactivepage(1 - page);                //Schimbam visualpage-urile pentru scroll
        clearviewport();
        int r = 1;
        rewind(reg);                             //Aduceam citirea fisierului inapoi la punctul (0/0)
        setcolor(1);
        setfillstyle(SOLID_FILL, COLOR(128, 212, 255));
        rectangle(maxW - 0.1 * maxW, 0.05 * maxH - 0.045 * maxH, maxW - 0.05 * maxW, 0.1 * maxH - 0.045 * maxH);
        bar(maxW - 0.1 * maxW + 2, 0.05 * maxH - 0.045 * maxH + 2, maxW - 0.05 * maxW - 1, 0.1 * maxH - 0.045 * maxH - 1);
        rectangle(maxW - 0.05 * maxW, maxH - 0.1 * maxH - 0.123 * maxH, maxW - 0.1 * maxW, maxH - 0.05 * maxH - 0.123 * maxH);
        bar(maxW - 0.1 * maxW + 2, maxH - 0.1 * maxH - 0.123 * maxH + 2, maxW - 0.05 * maxW - 1, maxH - 0.05 * maxH - 0.123 * maxH - 1);
        line(maxW - 0.075 * maxW, 0.05 * maxH - 0.045 * maxH, maxW - 0.1 * maxW, 0.1 * maxH - 0.045 * maxH);
        line(maxW - 0.075 * maxW, 0.05 * maxH - 0.045 * maxH, maxW - 0.05 * maxW, 0.1 * maxH - 0.045 * maxH);
        line(maxW - 0.075 * maxW, maxH - 0.05 * maxH - 0.123 * maxH, maxW - 0.1 * maxW, maxH - 0.1 * maxH - 0.123 * maxH);
        line(maxW - 0.075 * maxW, maxH - 0.05 * maxH - 0.123 * maxH, maxW - 0.05 * maxW, maxH - 0.1 * maxH - 0.123 * maxH);
        settextjustify(CENTER_TEXT, CENTER_TEXT);
        setbkcolor(COLOR(128, 212, 255));
        setfillstyle(SOLID_FILL, COLOR(128, 212, 255));
        rectangle(0.05 * maxW, 0.85 * maxH - 0.123 * maxH, 0.15 * maxW, 0.9 * maxH - 0.123 * maxH);
        bar(0.05 * maxW + 2, 0.85 * maxH - 0.123 * maxH + 2, 0.15 * maxW - 1, 0.9 * maxH - 0.123 * maxH - 1);
        outtextxy((0.05 * maxW + 0.15 * maxW) / 2, (0.85 * maxH + 0.9 * maxH) / 2 + 0.005 * maxH - 0.123 * maxH, word3);
        setbkcolor(WHITE);
        settextjustify(CENTER_TEXT, CENTER_TEXT);
        setbkcolor(COLOR(128, 212, 255));
        setfillstyle(SOLID_FILL, COLOR(128, 212, 255));
        rectangle(0.05 * maxW, 0.85 * maxH - 0.123 * maxH, 0.15 * maxW, 0.9 * maxH - 0.123 * maxH);                                  //Butonul de inapoi                
        outtextxy((0.05 * maxW + 0.15 * maxW) / 2, (0.85 * maxH + 0.9 * maxH) / 2 + 0.005 * maxH - 0.123 * maxH, word3);
        setbkcolor(WHITE);
        settextjustify(LEFT_TEXT, TOP_TEXT);
        xt = mousex();
        yt = mousey();
        if (xt >= (maxW - 0.1 * maxW) && xt <= (maxW - 0.05 * maxW) && yt >= (0.05 * maxH + 0.077 * maxH) && yt <= (0.1 * maxH + 0.077 * maxH))
        {
            setfillstyle(SOLID_FILL, COLOR(79, 129, 188));
            floodfill(maxW - 0.1 * maxW + 0.02 * maxW, 0.1 * maxH - 0.045 * maxH - 2, 1);    //Coloram doar parte din primul buton
        }
        if (xt >= (maxW - 0.1 * maxW) && xt <= (maxW - 0.05 * maxW) && yt >= (maxH - 0.1 * maxH) && yt <= (maxH - 0.05 * maxH))
        {
            setfillstyle(SOLID_FILL, COLOR(79, 129, 188));                                  //Coloram doar parte din al doilea buton
            floodfill(maxW - 0.1 * maxW + 0.02 * maxW, maxH - 0.1 * maxH - 0.123 * maxH + 2, 1);
        }
        if ((xt >= 0.05 * maxW) && (xt <= 0.15 * maxW) && (yt > 0.85 * maxH) && (yt < 0.9 * maxH))
        {
            setfillstyle(SOLID_FILL, COLOR(79, 129, 188));
            floodfill(0.05 * maxW + 4, 0.85 * maxH - 0.123 * maxH + 10, 1);
            setbkcolor(COLOR(79, 129, 188));
            settextjustify(CENTER_TEXT, CENTER_TEXT);
            outtextxy((0.05 * maxW + 0.15 * maxW) / 2, (0.85 * maxH + 0.9 * maxH) / 2 + 0.005 * maxH - 0.123 * maxH, word3);
            setbkcolor(WHITE);
            settextjustify(LEFT_TEXT, TOP_TEXT);
        }
        if (GetAsyncKeyState(VK_LBUTTON))
        {
            if (xt >= (maxW - 0.1 * maxW) && xt <= (maxW - 0.05 * maxW) && yt >= (0.05 * maxH + 0.077 * maxH) && yt <= (0.1 * maxH + 0.077 * maxH))
                ypoz += 5;                   //Crestem cu 5 un o varaiabila "offset" folosita in generarea cadranului regulilor ce ii va da propritatea de scroll
            else if (xt >= (maxW - 0.1 * maxW) && xt <= (maxW - 0.05 * maxW) && yt >= (maxH - 0.1 * maxH) && yt <= (maxH - 0.05 * maxH))
                ypoz -= 5;                   //Micsoram cu 5 un o varaiabila "offset" folosita in generarea cadranului regulilor ce ii va da propritatea de scroll
            else if ((xt >= 0.05 * maxW) && (xt <= 0.15 * maxW) && (yt > 0.85 * maxH) && (yt < 0.9 * maxH))
            {
                delay(200);              //Mergem inapoi la ecranul principal daca butonul inapoi a fost apasat
                start();
            }
        }
        if (GetAsyncKeyState(VK_UP))
            ypoz -= 5;                             //Crestem offset-ul ypoz daca se apasa tasta sus
        if (GetAsyncKeyState(VK_DOWN))
            ypoz += 5;                              //Scadem offset-ul ypoz daca se apasa tasta jos
        rectangle(maxW / 2 - (maxWidthRow / 2) - maxW * 0.01 + xpoz, maxH * 0.1 + ypoz, maxW / 2 + (maxWidthRow / 2) + maxW * 0.01 + xpoz, maxH * 0.1 + (textHeight * (nrRanduri + 1)) + ypoz);   //Se genereaza casuta in care se va afla textul pe baza "offsetului" folosit la scroll, lungimea celui mai lung sir si lungimea dintre siruri
        while (fgets(rand, sizeof(rand), reg))
        {
            outtextxy(maxW / 2 - (maxWidthRow / 2) + xpoz, maxH * 0.1 + (textHeight * r) + ypoz, rand); //Parcurgem fisierul si afisam rand cu rand
            r++;
        }
        page = 1 - page;     //Trecem la urmatorul visual page
    }
}

/*Ecranul-cu-regulile-de-sintaxa*/

void start()
{
    int x = 0, y = 0, page = 0, c = 1;
    char word[20] = "Alege fisier text";
    char word2[20] = "Iesire";
    char word3[20] = "Reguli de sintaxa";
    while (1)
    {
        setactivepage(page);            //schimbam intre doua visualpage-uri, una afista si una pe care se afiseaza elementele decalate 
        setvisualpage(1 - page);        //pentru a realiza scroll
        clearviewport();
        settextjustify(CENTER_TEXT, CENTER_TEXT);
        rectangle(maxW / 2 - 0.1 * maxW, maxH / 2 - 0.123 * maxH - 0.2 * maxH, maxW / 2 + 0.1 * maxW, maxH / 2 + 0.1 * maxH - 0.123 * maxH - 0.2 * maxH); //Primul buton
        rectangle(maxW / 2 - 0.1 * maxW, maxH / 2 - 0.123 * maxH, maxW / 2 + 0.1 * maxW, maxH / 2 + 0.1 * maxH - 0.123 * maxH);  //Al doilea buton
        rectangle(maxW / 2 - 0.1 * maxW, maxH / 2 + 0.2 * maxH - 0.123 * maxH, maxW / 2 + 0.1 * maxW, maxH / 2 + 0.3 * maxH - 0.123 * maxH);  //Al treilea buton
        setbkcolor(COLOR(128, 212, 255));
        setfillstyle(SOLID_FILL, COLOR(128, 212, 255));
        floodfill(maxW / 2 - 0.1 * maxW + 10, maxH / 2 - 0.123 * maxH - 0.2 * maxH + 10, 1);
        floodfill(maxW / 2 - 0.1 * maxW + 10, maxH / 2 - 0.123 * maxH + 10, 1);                       //Coloram butoanele
        floodfill(maxW / 2 - 0.1 * maxW + 10, maxH / 2 + 0.2 * maxH - 0.123 * maxH + 10, 1);
        outtextxy(maxW / 2, maxH / 2 + 0.06 * maxH - 0.123 * maxH, word);
        outtextxy(maxW / 2, maxH / 2 + 0.26 * maxH - 0.123 * maxH, word2);
        outtextxy(maxW / 2, maxH / 2 + 0.06 * maxH - 0.123 * maxH - 0.2 * maxH, word3);
        setbkcolor(WHITE);
        x = mousex();
        y = mousey();
        if (x >= (maxW / 2 - 0.1 * maxW + 1) && x <= (maxW / 2 + 0.1 * maxW - 1) && y >= (maxH / 2 + 1) && y <= (maxH / 2 + 0.1 * maxH - 1))
        {
            setbkcolor(COLOR(79, 129, 188));
            setfillstyle(SOLID_FILL, COLOR(79, 129, 188));
            floodfill(maxW / 2 - 0.1 * maxW + 10, maxH / 2 - 0.123 * maxH + 10, 1);   //Coloram primul buton diferit daca mouse-ul trece peste acesta
            outtextxy(maxW / 2, maxH / 2 + 0.06 * maxH - 0.123 * maxH, word);
            setbkcolor(WHITE);
        }
        if (x >= (maxW / 2 - 0.1 * maxW + 1) && x <= (maxW / 2 + 0.1 * maxW - 1) && y >= (maxH / 2 + 0.2 * maxH + 1) && y <= (maxH / 2 + 0.3 * maxH - 1))
        {
            setbkcolor(COLOR(79, 129, 188));
            setfillstyle(SOLID_FILL, COLOR(79, 129, 188));
            floodfill(maxW / 2 - 0.1 * maxW + 10, maxH / 2 + 0.2 * maxH - 0.123 * maxH + 10, 1);     //Coloram al doilea buton diferit daca mouse-ul trece peste acesta
            outtextxy(maxW / 2, maxH / 2 + 0.26 * maxH - 0.123 * maxH, word2);
            setbkcolor(WHITE);
        }
        if (x >= (maxW / 2 - 0.1 * maxW + 1) && x <= (maxW / 2 + 0.1 * maxW - 1) && y >= (maxH / 2 - 0.1 * maxH - 0.1 * maxH + 1) && y <= (maxH / 2 + 0.1 * maxH - 0.1 * maxH - 0.1 * maxH - 1))
        {
            setbkcolor(COLOR(79, 129, 188));
            setfillstyle(SOLID_FILL, COLOR(79, 129, 188));
            floodfill(maxW / 2 - 0.1 * maxW + 10, maxH / 2 - 0.123 * maxH - 0.2 * maxH + 10, 1);     //Coloram al treilea buton diferit daca mouse-ul trece peste acesta
            outtextxy(maxW / 2, maxH / 2 + 0.06 * maxH - 0.123 * maxH - 0.2 * maxH, word3);
            setbkcolor(WHITE);
        }
        if (GetAsyncKeyState(VK_LBUTTON))
        {
            if (x >= (maxW / 2 - 0.1 * maxW + 1) && x <= (maxW / 2 + 0.1 * maxW - 1) && y >= (maxH / 2 + 1) && y <= (maxH / 2 + 0.1 * maxH - 1))
            {
                alegeFisier();              //Trecem la selectia fisierului daca al doilea buton a fost apasat
            }
            else if (x >= (maxW / 2 - 0.1 * maxW + 1) && x <= (maxW / 2 + 0.1 * maxW - 1) && y >= (maxH / 2 + 0.2 * maxH + 1) && y <= (maxH / 2 + 0.3 * maxH - 1))
            {
                exit(1);                  //Iesim daca daca al treilea buton a fost apasat
            }
            else if (x >= (maxW / 2 - 0.1 * maxW + 1) && x <= (maxW / 2 + 0.1 * maxW - 1) && y >= (maxH / 2 - 0.1 * maxH - 0.1 * maxH + 1) && y <= (maxH / 2 + 0.1 * maxH - 0.1 * maxH - 0.1 * maxH - 1))
            {
                reguli();          //Afisam regulile sintactice daca al treilea buton a fost afisat
            }
        }
        settextjustify(LEFT_TEXT, TOP_TEXT);
        page = 1 - page;      //Trecem la urmatorul visualpage
    }
}

/*Ecranul-Principal*/

void rezolutie()
{
    initwindow(800, 600, "Generator de Diagrame Nassi-Shneiderman");      //Initializarea ferestrei de alegere a rezolutiei
    bool ok = true;
    int page = 0, x, y;
    char b11[100] = "Lasa programul";                                //Textele butoanelor
    char b12[200] = "sa aleaga";
    char b2[100] = "960 x 500";
    char b3[100] = "1024 x 768";
    char b4[100] = "1152 x 870";
    char b5[100] = "1270 x 720";
    char b6[100] = "1440 x 900";
    char b7[100] = "1600 x 900";
    char b8[100] = "1920 x 1080";
    setactivepage(page);
    readimagefile("rezolutie.bmp", 0, 0, 800, 600);         //Afisarea fundalului pe primul visual page
    setactivepage(1 - page);
    readimagefile("rezolutie.bmp", 0, 0, 800, 600);        //Afisarea fundalului pe al doilea visual page
    setvisualpage(page);
    setlinestyle(SOLID_LINE, 1, 3);
    setviewport(0, 50, 800, 600, 1);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    settextstyle(1, HORIZ_DIR, 0);
    setusercharsize(1, 2, 1, 2);
    setcolor(1);
    while (ok)
    {
        setactivepage(page);
        setvisualpage(1 - page);
        clearviewport();
        x = mousex();
        y = mousey();
        setbkcolor(COLOR(128, 212, 255));                                          //Desenarea butoanelor si afisarea textelor
        setfillstyle(SOLID_FILL, COLOR(128, 212, 255));
        rectangle(100, 25, 300, 125);
        floodfill(103, 28, 1);
        outtextxy(200, 75, b11);
        outtextxy(200, 100, b12);
        rectangle(100, 150, 300, 250);
        floodfill(103, 153, 1);
        outtextxy(200, 200, b2);
        rectangle(100, 275, 300, 375);
        floodfill(103, 278, 1);
        outtextxy(200, 325, b3);
        rectangle(100, 400, 300, 500);
        floodfill(103, 403, 1);
        outtextxy(200, 450, b4);
        rectangle(500, 25, 700, 125);
        floodfill(503, 28, 1);
        outtextxy(600, 75, b5);
        rectangle(500, 150, 700, 250);
        floodfill(503, 153, 1);
        outtextxy(600, 200, b6);
        rectangle(500, 275, 700, 375);
        floodfill(503, 278, 1);
        outtextxy(600, 325, b7);
        rectangle(500, 400, 700, 500);
        floodfill(503, 403, 1);
        outtextxy(600, 450, b8);
        if (x >= 103 and x <= 297 and y >= 78 and y <= 172)                   //Primul buton
        {
            setfillstyle(SOLID_FILL, COLOR(79, 129, 188));
            floodfill(103, 28, 1);
            setbkcolor(COLOR(79, 129, 188));                         //Efectul de hover
            outtextxy(200, 75, b11);
            outtextxy(200, 100, b12);
            if (ismouseclick(WM_LBUTTONDOWN) == true)
            {
                ok = false;                                           //Verificarea apasarii primului buton
            }
        }
        else if (x >= 103 and x <= 297 and y >= 203 and y <= 297)           //Al doilea buton
        {
            setfillstyle(SOLID_FILL, COLOR(79, 129, 188));
            floodfill(103, 153, 1);
            setbkcolor(COLOR(79, 129, 188));                               //Efectul de hover
            outtextxy(200, 200, b2);
            if (ismouseclick(WM_LBUTTONDOWN) == true)
            {
                maxW = 960;
                maxH = 500;                                               //Verificarea apasarii celui de al doilea buton
                ok = false;
            }
        }
        else if (x >= 103 and x <= 297 and y >= 328 and y <= 422)             //Al treilea buton
        {
            setfillstyle(SOLID_FILL, COLOR(79, 129, 188));
            floodfill(103, 278, 1);
            setbkcolor(COLOR(79, 129, 188));                            //Efectul de hover
            outtextxy(200, 325, b3);
            if (ismouseclick(WM_LBUTTONDOWN) == true)
            {
                maxW = 1024;
                maxH = 768;                                    //Verificarea apasarii celui de al treilea buton
                ok = false;
            }
        }
        else if (x >= 103 and x <= 297 and y >= 453 and y <= 547)                    //Al patrulea buton
        {
            setfillstyle(SOLID_FILL, COLOR(79, 129, 188));
            floodfill(103, 403, 1);
            setbkcolor(COLOR(79, 129, 188));                               //Efectul de hover
            outtextxy(200, 450, b4);
            if (ismouseclick(WM_LBUTTONDOWN) == true)
            {
                maxW = 1152;
                maxH = 870;                                                      //Verificarea apasarii celui de al patrulea buton
                ok = false;
            }
        }
        else if (x >= 503 and x <= 697 and y >= 78 and y <= 172)                     //Al cincilea buton
        {
            setfillstyle(SOLID_FILL, COLOR(79, 129, 188));
            floodfill(503, 28, 1);
            setbkcolor(COLOR(79, 129, 188));                              //Efectul de hover
            outtextxy(600, 75, b5);
            if (ismouseclick(WM_LBUTTONDOWN) == true)
            {
                maxW = 1270;
                maxH = 720;                                             //Verificarea apasarii celui de al cincilea buton
                ok = false;
            }
        }
        else if (x >= 503 and x <= 697 and y >= 203 and y <= 297)                  //Al saselea buton
        {
            setfillstyle(SOLID_FILL, COLOR(79, 129, 188));
            floodfill(503, 153, 1);
            setbkcolor(COLOR(79, 129, 188));                               //Efectul de hover
            outtextxy(600, 200, b6);
            if (ismouseclick(WM_LBUTTONDOWN) == true)
            {
                maxW = 1440;
                maxH = 900;                                                  //Verificarea apasarii celui de al saselea buton
                ok = false;
            }
        }
        else if (x >= 503 and x <= 697 and y >= 328 and y <= 422)                 //Al saptelea buton
        {
            setfillstyle(SOLID_FILL, COLOR(79, 129, 188));
            floodfill(503, 278, 1);
            setbkcolor(COLOR(79, 129, 188));                              //Efectul de hover
            outtextxy(600, 325, b7);
            if (ismouseclick(WM_LBUTTONDOWN) == true)
            {
                maxW = 1600;
                maxH = 900;                                          //Verificarea apasarii celui de al saptelea buton
                ok = false;
            }
        }
        else if (x >= 503 and x <= 697 and y >= 453 and y <= 547)              //Al optelea buton
        {
            setfillstyle(SOLID_FILL, COLOR(79, 129, 188));
            floodfill(503, 403, 1);
            setbkcolor(COLOR(79, 129, 188));                                //Efectul de hover
            outtextxy(600, 450, b8);
            if (ismouseclick(WM_LBUTTONDOWN) == true)
            {
                maxW = 1920;
                maxH = 1080;                                        //Verificarea apasarii celui de al optilea buton
                ok = false;
            }
        }
        setbkcolor(WHITE);
        page = 1 - page;
    }
    closegraph();
}

/*Ecranul-De-Alegere-Al-Rezolutiei*/

int main()
{
    rezolutie();
    int page = 0;
    initwindow(maxW, maxH, "Generator de Diagrame Nassi-Shneiderman"); //Initializarea ferestrei
    setactivepage(page);
    readimagefile("image.bmp", 0, 0, maxW, maxH);      //Afisam imaginea image.bmp pe ambele visualpage-uri pe care programul le foloseste pentru
    setactivepage(1 - page);                           //a putea reduce viewport-ul si a nu sterege anumite elemente pe care vrem sa le lasam in fundal
    readimagefile("image.bmp", 0, 0, maxW, maxH);
    setvisualpage(1 - page);
    setcolor(1);
    setbkcolor(WHITE);
    settextstyle(1, HORIZ_DIR, 0);
    setusercharsize(1, 0.1 * maxW, 1, 0.1 * maxH);
    setlinestyle(SOLID_LINE, 1, 3);
    setviewport(0, 0.123 * maxH, maxW, maxH - 0.045 * maxH, 1);
    start();                                             //Functia care duce la primul ecran al programului 
    setfillstyle(SOLID_FILL, 4);
    int a[6]{ 200,200,400,400,200,400 };
    fillpoly(6, a);
    getch();
    closegraph();
    return 0;
}

/*Initializarea-Ferestrei-si-a-catorva-modificari-globale*/
