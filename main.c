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

#define SWAP_SIZE 10240 // 10 Kb
#define RAM_SIZE  5120  // 5  Kb
#define BUFF_SIZE 1024  // 1  Kb
#define NAME_SIZE 4

char ram[RAM_SIZE], swap[SWAP_SIZE];
int currRamLoc = 0;    // Current RAM location
int         ID = 0;    // Process ID

char* ptrs_prcss[64];  // Pointer to process

int sz_int = sizeof(int);

typedef struct processheader {
    char name[NAME_SIZE];
    int  size; // Size in bytes.
    int  TTP;  // Total time of processing.
    int  PT;   // Processor time
}ProcessHeader;
int prcHeadSize = sizeof(ProcessHeader);

// -Returns process size. The pointer indicates the direction of process,
//  more specifically, the process header.
inline int processSize(char* prc_dirc) {
    return charToInt(&prc_dirc[NAME_SIZE]);
}

typedef struct page {
    int number;
    int   size;
    char* info;
}Page;
int pageSize = sizeof(Page);

struct BUFFER {       // -New processes can be allocated just at the end.
    int firstIndexAvailable; // -We'll navigate the buffer with this index.
    int firstProcIndex;
    int spaceAvailable;
    char memory[BUFF_SIZE];
} buffer = {0,0,BUFF_SIZE};

void printBytes(const char* bytes, int len);

// Print the bytes of the buffer organized in its correspondent sections.
void printBufferBytes();

// -Allocates a new process in the buffer.
// -For the time being info = NULL
void newProcess(char name[NAME_SIZE], int numofPages);

// -Allocates process from BUFFER to RAM.
// -This function does not delete data from buffer.
void loadProcess();

void viewRAM();

// Defined as Total_processing_time - Processor_time
void Simulate(char* prc_dirc);

// -Simulates all the process.
void SimulateAll();

int main (int argc, char *argv[])
{
    char name[NAME_SIZE] = {'A', 'B', 'C', 0};
    srand(time(NULL));
    newProcess(name, 3);
    name[0]++;
    newProcess(name, 2);
    name[1]++;
    newProcess(name, 4);
    name[2]++;
    newProcess(name, 5);
    name[0]++;
    newProcess(name, 7);
    name[1]++;
    newProcess(name, 6);
    name[2]++;
    newProcess(name, 8);
    printBufferBytes();
    printf("\n");
    loadProcess();
    loadProcess();
    loadProcess();
    loadProcess();
    loadProcess();
    loadProcess();
    loadProcess();
    viewRAM();
    SimulateAll();
    viewRAM();
    Simulate(&ram[4]);
    viewRAM();
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

    printf("----------------------------BUFFER--------------------"
        "--------\n\n");
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

        for(i += sz_int*2; i-t < size.in; ) {
            printf("  %d  ", j++);
            printBytes(&buffer.memory[i], 4); i += 4;
            printBytes(&buffer.memory[i], 4); i += 4;
            printBytes(&buffer.memory[i], 8); i += 8;
            printf("\n");
        }
        printf("\n");
    }
}

void newProcess(char name[NAME_SIZE], int numofPages) {
    int size = prcHeadSize + pageSize * numofPages, i;
    if(size > buffer.spaceAvailable) { // Not enough space
        printf("\nCould not allocate new process in buffer...\n");
        return;
    }
    if(buffer.firstIndexAvailable==BUFF_SIZE) buffer.firstIndexAvailable = 0;

    // -Setting name and updating current index.
    for(i = 0; i < NAME_SIZE - 1; i++)
        buffer.memory[buffer.firstIndexAvailable++] = name[i];
    buffer.memory[buffer.firstIndexAvailable++] = 0;
    if(buffer.firstIndexAvailable==BUFF_SIZE) buffer.firstIndexAvailable = 0;

    // -Setting size and updating current index.
    IntToChar(size,&buffer.memory[buffer.firstIndexAvailable]);
    buffer.firstIndexAvailable += sz_int;
    if(buffer.firstIndexAvailable==BUFF_SIZE) buffer.firstIndexAvailable = 0;

    // Total process time and processor time
    // TTP
    IntToChar(rand()%15 + 2,&buffer.memory[buffer.firstIndexAvailable]);
    buffer.firstIndexAvailable += sz_int;
    if(buffer.firstIndexAvailable==BUFF_SIZE) buffer.firstIndexAvailable = 0;
    // PT = 0
    IntToChar(0, &buffer.memory[buffer.firstIndexAvailable]);
    buffer.firstIndexAvailable += sz_int;
    if(buffer.firstIndexAvailable==BUFF_SIZE) buffer.firstIndexAvailable = 0;

    // -Allocating pages
    Page bf = {0,pageSize,NULL};
    // Supposing pageSize is a multiple of four.
    for(;bf.number < numofPages; bf.number++){
        // Page number
        memcpy(&buffer.memory[buffer.firstIndexAvailable], &bf.number, sz_int);
        buffer.firstIndexAvailable += sz_int;
        if(buffer.firstIndexAvailable==BUFF_SIZE) buffer.firstIndexAvailable = 0;
        // Page Size
        memcpy(&buffer.memory[buffer.firstIndexAvailable], &bf.size, sz_int);
        buffer.firstIndexAvailable += sz_int;
        if(buffer.firstIndexAvailable==BUFF_SIZE) buffer.firstIndexAvailable = 0;
        // Info
        memcpy(&buffer.memory[buffer.firstIndexAvailable], &bf.info, sz_int*2);
        buffer.firstIndexAvailable += sz_int*2;
        if(buffer.firstIndexAvailable==BUFF_SIZE) buffer.firstIndexAvailable = 0;
    }

    buffer.spaceAvailable -= size; // -Updating space available.
}

// How the process is loaded in RAM?
// | ID | Process header | Pages ... | ...

void loadProcess() {
    // Supposing there is enough space in ram
    IntChar _ID = {ID++};
    ram[currRamLoc++] = _ID.ch[0];
    ram[currRamLoc++] = _ID.ch[1];
    ram[currRamLoc++] = _ID.ch[2];
    ram[currRamLoc++] = _ID.ch[3];
    int prcStart = currRamLoc;
    // Supposing the Buffer is not empty
    IntChar sz; // Process size
    sz.ch[0] = buffer.memory[buffer.firstProcIndex+4];
    sz.ch[1] = buffer.memory[buffer.firstProcIndex+5];
    sz.ch[2] = buffer.memory[buffer.firstProcIndex+6];
    sz.ch[3] = buffer.memory[buffer.firstProcIndex+7];

    memcpy(&ram[currRamLoc], &buffer.memory[buffer.firstProcIndex], sz.in);
    buffer.firstProcIndex += sz.in;
    currRamLoc += sz.in;

    // Processor time
    ram[prcStart + prcHeadSize - 4] = rand()%3 + 1;;
}

void viewRAM() {
    int i, // Runs trough bytes of the ram
        j, // Meant for the copy of contents
        t, // Holds the value of 'i' temporarily
        c; // Counter of cycles
    int pageAmount;
    int ibuff[4];
    char cbuff[5];
    IntChar sz = {0}, tp = {0};
    ProcessHeader ph;

    printf("--------------------------------RAM-------------------------"
        "----------\n");
    printf("|  ID   ||  Process  ||  #Pages  || RAM loc  ||   TPT    |"
        "|    PT    |\n");

    for(c = 0, i = 0; (t=i) + c < currRamLoc; c += sz_int, i += sz.in) {
        // Printing ID
        printBytesAsInt(&ram[i], sz_int, 7);
        i += sz_int;
        // Retrieving process name.
        for(j = 0; j < NAME_SIZE; j++) ph.name[j] = ram[i+j];
        printf("|");
        printCentered(ph.name, 11);
        printf("|");
        i += sz_int;
        // Retrieving process size.
        for(j = 0; j < 4; j++) sz.ch[j] = ram[i+j];
        i += sz_int;
        // Naive way of calculating the number of pages.
        pageAmount = (sz.in - prcHeadSize)/pageSize;
        toString (pageAmount, cbuff);
        ibuff[0] = pageAmount;
        ibuff[1] = t;
        // Total time of processing.
        for(j = 0; j < 4; j++) tp.ch[j] = ram[i+j];
        i += sz_int;
        ibuff[2] = tp.in;
        // Processor time
        for(j = 0; j < 4; j++) tp.ch[j] = ram[i+j];
        i += sz_int;
        ibuff[3] = tp.in;
        printTabularForm(ibuff, _int, 4, 4, NULL, NULL);
        sz.in -= prcHeadSize;
    }
}

void Simulate(char* prc_dirc) {
    int ttp_ind = NAME_SIZE + sz_int, pt_ind = ttp_ind + sz_int;
    int ttp = charToInt(&prc_dirc[ttp_ind]),  // -Getting TTP and PT
        pt  = charToInt(&prc_dirc[pt_ind]);
    ttp -= pt;
    pt = rand()%3 + 1;
    IntToChar(ttp, &prc_dirc[ttp_ind]);
    IntToChar(pt, &prc_dirc[pt_ind]);
}

void SimulateAll() {
    int prc_sz, i;
    char* prc_ptr;
    for(i = sz_int; i < currRamLoc; i += prc_sz + sz_int) {
        prc_sz = charToInt(&ram[i + NAME_SIZE]);
        prc_ptr = &ram[i];
        Simulate(prc_ptr);
    }
}
