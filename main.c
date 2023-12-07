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

int sz_int = sizeof(int);

struct RAM {
    int currFreeRam;    // Current free RAM location
    int first_prc;       // First process index
    int last_prc;       // Last process index
    int spaceAvailable; // Usable space
    char memory[RAM_SIZE];
} ram = {0,0,0,RAM_SIZE};

inline void copyOnRam(char* source, int size) {
    // Warning: No bound checking
    memcpy(&ram.memory[ram.currFreeRam], source, size);
    ram.currFreeRam += size;
}

struct SWAP {
    int currFreeSwap;   // Current free Swap location
    int first_prc;       // First process index
    int last_prc;       // Last process index
    int spaceAvailable; // Usable space
    char memory[SWAP_SIZE];
} swap = {0,0,0,SWAP_SIZE};

inline void copyOnSwap(char* source, int size) {
    // Warning: No bound checking
    memcpy(&swap.memory[swap.currFreeSwap], source, size);
    swap.currFreeSwap += size;
}

int    ID = 0; // Process ID
int sz_ID = sizeof(ID);
inline void setID(char* dir) {
    IntToChar(ID++, dir);
}

typedef struct processheader {
    int  ID;
    char name[NAME_SIZE];
    int  size;     // Size in bytes.
    int  offset;    // Space between processes
    int  TTP;      // Total time of processing.
    int  PT;       // Processor time
    int  previous_prc; // Index of previous process
}ProcessHeader;
int pos_ID    = 0,// Position (in bytes) of the attributes of ProcessHeader
    pos_name  = 4,
    pos_size  = NAME_SIZE + 4,
    pos_offset = NAME_SIZE + 8,
    pos_TTP   = NAME_SIZE + 12,
    pos_PT    = NAME_SIZE + 16,
    pos_previousPrc = NAME_SIZE + 20;
int prcHeadSize = sizeof(ProcessHeader);

// -Returns process ID. The pointer indicates the direction of process,
//  more specifically, the process header.
inline int processID(char* prc_dirc) {
    return charToInt(&prc_dirc[pos_ID]);
}

// -Sets the process ID. The pointer indicates the direction of process,
//  more specifically, the process header.
inline void setPrcssID(char* prc_dirc, int _ID) {
    IntToChar(_ID, &prc_dirc[pos_ID]);
}

// -Copy the process name in 'dest'. The pointer indicates the direction of
//  process, more specifically, the process header.
inline void processName(char* prc_dirc, char* dest) {
    for(int i = 0; i < NAME_SIZE; i++) {
        dest[i] = prc_dirc[pos_name + i];
    }
}

// -Sets the process name. The pointer indicates the direction of process,
//  more specifically, the process header.
inline void setPrcssName(char* prc_dirc, char* name) {
    for(int i = 0; i < NAME_SIZE; i++) {
        prc_dirc[pos_name + i] = name[i];
    }
}

// -Returns process size. The pointer indicates the direction of process,
//  more specifically, the process header.
inline int processSize(char* prc_dirc) {
    return charToInt(&prc_dirc[pos_size]);
}

// -Sets the process size. The pointer indicates the direction of process,
//  more specifically, the process header.
inline void setPrcssSize(char* prc_dirc, int size) {
    IntToChar(size, &prc_dirc[pos_size]);
}

// -Returns process offset. The pointer indicates the direction of process,
//  more specifically, the process header.
inline int processOffset(char* prc_dirc) {
    return charToInt(&prc_dirc[pos_offset]);
}

// -Sets the process offset. The pointer indicates the direction of process,
//  more specifically, the process header.
inline void setPrcssOffset(char* prc_dirc, int offset) {
    IntToChar(offset, &prc_dirc[pos_offset]);
}

// -Returns process TTP. The pointer indicates the direction of process,
//  more specifically, the process header.
inline int processTTP(char* prc_dirc) {
    return charToInt(&prc_dirc[pos_TTP]);
}

// -Sets the process TTP. The pointer indicates the direction of process,
//  more specifically, the process header.
inline void setPrcssTTP(char* prc_dirc, int TTP) {
    IntToChar(TTP, &prc_dirc[pos_TTP]);
}

// -Returns process PT. The pointer indicates the direction of process,
//  more specifically, the process header.
inline int processPT(char* prc_dirc) {
    return charToInt(&prc_dirc[pos_PT]);
}

// -Sets the process PT. The pointer indicates the direction of process,
//  more specifically, the process header.
inline void setPrcssPT(char* prc_dirc, int PT) {
    IntToChar(PT, &prc_dirc[pos_PT]);
}

// -Returns process previous_prc. The pointer indicates the direction of
//  process, more specifically, the process header.
inline int processPrevPrc(char* prc_dirc) {
    return charToInt(&prc_dirc[pos_previousPrc]);
}

// -Sets the process previous process. The pointer indicates the direction of
//  process, more specifically, the process header.
inline void setPrcssPrevPrc(char* prc_dirc, int prevPrc) {
    IntToChar(prevPrc, &prc_dirc[pos_previousPrc]);
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

inline void resetBuffer() {
    buffer.firstIndexAvailable = 0;
    buffer.firstProcIndex = 0;
    buffer.spaceAvailable = BUFF_SIZE;
}

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

// -Allocates process from BUFFER to SWAP.
// -This function does not delete data from buffer.
void loadProcessSwap(int pageNumber);

void viewSwap();

// -Substitutes the process in RAM that starts in the index 'prcRamInd' with
//  the process in SWAP that starts in the index 'prcSwapInd'.
// -This function does not changes the integer attributes of the RAM and SWAP
//  structures.
int swapProcess(int prcRamInd, int prcSwapInd);

// Defined as Total_processing_time - Processor_time
void Simulate(int prcInd);

// -Simulates all the process.
void SimulateAll();

int main (int argc, char *argv[])
{
    char name[NAME_SIZE] = {'A', 'A', 'A', 0};
    srand(time(NULL));
    int i, k;
    for(i = 0, k = 0; i < 15; i++) {
        newProcess(name, 30);
        if((k = i%3) == 0) name[k]++;
        if((k = i%3) == 1) name[k]++;
        if((k = i%3) == 2) name[k]++;
        loadProcess();
        resetBuffer ();
    }
    viewRAM();
    viewSwap();

    for(i = 0; i < 5; i++) {
        SimulateAll();
        viewRAM();
        viewSwap();
    }
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
        printBytes(&buffer.memory[i], sz_int);
        // Getting size of first process.
        size.ch[0] = buffer.memory[i++];
        size.ch[1] = buffer.memory[i++];
        size.ch[2] = buffer.memory[i++];
        size.ch[3] = buffer.memory[i++];

        printf("\nPage |  number   ||   size    ||         info*         |\n");

        for(i += sz_int*4; i-t < size.in; ) {
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

    // -Setting size and updating current index.
    IntToChar(size,&buffer.memory[buffer.firstIndexAvailable]);
    buffer.firstIndexAvailable += sz_int;

    if(buffer.firstIndexAvailable==BUFF_SIZE) buffer.firstIndexAvailable = 0;

    // -Setting offset and updating current index.
    IntToChar(0,&buffer.memory[buffer.firstIndexAvailable]);
    buffer.firstIndexAvailable += sz_int;

    // Total process time and processor time
    // TTP
    IntToChar(rand()%15 + 2,&buffer.memory[buffer.firstIndexAvailable]);
    buffer.firstIndexAvailable += sz_int;

    if(buffer.firstIndexAvailable==BUFF_SIZE) buffer.firstIndexAvailable = 0;

    // PT = 0
    IntToChar(0, &buffer.memory[buffer.firstIndexAvailable]);
    buffer.firstIndexAvailable += sz_int;

    // previous_prc == 0
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
    if(ram.currFreeRam == RAM_SIZE) {
        loadProcessSwap(-1); // RAM full
        return;
    }
    int tmp = RAM_SIZE - ram.currFreeRam; // Space at the end of RAM

    // In case of not be capable of allocate the ID and the process header
    if(prcHeadSize + sz_int > tmp) {
        IntToChar(tmp,  // Updating offset of last process
                  &ram.memory[ram.last_prc + NAME_SIZE + sz_int]);
        ram.currFreeRam = RAM_SIZE;
        loadProcessSwap(-1);
        return;
    }
    // Supposing the Buffer is not empty
    IntChar sz; // Process size
    sz.ch[0] = buffer.memory[buffer.firstProcIndex + NAME_SIZE];
    sz.ch[1] = buffer.memory[buffer.firstProcIndex + NAME_SIZE + 1];
    sz.ch[2] = buffer.memory[buffer.firstProcIndex + NAME_SIZE + 2];
    sz.ch[3] = buffer.memory[buffer.firstProcIndex + NAME_SIZE + 3];

    // There is enough space for ID and header
    IntChar _ID = {ID++};
    ram.memory[ram.currFreeRam++] = _ID.ch[0];
    ram.memory[ram.currFreeRam++] = _ID.ch[1];
    ram.memory[ram.currFreeRam++] = _ID.ch[2];
    ram.memory[ram.currFreeRam++] = _ID.ch[3];
    int prcStart = ram.currFreeRam, pag_num = (sz.in - prcHeadSize)/pageSize;

    // Loading Header
    memcpy( &ram.memory[prcStart],
           &buffer.memory[buffer.firstProcIndex],
           prcHeadSize );
    ram.currFreeRam += prcHeadSize;

    // Processor time
    ram.memory[prcStart + NAME_SIZE + sz_int*3] = rand()%3 + 1;
    // Previous process
    IntToChar(ram.last_prc, &ram.memory[prcStart + NAME_SIZE + sz_int*4]);

    int i; // Allocating pages
    for(i = 0, tmp = prcHeadSize;
        (i < pag_num) && (RAM_SIZE - ram.currFreeRam >= pageSize);
        i++, tmp += pageSize) {
        memcpy( &ram.memory[ram.currFreeRam],
               &buffer.memory[buffer.firstProcIndex + tmp],
               pageSize );
        ram.currFreeRam += pageSize;
    }

    // Not enough space in RAM, loading in SWAP
    if(i < pag_num) {
        loadProcessSwap(i);
        if(RAM_SIZE - ram.currFreeRam > 0) {
            setPrcssOffset(&ram.memory[prcStart], RAM_SIZE - ram.currFreeRam);
        }
        ram.currFreeRam = RAM_SIZE;
    } else {
        buffer.firstProcIndex += sz.in; // Going to the next process in buffer
    }
    ram.last_prc = prcStart; // -This is now the last process.
}

void loadProcessSwap(int pageNumber) {
    // Warning: No bound checking
    // Retrieving process size
    int prc_sz = processSize(&buffer.memory[buffer.firstProcIndex]);
    if(pageNumber >= 0) { // Allocating part of the process
        // -Supposing buffer.firstProcIndex is pointing to the correct page
        int pgs_loc;
        for(pgs_loc = prcHeadSize + pageNumber*pageSize;
            pgs_loc < prc_sz;
            pgs_loc += pageSize) {
            copyOnSwap(&buffer.memory[buffer.firstProcIndex + pgs_loc],pageSize);
            swap.first_prc += pageSize;
        }
        buffer.firstProcIndex += prc_sz;
        return;
    }
    // Allocating the hole process
    // Allocating ID
    IntToChar(ID++,&swap.memory[swap.currFreeSwap]);
    swap.currFreeSwap += sz_int;

    int prcStart = swap.currFreeSwap; // Marking the beginning of the process
    //printf("Process start = %d\n", prcStart);

    memcpy(&swap.memory[prcStart],&buffer.memory[buffer.firstProcIndex], prc_sz);
    buffer.firstProcIndex += prc_sz;
    swap.currFreeSwap += prc_sz;

    // Processor time
    swap.memory[prcStart + NAME_SIZE + sz_int*3] = rand()%3 + 1;
    // Previous process
    IntToChar(swap.last_prc, &swap.memory[prcStart + NAME_SIZE + sz_int*4]);
    swap.last_prc = prcStart; // -This is now the last process.
    /*printf("offset in loadProcessSwap = %d\n",
           processOffset(&swap.memory[prcStart]));*/
}

void viewRAM() {
    int i, // Runs trough bytes of the ram
        j, // Meant for the copy of contents
        t; // Holds the value of 'i' temporarily
    int pageAmount;
    int offset;
    int ibuff[4];
    char cbuff[5];
    IntChar sz = {0}, tp = {0};
    ProcessHeader ph;

    printf("\n--------------------------------RAM-------------------------"
        "----------\n");
    printf("|  ID   ||  Process  ||  #Pages  || RAM loc  ||   TPT    |"
        "|    PT    |\n");

    // i will run over the ram.
    for(i = ram.first_prc; (t=i) < ram.currFreeRam; i += sz.in) {
        // Printing ID
        printBytesAsInt(&ram.memory[i], sz_int, 7);
        i += sz_int;

        offset = processOffset(&ram.memory[i]); // Retrieving offset

        // Retrieving process name.
        for(j = 0; j < NAME_SIZE; j++) ph.name[j] = ram.memory[i+j];
        printf("|");
        printCentered(ph.name, 11);
        printf("|");
        i += NAME_SIZE;
        // Retrieving process size.
        for(j = 0; j < 4; j++) sz.ch[j] = ram.memory[i+j];
        i += sz_int*2; // Skipping the size and the offset
        // Naive way of calculating the number of pages.
        pageAmount = (sz.in - prcHeadSize)/pageSize;
        toString(pageAmount, cbuff);
        ibuff[0] = pageAmount;
        ibuff[1] = t;
        // Total time of processing.
        for(j = 0; j < 4; j++) tp.ch[j] = ram.memory[i+j];
        i += sz_int;
        ibuff[2] = tp.in;
        // Processor time
        for(j = 0; j < 4; j++) tp.ch[j] = ram.memory[i+j];
        i += sz_int;
        ibuff[3] = tp.in;
        printTabularForm(ibuff, _int, 4, 4, NULL, NULL);
        i += sz_int;
        sz.in -= prcHeadSize;
        sz.in += offset;
    }
}

void viewSwap() {
    int i, // Runs trough bytes of the ram
        j, // Meant for the copy of contents
        t; // Holds the value of 'i' temporarily
    int pageAmount;
    int offset;
    int ibuff[4];
    char cbuff[5];
    IntChar sz = {0}, tp = {0};
    ProcessHeader ph;

    printf("\n--------------------------------Swap-------------------------"
        "----------\n");
    printf("|  ID   ||  Process  ||  #Pages  || SWAP loc ||   TPT    |"
        "|    PT    |\n");

    // i will run over the swap.
    /*printf("swap.first_prc = %d\n"
           "swap.currFreeSwap = %d\n", swap.first_prc,swap.currFreeSwap);*/
    for(i = swap.first_prc; (t=i) < swap.currFreeSwap; i += sz.in) {
        // Printing ID
        printBytesAsInt(&swap.memory[i], sz_int, 7);
        //printf("i == %d\n", i);
        i += sz_int;
        //printf("i == %d\n", i);

        offset = processOffset(&swap.memory[i]); // Retrieving offset

        // Retrieving process name.
        for(j = 0; j < NAME_SIZE; j++) ph.name[j] = swap.memory[i+j];
        printf("|");
        printCentered(ph.name, 11);
        printf("|");
        i += NAME_SIZE;
        // Retrieving process size.
        for(j = 0; j < 4; j++) sz.ch[j] = swap.memory[i+j];
        //printf("%d", sz.in);
        i += sz_int*2; // Skipping the size and the offset
        // Naive way of calculating the number of pages.
        pageAmount = (sz.in - prcHeadSize)/pageSize;
        toString(pageAmount, cbuff);
        ibuff[0] = pageAmount;
        ibuff[1] = t;
        // Total time of processing.
        for(j = 0; j < 4; j++) tp.ch[j] = swap.memory[i+j];
        i += sz_int;
        ibuff[2] = tp.in;
        // Processor time
        for(j = 0; j < 4; j++) tp.ch[j] = swap.memory[i+j];
        i += sz_int;
        ibuff[3] = tp.in;
        printTabularForm(ibuff, _int, 4, 4, NULL, NULL);
        i += sz_int;
        sz.in -= prcHeadSize;
        sz.in += offset; //printf("offset = %d\n", offset);
    }
}

int swapProcess(int prcRamInd, int prcSwapInd) {
    // -Skipping ID

    char* prcInRam = &ram.memory[prcRamInd];
    char* prevprcR = NULL;
    int   prevPrcOffset = 0;
    // Verifying prcInRam is not the first process in RAM
    if(prcRamInd - sz_int != ram.first_prc) {
        prevprcR = &ram.memory[processPrevPrc(prcInRam)];
        prevPrcOffset = processOffset(prevprcR);
    }
    char* prcInSwap = &swap.memory[prcSwapInd];
    int spaceAvailable = processSize(prcInRam) + processOffset(prcInRam) +
                         prevPrcOffset;
    int swapPrcSz = processSize(prcInSwap);

    // -Verifying if there is enough space for the process and the ID.
    printf("spaceAvailable = %d,swapPrcSz = %d\n", spaceAvailable, swapPrcSz);
    if(spaceAvailable >= swapPrcSz) {
        if(prcRamInd - sz_int == ram.first_prc) {
            prcInRam = &ram.memory[0];
            prcInSwap = &swap.memory[prcSwapInd - 4];
            memcpy(prcInRam, prcInSwap, swapPrcSz + sz_int);
            setPrcssOffset(prcInRam + sz_int,
                          spaceAvailable - swapPrcSz + prcRamInd - sz_int);
            ram.first_prc = 0;
        } else {
            prcInRam = &ram.memory[prcRamInd - prevPrcOffset - sz_int];
            prcInSwap = &swap.memory[prcSwapInd - sz_int];
            memcpy(prcInRam, prcInSwap, swapPrcSz + sz_int);
            setPrcssOffset(prcInRam + sz_int, spaceAvailable - swapPrcSz);
        }
        return 0;
    }
    if(prcRamInd - sz_int == ram.first_prc) {
        ram.first_prc+=processSize(prcInRam) + processOffset(prcInRam) + sz_int;
    } else {
        setPrcssOffset(prevprcR, spaceAvailable + sz_int);
    }
    return -1;
}

void Simulate(int prcInd) {
    int ttp_ind = NAME_SIZE + sz_int*2, pt_ind = ttp_ind + sz_int;
    int ttp = charToInt(&ram.memory[prcInd + ttp_ind]),// -Getting TTP and PT
        pt  = charToInt(&ram.memory[prcInd + pt_ind]);
    ttp -= pt;
    if(ttp <= 0) {
        if(swapProcess(prcInd, swap.first_prc + sz_int) == 0) {
            swap.first_prc += processSize(&swap.memory[swap.first_prc + sz_int]) +
                         processOffset(&swap.memory[swap.first_prc + sz_int]) + sz_int;
        }
        return;
    }
    pt = rand()%3 + 1;
    IntToChar(ttp, &ram.memory[prcInd + ttp_ind]);
    IntToChar(pt, &ram.memory[prcInd + pt_ind]);
}

void SimulateAll() {
    int prc_sz,prc_offset, i;

    for(i = ram.first_prc + sz_int;
        i < ram.currFreeRam;
        i += prc_sz + prc_offset + sz_int) {

        prc_sz = charToInt(&ram.memory[i + NAME_SIZE]);
        prc_offset = processOffset(&ram.memory[i]);
        Simulate(i);
    }
}
