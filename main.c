/* main.c
 *
 * Copyright 2023 Fernando Osorio
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include"PrintArray.h"

#define RAM_SIZE  10000
#define SWAP_SIZE 20000
#define BUFF_SIZE 1000
#define NAME_SIZE 4

char ram[RAM_SIZE], swap[SWAP_SIZE];
int currRamLoc = 0;    // Current RAM location
int         ID = 0;    // Process ID

typedef struct processheader {
    char name[NAME_SIZE];
    int  size; // Size in bytes.
}ProcessHeader;

int prcHeadSize = sizeof(ProcessHeader);

typedef struct page {
    int number;
    int   size;
    char* info;
}Page;

int pageSize = sizeof(Page);

struct BUFFER {       // -New processes can be allocated just at the end.
    int firstIndexAvailable; // -We'll navigate the buffer with this index.
    int firstProcIndex;
    char memory[BUFF_SIZE];
} buffer = {0,0};

void printBytes(const char* bytes, int len);
void printBufferBytes();

// -Allocates a new process in the buffer.
// -For the time being info = NULL
void newProcess(char name[NAME_SIZE], int numofPages);

// -Allocates process from BUFFER to RAM.
// -This function does not delete data from buffer.
void loadProcess();

void viewRAM();

int main (int argc, char *argv[])
{
    char name[4] = {0x11, 0x22, 0x33, 0x44};
    newProcess(name, 3);
    newProcess(name, 2);
    newProcess(name, 4);
    printBufferBytes();
    printf("\n");
    loadProcess();
    loadProcess();
    loadProcess();
    viewRAM ();
    return EXIT_SUCCESS;
}

void printBytes(const char* bytes, int len) {
    printf("[");
    for(int i = 0; i < len; i++) {
        if(bytes[i] < 16 && bytes[i] >= 0) printf("0");
        printf("%X", (unsigned char)bytes[i]);
        if(i != len - 1) printf(",");
    }
    printf("]");
}

void printBufferBytes() {
    IntChar size;
    int i = 0; // We'll use i to run trough the buffer memory.
    int j = 0; // Page counter,
    int k = 0; // Process counter.
    int t = 0; // Temporal

    printf("---------------------------------------BUFFER--------------------"
        "--------------\n");
    for(k = 0; i < buffer.firstIndexAvailable; k++, t = i, j = 0) {
        printf("Process %d:-----------------------------------------------"
        "-----\nName:  ", k+1);
        // Printing name of first process.
        printBytes(&buffer.memory[i], NAME_SIZE);
        i += NAME_SIZE;
        printf("\nSize:  ");
        // Printing size of first process.
        printBytes(&buffer.memory[i], 4);
        // Getting size of first process.
        size.ch[0] = buffer.memory[i++];
        size.ch[1] = buffer.memory[i++];
        size.ch[2] = buffer.memory[i++];
        size.ch[3] = buffer.memory[i++];

        printf("\nPage |  number   ||   size    ||         info*         |\n");

        while(i-t < size.in) {
            printf("  %d  ", j++);
            printBytes(&buffer.memory[i], 4); i += 4;
            printBytes(&buffer.memory[i], 4); i += 4;
            printBytes(&buffer.memory[i], 8); i += 8;
            printf("\n");
        }
    }
}

void newProcess(char name[NAME_SIZE], int numofPages) {
    // Warning, we are supposing there is enough space
    // to allocate a new process in the BUFFER.
    int size = NAME_SIZE + 4 + pageSize * numofPages;
    // -Preparing the conversion from int to char[4].
    IntChar tmp = {size};
    // -Setting name and updating current index.
    for(int i = 0; i < NAME_SIZE; i++)
        buffer.memory[buffer.firstIndexAvailable++] = name[i];
    // -Setting size and updating current index.
    buffer.memory[buffer.firstIndexAvailable++] = tmp.ch[0];
    buffer.memory[buffer.firstIndexAvailable++] = tmp.ch[1];
    buffer.memory[buffer.firstIndexAvailable++] = tmp.ch[2];
    buffer.memory[buffer.firstIndexAvailable++] = tmp.ch[3];
    // -Allocating pages
    Page bf = {0,pageSize,NULL};
    for(; bf.number < numofPages; bf.number++, buffer.firstIndexAvailable += pageSize)
        memcpy(&buffer.memory[buffer.firstIndexAvailable], &bf, pageSize);
}

void loadProcess() {
    // Supposing there is enough space in ram
    IntChar _ID = {ID++};
    ram[currRamLoc++] = _ID.ch[0];
    ram[currRamLoc++] = _ID.ch[1];
    ram[currRamLoc++] = _ID.ch[2];
    ram[currRamLoc++] = _ID.ch[3];
    // Supposing the Buffer is not empty
    IntChar sz; // Process size
    sz.ch[0] = buffer.memory[buffer.firstProcIndex+4];
    sz.ch[1] = buffer.memory[buffer.firstProcIndex+5];
    sz.ch[2] = buffer.memory[buffer.firstProcIndex+6];
    sz.ch[3] = buffer.memory[buffer.firstProcIndex+7];

    memcpy(&ram[currRamLoc], &buffer.memory[buffer.firstProcIndex], sz.in);
    buffer.firstProcIndex += sz.in;
    currRamLoc += sz.in;

    IntChar t; // Process time and processor time
    srand(time(NULL)); t.in = rand() % 12;
    // Process time
    ram[currRamLoc++] = t.ch[0];
    ram[currRamLoc++] = t.ch[1];
    ram[currRamLoc++] = t.ch[2];
    ram[currRamLoc++] = t.ch[3];
    t.in = rand() % 4;
    // Processor time
    ram[currRamLoc++] = t.ch[0];
    ram[currRamLoc++] = t.ch[1];
    ram[currRamLoc++] = t.ch[2];
    ram[currRamLoc++] = t.ch[3];
}

void viewRAM() {
    int i, // Runs trough bytes of the ram
        j, // Meant for the copy of contents
        t, // Holds the value of 'i' temporarily
        c; // Counter of cycles
    int pageAmount;
    IntChar _ID;
    IntChar sz = {0};
    ProcessHeader ph;

    printf("---------------------------------------RAM-----------------------"
        "----------\n");
    printf(" ID ||  Process  ||  #Pages  ||  RAM loc  |\n");

    for(c = 0, i = 0; (t=i) + c*2 < currRamLoc; c++, i += sz.in) {
        _ID.ch[0] = ram[i++];
        _ID.ch[1] = ram[i++];
        _ID.ch[2] = ram[i++];
        _ID.ch[3] = ram[i++];
        printf(" %d  |", _ID.in);
        // Retrieving process name.
        for(j = 0; j < 4; j++) ph.name[j] = ram[i+j];
        printBytes(ph.name, 4);
        // Retrieving process size.
        for(j = 0; j < 4; j++) sz.ch[j] = ram[i+4+j];
        // Naive way of calculating the number of pages.
        pageAmount = (sz.in - prcHeadSize)/pageSize;
        printf("|     %d    |",pageAmount);
        if(t < 10)                    printf("|     %d     |\n",t);
        else if(10  <= t && t < 100)  printf("|     %d    |\n",t);
        else if(100 <= t && t < 1000) printf("|    %d    |\n",t);
        else                          printf("|    %d   |\n",t);
    }
}
