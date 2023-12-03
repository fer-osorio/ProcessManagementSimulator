#include<stdlib.h>
#include<stdio.h>
#include"PrintArray.h"

// -Length of string.
int len(const char str[]) {
    if(str == NULL) return 0; // -The null string and the empty string has
    int l = -1;               //  have zero length.
    while(str[++l] != 0) {}
    return l;
}

// -Maximum length of a family of standard c-strings.
int maxLen(const char* strArray[], int numStrs) {
    if(strArray == NULL) return 0;
    int ml = len(strArray[0]), i = 0;
    while(++i < numStrs) if(ml < len(strArray[i])) ml = len(strArray[i]);
    return ml;
}

// -Prints the character 'ch' a 'length' amount of times.
void printCharacters(char ch, int length) {
    if(length > 0) for(int i = 0; i < length; i++) printf("%c", ch);
}

// -Printing a 'lenght' amount of spaces.
void printSpaces(int length) {
    if(length > 0) for(int i = 0; i < length; i++) printf(" ");
}

// -Prints 'str' centrally inside a imaginary column of length 'spaceAvailable'
// -If spaceAvailable <= len(str), then the str is printed normally.
void printCentered(const char str[], int spaceAvaible) {
    int strlen = len(str), offset = 0;
    // -In any case, the space used for the printing is
    //  going to be at least the length of the string.
    if(spaceAvaible < strlen) spaceAvaible = strlen;

    // -Length of the empty space for the centering.
    offset = (spaceAvaible - strlen) / 2;

    printSpaces(offset);
    printf("%s",str);
    // -Offset of the ending side.
    offset = spaceAvaible - (strlen + offset);
    printSpaces(offset);
}

// -String representation of the number n
// -Decimal base is used.
void toString(long long n, char buffer[]) {
    int i = 0, j = 0, rem;
    char aux;
    if(n == 0) {
        buffer[0] = '0'; buffer[1] =  0 ;
        return ;
    }
    if(n < 0) {
        buffer[i++] = '-'; n = -n;
    }
    while(n > 0) {
        rem = n % 10; buffer[i++] = rem + 48;
        n -= rem; n /= 10;
    }
    buffer[i--] = 0;
    if(buffer[0] == '-') j = 1;
    while(i > j) {
        aux = buffer[i];
        buffer[i--] = buffer[j];
        buffer[j++] = aux;
    }
}

void printBytesAsInt(const char* bytes, int byteslen, int speacePerNum) {
    // -We'll take blocks of four bytes, that's why
    int q = byteslen/4, r = byteslen%4; // we need 'q' and 'r'
    int i,j;
    IntChar ic;
    char buffer[12];
    for(i = 0, j = 0; i < q; i++) {
        ic.ch[0] = bytes[j++];
        ic.ch[1] = bytes[j++];
        ic.ch[2] = bytes[j++];
        ic.ch[3] = bytes[j++];
        toString (ic.in, buffer);
        printf("|");
        printCentered (buffer, speacePerNum);
        printf("|");
    }
    if(r > 0) {
        for(i = 0; i < r; i++) ic.ch[i] = bytes[j++];
        for(; i < 4; i++) ic.ch[i] = 0;
    }
    if(j != byteslen) {
        printf("\nSomething went wrong in file PrintArray.c :: void"
               "printBytesTableAsType(const char* bytes, Type tp, int "
               "byteslen, int numColumns,const char* headers[], const char* "
               "leftTags[]).\n"
               "k == %d, it should have been %d\n", j, byteslen);
    }
}

// -Prints data in a tabular form.
void printTabularForm(void* data,           // -What we want to print.
                      Type  datatype,       // -Interpretation of data.
                      int   dataSize,       // -Number of elements in array.
                      int   numColumns,     // -Determine the number of rows.
                      const char* headers[],
                      const char* leftTags[]) {
    int aux, i, j;       // -Auxiliary variable and counters.
    int columnLen = 10;  // -Length of the columns in spaces.
    int leftTagsLen = 3; // -Length of left tags (if any).
    // -Determining the number of rows.
    int numRows = dataSize/numColumns;
    if(dataSize % numColumns != 0) numRows++;

    if(leftTags != NULL) { // -Left tags column length.
        if( leftTagsLen < (aux = maxLen(leftTags, numRows)) )
            leftTagsLen = aux;
    }

    if(headers != NULL) { // -Data length < headers length?
        if( columnLen < (aux = maxLen(headers, numColumns)) ) {
            columnLen = aux;
        }
        // -Printing headers.
        printf("|");
        printSpaces(leftTagsLen);
        printf("|");
        for(i = 0; i < numColumns; i++) {
            printf("|");
            printCentered(headers[i], columnLen);
            printf("|");
        }
        printf("\n");
    }
    char      *data_ch; // -Interpreting data as chars
    short     *data_sh; // -Interpreting data as shorts
    int       *data_in; // -Interpreting data as ints
    long long *data_ll; // -Interpreting data as long long's
    char buffer[20];     // -Holds the string representation of data elements
    for(i = 0; i < numRows; i++) {
        if(leftTags != NULL) {
            printf("|");
            printCentered(leftTags[i], leftTagsLen);
            printf("|");
        }
        aux = i*numColumns;
        switch(datatype) {
            case _char:
                data_ch = (char*)data;
                for(j = 0; j < numColumns; j++) {
                    printf("|");
                    if(aux + j < dataSize) {
                        toString(data_ch[aux + j],buffer);
                        printCentered(buffer, columnLen);
                    }
                    else printSpaces(columnLen);
                    printf("|");
                }
                break;
            case _short:
                data_sh = (short*)data;
                for(j = 0; j < numColumns; j++) {
                    printf("|");
                    if(aux + j < dataSize) {
                        toString(data_sh[aux + j],buffer);
                        printCentered(buffer, columnLen);
                    }
                    else printSpaces(columnLen);
                    printf("|");
                }
                break;
            case _int:
                data_in = (int*)data;
                for(j = 0; j < numColumns; j++) {
                    printf("|");
                    if(aux + j < dataSize) {
                        toString(data_in[aux + j],buffer);
                        printCentered(buffer, columnLen);
                    }
                    else printSpaces(columnLen);
                    printf("|");
                }
                break;
            case _longlong:
                data_ll = (long long*)data;
                for(j = 0; j < numColumns; j++) {
                    printf("|");
                    if(aux + j < dataSize) {
                        toString(data_ll[aux + j],buffer);
                        printCentered(buffer, columnLen);
                    }
                    else printSpaces(columnLen);
                    printf("|");
                }
                break;
        }
        printf("\n");
    }
}

// -Takes an array of char's an prints a table as if the array where of type
//  'tp'.
// -The elements of the tables are constructed using the bytes in the array
//  'bytes' and the type described in 'tp'; for example, if tp == _int then
//  the elements of the table are of the form bytes[4*i]·...·bytes[4*i + 3]
//  this is, the integer formed by the concatenation of four continuous bytes.
void printBytesTableAsType(const char* bytes,
                      Type  tp,         // -Interpretation of data.
                      int   byteslen,   // -Number of elements in array.
                      int   numColumns, // -Determine the number of rows.
                      const char* headers[],
                      const char* leftTags[]) {
    int aux, i, j, k, l; // -Auxiliary variable and counters.
    int columnLen = 18;  // -Length of the columns in spaces.
    int leftTagsLen = 3; // -Length of left tags (if any).

    // -Determining the number of rows.
    int tablelen = byteslen, remainder = 0;
    switch(tp) // Determining number of elements in table.
        {
        case _char: // -Nothing to do.
            break;
        case _short:
            tablelen /= 2;
            remainder = byteslen%2;
            break;
        case _int:
            tablelen /= 4;
            remainder = byteslen%4;
            break;
        case _longlong:
            tablelen /= 8;
            remainder = byteslen%8;
            break;
        }
    if(remainder > 0) tablelen++;
    int numRows = tablelen/numColumns;
    if(byteslen % numColumns != 0) numRows++;
    if(remainder > 0) tablelen--;

    if(leftTags != NULL) { // -Left tags column length.
        if( leftTagsLen < (aux = maxLen(leftTags, numRows)) )
            leftTagsLen = aux;
    }

    if(headers != NULL) { // -Data length < headers length?
        if( columnLen < (aux = maxLen(headers, numColumns)) ) {
            columnLen = aux;
        }
        // -Printing headers.
        printf("|");
        printSpaces(leftTagsLen);
        printf("|");
        for(i = 0; i < numColumns; i++) {
            printf("|");
            printCentered(headers[i], columnLen);
            printf("|");
        }
        printf("\n");
    }
    ShortChar     sc;
    IntChar       ic;
    LongLongChar llc;
    char buffer[20];
    // Printing table data.
    for(aux = i = j = k = 0; i < numRows; i++, aux += numColumns) {
        // -'i' is the row coordinate
        // -'j' is the column coordinate
        // -'k' will tell us were do we are in 'bytes'. Also can be
        //  interpreted as (i+j)*(sizeof(target type)).
        // -'aux' will tell us the row we are
        if(leftTags != NULL) {
            printf("|");
            printCentered(leftTags[i], leftTagsLen);
            printf("|");
        }
        switch(tp) {
            case _char:
                for(j = 0; j < numColumns; j++) {
                    printf("|");
                    // In this case, k == aux + j
                    if(k < tablelen) {
                        toString(bytes[k++],buffer);
                        printCentered(buffer, columnLen);
                    }
                    else printSpaces(columnLen);
                    printf("|");
                }
                break;
            case _short:
                for(j = 0; j < numColumns; j++) {
                    printf("|");
                    if(aux+j < tablelen) {
                        // -Taking two bytes at a time.
                        sc.ch[0] = bytes[k++];
                        sc.ch[1] = bytes[k++];
                        toString(sc.sh,buffer);
                        printCentered(buffer, columnLen);
                    }
                    else if(aux + j == tablelen && remainder > 0) {
                        sc.ch[0] = bytes[k++];
                        sc.ch[1] = 0;
                        toString(sc.sh,buffer);
                        printCentered(buffer, columnLen);
                    } else
                        printSpaces(columnLen);
                    printf("|");
                }
                break;
            case _int:
                for(j = 0; j < numColumns;j++) {
                    printf("|");
                    if(aux + j < tablelen) {
                        // -Taking four bytes at a time.
                        ic.ch[0] = bytes[k++];
                        ic.ch[1] = bytes[k++];
                        ic.ch[2] = bytes[k++];
                        ic.ch[3] = bytes[k++];
                        toString(ic.in,buffer);
                        printCentered(buffer, columnLen);
                    }
                    else if(aux + j == tablelen && remainder > 0) {
                        // -The data length is not a multiple of the size
                        //  of the targeted data type.
                        for(l = 0; l < remainder; l++) ic.ch[l] = bytes[k++];
                        for(; l < 4; l++) ic.ch[l] = 0;
                        toString(ic.in,buffer);
                        printCentered(buffer, columnLen);
                    } else
                        printSpaces(columnLen);
                    printf("|");
                }
                break;
            case _longlong:
                for(j = 0; j < numColumns*8;) {
                    printf("|");
                    if(aux + j < tablelen) {
                        // -Taking eight bytes at a time.
                        llc.ch[0] = bytes[k++];
                        llc.ch[1] = bytes[k++];
                        llc.ch[2] = bytes[k++];
                        llc.ch[3] = bytes[k++];
                        llc.ch[4] = bytes[k++];
                        llc.ch[5] = bytes[k++];
                        llc.ch[6] = bytes[k++];
                        llc.ch[7] = bytes[k++];
                        toString(llc.ll,buffer);
                        printCentered(buffer, columnLen);
                    } else if(aux + j == tablelen && remainder > 0) {
                        // -The data length is not a multiple of the size
                        //  of the targeted data type.
                        for(l = 0; l < remainder; l++) ic.ch[l] = bytes[k++];
                        for(; l < 8; l++) ic.ch[l] = 0;
                        toString(ic.in,buffer);
                        printCentered(buffer, columnLen);
                    } else
                        printSpaces(columnLen);
                    printf("|");
                }
                break;
        }
        printf("\n");
    }
    if(k != byteslen) {
        printf("\nSomething went wrong in file PrintArray.c :: void"
               "printBytesTableAsType(const char* bytes, Type tp, int "
               "byteslen, int numColumns,const char* headers[], const char* "
               "leftTags[]).\n"
               "k == %d, it should have been %d\n", k, byteslen);
    }
}
