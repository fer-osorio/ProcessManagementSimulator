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

// Size of units of memory == 16 bytes

#define SWAP_SIZE 10240 // 10 Kb
#define RAM_SIZE  5120  // 5  Kb
#define BUFF_SIZE 1024  // 1  Kb
#define NAME_SIZE 4

typedef enum _boolean {
    false,
    true
} boolean;

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

inline void resetRam() { // Returns ram to original state.
    ram.currFreeRam = 0;
    ram.first_prc = 0;
    ram.last_prc = 0;
    ram.spaceAvailable = RAM_SIZE;
}

inline boolean ramIsEmpty() {
    if(ram.spaceAvailable == RAM_SIZE) return true;
    return false;
}

struct SWAP {
    int currFreeSwap;   // Current free Swap location
    int first_prc;       // First process index
    int last_prc;       // Last process index
    int pagesOffset;     // Offset from incomplete allocation of process in RAM
    int spaceAvailable; // Usable space
    char memory[SWAP_SIZE];
} swap = {0,0,0,0,SWAP_SIZE};

inline void copyOnSwap(char* source, int size) {
    // Warning: No bound checking
    memcpy(&swap.memory[swap.currFreeSwap], source, size);
    swap.currFreeSwap += size;
    swap.spaceAvailable -= size;
}

inline void resetSwap() { // Returns swap to original state
    swap.currFreeSwap = 0;
    swap.first_prc = 0;
    swap.last_prc = 0;
    swap.pagesOffset = 0;
    swap.spaceAvailable = SWAP_SIZE;
}

inline boolean noProcessInSwap() {
    if(swap.spaceAvailable == SWAP_SIZE - swap.pagesOffset) return true;
    return false;
}

inline boolean swapIsEmpty() {
    if(swap.spaceAvailable == SWAP_SIZE) return true;
    return false;
}

int    ID = 0; // Process ID
int sz_ID = sizeof(ID);
inline void setID(char* dir) {
    IntToChar(ID++, dir);
}

typedef struct processheader { // Process information
    int  ID;
    char name[NAME_SIZE];
    int  size;     // Size in bytes
    int  offset;    // Space between processes
    int  TTP;      // Total time of processing
    int  PT;       // Processor time
    int  previous_prc; // Index of previous process
    int  next_prc; // Index of the next process
}ProcessHeader;
int pos_ID    = 0,// Position (in bytes) of the attributes of ProcessHeader
    pos_name  = 4,
    pos_size  = NAME_SIZE + 4,
    pos_offset = NAME_SIZE + 8,
    pos_TTP   = NAME_SIZE + 12,
    pos_PT    = NAME_SIZE + 16,
    pos_previousPrc = NAME_SIZE + 20,
    pos_nextPrc = NAME_SIZE + 24;
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

// -Returns process next_prc. The pointer indicates the direction of
//  process, more specifically, the process header.
inline int processNextPrc(char* prc_dirc) {
    return charToInt(&prc_dirc[pos_nextPrc]);
}

// -Sets the process next process. The pointer indicates the direction of
//  process, more specifically, the process header.
inline void setPrcssNextPrc(char* prc_dirc, int nextPrc) {
    IntToChar(nextPrc, &prc_dirc[pos_nextPrc]);
}

// Writes the information of the header process into an array of chars
inline void setProcessHeader(
    int id,
    char name[NAME_SIZE],
    int size,
    int offset,
    int TTP,
    int PT,
    int prev_prc,
    int next_prc,
    char* prc_dirc // Pointer to the place we want to write the header
)   {
    setPrcssID(prc_dirc, id);
    setPrcssName(prc_dirc, name);
    setPrcssSize(prc_dirc, size);
    setPrcssOffset(prc_dirc, offset);
    setPrcssTTP(prc_dirc, TTP);
    setPrcssPT(prc_dirc, PT);
    setPrcssPrevPrc(prc_dirc, prev_prc);
    setPrcssNextPrc(prc_dirc, next_prc);
}

// -prc_dirc points to the beginning of a process written into an array of
//  char's. These function creates an instance of 'ProcessHeader' structure
//  using that array of char's
inline ProcessHeader getProcessHeader(char* prc_dirc) {
    ProcessHeader ph;

    ph.ID    = processID(prc_dirc);
    processName(prc_dirc, ph.name);
    ph.size  = processSize(prc_dirc);
    ph.offset = processOffset(prc_dirc);
    ph.TTP   = processTTP(prc_dirc);
    ph.PT    = processPT(prc_dirc);
    ph.previous_prc = processPrevPrc(prc_dirc);
    ph.next_prc = processNextPrc(prc_dirc);

    return ph;
}

// -Tells us if the last process is completely allocated in ram or if part of
//  it is in swap.
inline boolean ramLastProcessIncomplete() {
    char* last = &ram.memory[ram.last_prc];
    if(ram.last_prc + processSize(last) + processOffset(last) > RAM_SIZE)
        return true;
    return false;
}

typedef struct page {
    int number;
    int   size;
    char* info;
}Page;
int pageSize = sizeof(Page);

struct BUFFER {             // -New processes can be allocated just at the end.
    int firstIndexAvailable; // -We'll navigate the buffer with this index.
    int firstProcIndex;      // -first process index
    int spaceAvailable;
    char memory[BUFF_SIZE];
} buffer = {0,0,BUFF_SIZE};

inline void resetBuffer() {
    buffer.firstIndexAvailable = 0;
    buffer.firstProcIndex = 0;
    buffer.spaceAvailable = BUFF_SIZE;
}

// -Print each byte of an array of bytes (char's).
void printBytes(const char* bytes, int len);

// Print the bytes of the buffer organized in its correspondent sections.
void printBufferBytes();

// -Allocates a new process in the buffer.
void newProcess(char name[NAME_SIZE], int numofPages);

// -Allocates process from BUFFER to RAM.
// -This function does not delete data from buffer.
void loadProcess();

// -Terminates a process
void terminateProcess(int prcInd);

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

// -Same as swapProcess but with the last process of ram.
int swapWithLastRamProcess(int prcRamInd);

// Defined as Total_processing_time - Processor_time
void Simulate(int prcInd);

// -Simulates all the process.
void SimulateAll();

//////////////////////////////////////////////////////////////////////////////
int main (int argc, char *argv[])
{
    char name[NAME_SIZE] = {'A', 'A', 'A', 0};
    srand(time(NULL));
    int i, k;
    for(i = 0, k = 0; i < 15; i++) {
        //newProcess(name, i+2);
        newProcess(name, 30);
        if((k = i%3) == 0) name[k]++;
        if((k = i%3) == 1) name[k]++;
        if((k = i%3) == 2) name[k]++;
        loadProcess();
        resetBuffer ();
    }
    //printBufferBytes();
    viewRAM();
    viewSwap();

    while(ramIsEmpty() == false) {
        SimulateAll();
        viewRAM();
        viewSwap();
    }

    return EXIT_SUCCESS;
}
//////////////////////////////////////////////////////////////////////////////

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
    int size = 0;
    int i = 0; // We'll use i to run trough the buffer memory.
    int j = 0; // Page counter,
    int k = 0; // Process counter.

    printf("----------------------------BUFFER--------------------"
        "--------\n\n");
    for(int t = 0; i < buffer.firstIndexAvailable; i += size, t++) {
        printf("Process %d:-----------------------------------------------"
        "-----\nID:               ", t);
        // Printing ID of process.
        printBytes(&buffer.memory[i + pos_ID], sz_int);
        // Printing name of process.
        printf("\nName:             ");
        printBytes(&buffer.memory[i + pos_name], NAME_SIZE);
        // Printing size
        printf("\nSize:             ");
        printBytes(&buffer.memory[i + pos_size], sz_int);
        // Printing offset
        printf("\nOffset:           ");
        printBytes(&buffer.memory[i + pos_offset], sz_int);
        // Printing TTP
        printf("\nTTP:              ");
        printBytes(&buffer.memory[i + pos_TTP], sz_int);
        // Printing PT
        printf("\nPT:               ");
        printBytes(&buffer.memory[i + pos_PT], sz_int);
        // Printing previous process
        printf("\nPrevious process: ");
        printBytes(&buffer.memory[i + pos_previousPrc], sz_int);
        // Printing next process
        printf("\nNext process:     ");
        printBytes(&buffer.memory[i + pos_nextPrc], sz_int);

        // Getting size of process.
        size = processSize(&buffer.memory[i]);

        printf("\nPage |  number   ||   size    ||         info*         |\n");

        for(k = i + prcHeadSize, j = 0 ; k-i < size; k += pageSize) {
            printf("  %d  ", j++);
            printBytes(&buffer.memory[k], 4);
            printBytes(&buffer.memory[k + sz_int], 4);
            printBytes(&buffer.memory[k + (sz_int<<1)], 8);
            printf("\n");
        }
        printf("\n");
    }
}

void newProcess(char name[NAME_SIZE], int numofPages) {
    int size = prcHeadSize + pageSize*numofPages,
        // -Saving the integer attributes of the buffer
        prevFirstIndexAvailable = buffer.firstIndexAvailable,
        prevSpaceAvailable = buffer.spaceAvailable,
        prevFirstProcess = buffer.firstProcIndex;

    // -Checking if there is enough space for the header at the end of the
    //  buffer
    if(BUFF_SIZE - buffer.firstIndexAvailable < prcHeadSize) {
        buffer.spaceAvailable -= BUFF_SIZE - buffer.firstIndexAvailable;
        buffer.firstIndexAvailable = 0;
    }
    if(size > buffer.spaceAvailable) { // Not enough space
        printf("\nCould not allocate new process in buffer...\n");
        // -Returning to original state.
        buffer.firstIndexAvailable = prevFirstIndexAvailable;
        buffer.spaceAvailable = prevSpaceAvailable;
        buffer.firstProcIndex =  prevFirstProcess;
        return;
    }

    setProcessHeader(0,  // ID
                     name,
                     size,
                     0,  // Offset
                     rand()%15+2, // TTP
                     0,  // PT
                     -1, // Previous process
                     -1, // Next process
                     &buffer.memory[buffer.firstIndexAvailable]);

    //char *this_prc = &buffer.memory[buffer.firstIndexAvailable];
    buffer.firstIndexAvailable += prcHeadSize; // Updating index available

    // -Allocating pages
    Page bf = {0,pageSize,NULL};
    // Supposing pageSize is a multiple of four.
    for(;bf.number < numofPages; bf.number++){
        // -Taking advantage that the size of the buffer is a multiple of
        //  16 (== pageSize) and 32 (== prcHeadSize)
        // -Page number
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
    //setPrcssNextPrc(this_prc, buffer.firstIndexAvailable);
    buffer.spaceAvailable -= size; // -Updating space available
}

void loadProcess() {
    if(ram.currFreeRam == RAM_SIZE) {
        loadProcessSwap(-1); // RAM full
        return;
    }
    int tmp = RAM_SIZE - ram.currFreeRam; // Space at the end of RAM

    // In case of not be capable of allocate the the process header
    if(prcHeadSize > tmp) {
        setPrcssOffset(&ram.memory[ram.last_prc], tmp);
        ram.currFreeRam = RAM_SIZE;
        ram.spaceAvailable -= tmp;
        loadProcessSwap(-1);
        return;
    }
    ProcessHeader ph = getProcessHeader(&buffer.memory[buffer.firstProcIndex]);
    // ID
    ph.ID = ID++;
    // Processor time
    ph.PT = rand()%3 + 1;

    int prcStart = ram.currFreeRam;
    // Condition to determine if this is the first process allocated in ram
    if(ram.spaceAvailable != RAM_SIZE) {
        setPrcssNextPrc(&ram.memory[ram.last_prc], prcStart);
        ph.previous_prc = ram.last_prc;
    }
    memcpy(&ram.memory[ram.currFreeRam], &ph, prcHeadSize);

    ram.currFreeRam += prcHeadSize;
    ram.spaceAvailable -= prcHeadSize;

    int i, pag_num = (ph.size - prcHeadSize)/pageSize; // Allocating pages
    for(i = 0, tmp = prcHeadSize;
        (i < pag_num) && (RAM_SIZE - ram.currFreeRam >= pageSize);
        i++, tmp += pageSize) {
        memcpy( &ram.memory[ram.currFreeRam],
               &buffer.memory[buffer.firstProcIndex + tmp],
               pageSize );
        ram.currFreeRam += pageSize;
        ram.spaceAvailable -= pageSize;
    }
    // Not enough space in RAM, loading in SWAP
    if(i < pag_num) {
        loadProcessSwap(i);
        if(RAM_SIZE - ram.currFreeRam > 0) {
            setPrcssOffset(&ram.memory[prcStart], RAM_SIZE - ram.currFreeRam);
            ram.spaceAvailable -= RAM_SIZE - ram.currFreeRam;
        }
        ram.currFreeRam = RAM_SIZE;
    } else {
        buffer.firstProcIndex += ph.size; // Going to the next process in buffer;
    }
    ram.last_prc = prcStart; // -This is now the last process.
}

void terminateProcess(int prcInd) {
    char* this_prc = &ram.memory[prcInd]; // This process
    char* prev = NULL; // Previous process
    char* next = NULL; // Next process
    // Space in ram that's going to be freed
    int freedSpace = processSize(this_prc) + processOffset(this_prc);

    if(prcInd != ram.first_prc) { // Not the first one
        prev = &ram.memory[processPrevPrc(this_prc)];
        // Increasing previous process offset
        setPrcssOffset(prev, processOffset(prev) + freedSpace);
        // Connecting previous process with next process
        setPrcssNextPrc(prev, processNextPrc(this_prc));
    } else {
        // It is the first process
        ram.first_prc += freedSpace; // Just moving the to the next process
        ram.spaceAvailable += freedSpace;
        if(ramIsEmpty() == true) resetRam();
    }
    if(prcInd != ram.last_prc) { // Is not the last process
        next = &ram.memory[processNextPrc(this_prc)];
        setPrcssPrevPrc(next, processPrevPrc(this_prc));
    }
}

void loadProcessSwap(int pageNumber) {
    ProcessHeader ph = getProcessHeader(&buffer.memory[buffer.firstProcIndex]);
    if(pageNumber >= 0) { // Process couldn't be allocated completely in RAM
        // -Supposing buffer.firstProcIndex is pointing to the correct page
        int incompletePrcSz = ph.size - prcHeadSize - pageNumber*pageSize;
        // Incomplete process size
        if(incompletePrcSz > swap.spaceAvailable) { // Checking space
            printf("\nNot enough space in swap to allocate the process "
                   "%s.\n", ph.name);
            return;
        }
        if(swap.first_prc > 0 && incompletePrcSz > swap.first_prc) {
            printf("\nCan not allocate the process %s cause this would "
                   "overwrite the first process in swap.\n", ph.name);
            return;
        }
        int pgs_loc = prcHeadSize + pageNumber*pageSize;

        memcpy(swap.memory,&buffer.memory[buffer.firstProcIndex + pgs_loc],
               incompletePrcSz); // Copying pages

        if(swap.first_prc == 0) swap.first_prc = incompletePrcSz;
        swap.pagesOffset = incompletePrcSz;
        swap.spaceAvailable -= incompletePrcSz;
        buffer.firstProcIndex += ph.size;
        buffer.firstProcIndex %= BUFF_SIZE;
        return;
    }
    int tmp = SWAP_SIZE - swap.currFreeSwap; // Space at the end of SWAP
    if(ph.size > swap.spaceAvailable) {
        printf("\nNot enough space for the allocation of process in Swap.\n");
        return;
    }
    // In case of not be capable of allocate the the process header
    if(prcHeadSize > tmp) {
        setPrcssOffset(&swap.memory[swap.last_prc], tmp);
        swap.currFreeSwap = swap.pagesOffset;
        swap.spaceAvailable -= tmp;
        return;
    }
    if(ph.size > swap.spaceAvailable) {
        printf("\nNot enough space for the allocation of process in Swap.\n");
        return;
    }
    if(swap.currFreeSwap < swap.first_prc &&
       swap.currFreeSwap + ph.size > swap.first_prc) {
            printf("\nSomething went wrong; can not overwrite the first "
                   "process in order to allocate %s\n", ph.name);
            return;
    }
    // ID
    ph.ID = ID++;
    // Processor time
    ph.PT = rand()%3 + 1;

    int prcStart = swap.currFreeSwap;
    // Condition to determine if this is the first process allocated in swap
    if(swap.spaceAvailable != SWAP_SIZE - swap.pagesOffset) {
        setPrcssNextPrc(&swap.memory[swap.last_prc], prcStart);
        ph.previous_prc = swap.last_prc;
    }
    memcpy(&swap.memory[swap.currFreeSwap], &ph, prcHeadSize);
    swap.currFreeSwap += prcHeadSize;
    swap.spaceAvailable -= prcHeadSize;

    swap.last_prc = prcStart; // -This is now the last process.
    int i, pag_num = (ph.size - prcHeadSize)/pageSize; // Allocating pages
    for(i = 0, tmp = prcHeadSize;
        (i < pag_num) && (SWAP_SIZE - swap.currFreeSwap >= pageSize);
        i++, tmp += pageSize) {
        memcpy( &swap.memory[swap.currFreeSwap],
               &buffer.memory[buffer.firstProcIndex + tmp],
               pageSize );
        swap.currFreeSwap += pageSize;
        swap.spaceAvailable -= pageSize;
    }
    if(i < pag_num) { // Allocating the pages could not be allocated before.
        swap.currFreeSwap = swap.pagesOffset;
        for(; i < pag_num; i++, tmp += pageSize) {
            memcpy( &swap.memory[swap.currFreeSwap],
               &buffer.memory[buffer.firstProcIndex + tmp],
               pageSize );
        }
    }
    buffer.firstProcIndex += ph.size; // Going to the next process in buffer;
}

void viewRAM() {
    int i, // Runs trough bytes of the ram
        t; // Holds the value of 'i' temporarily
    int pageAmount;
    int ibuff[4];
    char cbuff[5];
    ProcessHeader ph;

    printf("\n--------------------------------RAM-------------------------"
        "----------\n");
    printf("|  ID   ||  Process  ||  #Pages  || RAM loc  ||   TPT    |"
        "|    PT    |\n");

    if(ramIsEmpty() == true) return;

    // i will run over the ram.
    for(i = ram.first_prc; (t=i) >= 0; i = ph.next_prc) {
        ph = getProcessHeader(&ram.memory[i]);
        // Printing ID
        toString(ph.ID, cbuff);
        printCentered(cbuff,8);
        // Printing name
        printf("||");
        printCentered(ph.name, 11);
        printf("|");

        pageAmount = (ph.size - prcHeadSize)/pageSize;
        toString(pageAmount, cbuff);
        ibuff[0] = pageAmount;
        ibuff[1] = t;
        ibuff[2] = ph.TTP;
        ibuff[3] = ph.PT;
        printTabularForm(ibuff, _int, 4, 4, NULL, NULL);
    }
}

void viewSwap() {
    int i, // Runs trough bytes of the ram
        t; // Holds the value of 'i' temporarily
    int pageAmount;
    int ibuff[4];
    char cbuff[5];
    ProcessHeader ph;

    printf("\n--------------------------------Swap-------------------------"
        "----------\n");
    printf("|  ID   ||  Process  ||  #Pages  || SWAP loc ||   TPT    |"
        "|    PT    |\n");

    if(swapIsEmpty() == true) return;

    // i will run over the ram.
    for(i = swap.first_prc; (t=i) >= 0; i = ph.next_prc) {
        ph = getProcessHeader(&swap.memory[i]);
        // Printing ID
        toString(ph.ID, cbuff);
        printCentered(cbuff,8);
        // Printing name
        printf("||");
        printCentered(ph.name, 11);
        printf("|");

        pageAmount = (ph.size - prcHeadSize)/pageSize;
        toString(pageAmount, cbuff);
        ibuff[0] = pageAmount;
        ibuff[1] = t;
        ibuff[2] = ph.TTP;
        ibuff[3] = ph.PT;
        printTabularForm(ibuff, _int, 4, 4, NULL, NULL);
    }
}

int swapProcess(int prcRamInd, int prcSwapInd) {
    char* prcInRam = &ram.memory[prcRamInd]; // Process in ram
    int next = processNextPrc(prcInRam), // Previous process index
        prev = processPrevPrc(prcInRam); // Next process offset
    char* prevprcR = NULL;  // Pointer to previous process in RAM
    int   prevPrcOffset = 0; // Previous process offset

    // Verifying prcInRam is not the first process in RAM
    if(processPrevPrc(prcInRam) >= 0) {
        prevprcR = &ram.memory[processPrevPrc(prcInRam)];
        prevPrcOffset = processOffset(prevprcR);
    }
    char* prcInSwap = &swap.memory[prcSwapInd];
    int spaceAvailable = processSize(prcInRam) + processOffset(prcInRam) +
                         prevPrcOffset;
    int swapPrcSz = processSize(prcInSwap);

    // -Verifying if there is enough space for the process.
    if(spaceAvailable >= swapPrcSz) {
        if(prcRamInd == ram.first_prc) { // Is the first process
            prcInRam = &ram.memory[0]; // Moving to the beginning
            prcRamInd = 0;
            memcpy(prcInRam, prcInSwap, swapPrcSz); // Copying on ram
            // Connecting swapped process
            setPrcssOffset(prcInRam,
                          spaceAvailable - swapPrcSz + prcRamInd);
            setPrcssNextPrc(prcInRam, next);
            setPrcssPrevPrc(prcInRam, -1);
            if(next >= -1) setPrcssPrevPrc(&ram.memory[next], prcRamInd);
            ram.spaceAvailable -= prcRamInd;
            ram.first_prc = 0;
        } else { // Is not the first process
            // Using the offset of the previous process
            prcInRam = &ram.memory[prcRamInd - prevPrcOffset];
            prcRamInd = prcRamInd - prevPrcOffset;
            memcpy(prcInRam, prcInSwap, swapPrcSz); // Copying on ram
            // Connecting swapped process
            setPrcssOffset(prcInRam, spaceAvailable - swapPrcSz);
            setPrcssNextPrc(prcInRam, next);
            setPrcssPrevPrc(prcInRam, prev);
            if(next >= -1) setPrcssPrevPrc(&ram.memory[next], prcRamInd);
        }
        return prcRamInd;
    }
    return -1; // -Indicating failure
}

int swapWithLastRamProcess(int prcRamInd) {
    char* prcInRam = &ram.memory[prcRamInd]; // Process in ram
    int next = processNextPrc(prcInRam), // next process index
        prev = processPrevPrc(prcInRam); // previous process index
    char* prevprcR = NULL;  // Pointer to previous process in ram
    int   prevPrcOffset = 0; // Previous process offset
    // Verifying prcInRam is not the first process in RAM
    if(processPrevPrc(prcInRam) >= 0) {
        prevprcR = &ram.memory[processPrevPrc(prcInRam)];
        prevPrcOffset = processOffset(prevprcR);
    }
    char* last_prc = &ram.memory[ram.last_prc];

    // Index of the previous process of the last process
    int prevlastInd = processPrevPrc(last_prc);
    char* prevlast = NULL; // Pointer to previous process of the last process
    // Verifying if last process has a previous process
    if(prevlastInd != -1) prevlast = &ram.memory[prevlastInd];

    int spaceAvailable = processSize(prcInRam) + processOffset(prcInRam) +
                         prevPrcOffset;
    int lastPrcSz = processSize(last_prc);
    int szPartInRam = RAM_SIZE - ram.last_prc; // Size of the part in ram
    int szPartInSwap = lastPrcSz - szPartInRam; // Size of the part in swap

    if(szPartInRam > lastPrcSz) { // The process is entirely in ram
        szPartInRam = lastPrcSz;
        szPartInSwap = 0;
    }

    // -Verifying if there is enough space for the process.
    if(spaceAvailable >= lastPrcSz) {
        if(prcRamInd == ram.first_prc) { // Is first process
            prcInRam = &ram.memory[0];  // Moving to the beginning
            prcRamInd = 0;
            // Copying part in ram
            memcpy(prcInRam, last_prc, szPartInRam);
            // Copying part in swap
            memcpy(prcInRam + szPartInRam, swap.memory, szPartInSwap);
            // Conecting
            setPrcssOffset(prcInRam,
                          spaceAvailable - lastPrcSz + prcRamInd);
            setPrcssNextPrc(prcInRam, next);
            setPrcssPrevPrc(prcInRam, -1);
            if(next >= -1) setPrcssPrevPrc(&ram.memory[next], prcRamInd);
            ram.spaceAvailable -= prcRamInd;
            ram.first_prc = 0;
        } else { // Is not first process
            // Using previous process offset
            prcInRam = &ram.memory[prcRamInd - prevPrcOffset];
            prcRamInd = prcRamInd - prevPrcOffset;
            // Copying part in ram
            memcpy(prcInRam, last_prc, szPartInRam);
            // Copying part in swap
            memcpy(prcInRam + szPartInRam, swap.memory, szPartInSwap);
            // Connecting
            setPrcssOffset(prcInRam, spaceAvailable - lastPrcSz);
            setPrcssNextPrc(prcInRam, next);
            setPrcssPrevPrc(prcInRam, prev);
            if(next >= -1) setPrcssPrevPrc(&ram.memory[next], prcRamInd);
        }
        ram.last_prc = prevlastInd; // Updating last index
        if(prevlast != NULL) {
            setPrcssNextPrc(prevlast, -1);
            setPrcssOffset(prevlast, 0);
        }
        return prcRamInd;
    }
    return -1; // Indicating failure
}

void Simulate(int prcInd) {
    char* this = &ram.memory[prcInd];
    int ttp = processTTP(&ram.memory[prcInd]),// -Getting TTP and PT
        pt  = processPT(&ram.memory[prcInd]),
        sz  = processSize(this) + processOffset(this);
    ttp -= pt; // Updating total process time
    if(ttp <= 0) { // In this case we swap/terminate the process
        if(ramLastProcessIncomplete() == true) { // Try to swap with last prc
            if(swapWithLastRamProcess(prcInd) >= 0) {}
            else terminateProcess(prcInd);
            return;
        }
        if(noProcessInSwap() == false) { // Swap with first process in swap
            if(swapProcess(prcInd, swap.first_prc) >= 0) {
                swap.spaceAvailable+=processSize(&swap.memory[swap.first_prc])+
                                   processOffset(&swap.memory[swap.first_prc]);
                swap.first_prc += processSize(&swap.memory[swap.first_prc]) +
                             processOffset(&swap.memory[swap.first_prc]);
                if(swapIsEmpty() == true) resetSwap();
            } else {
                terminateProcess(prcInd);
            }
        } else { // No process in swap, terminating process
            terminateProcess(prcInd);
        }
        return;
    }
    pt = rand()%3 + 1; // Updating processor time
    setPrcssPT(&ram.memory[prcInd], pt);
    setPrcssTTP(&ram.memory[prcInd], ttp);
}

void SimulateAll() {
    int i;
    for(i = ram.first_prc; i >= 0; i = processNextPrc(&ram.memory[i])) {
        if(processNextPrc(&ram.memory[i]) != -1) // If is not last process
            Simulate(i);
        else if(processSize(&ram.memory[i]) + processOffset(&ram.memory[i]) + i
                <= RAM_SIZE)
                Simulate(i);
    }
}
