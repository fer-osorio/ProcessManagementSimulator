#ifndef _PRINTARRAY_
#define  _PRINTARRAY_

#define TRUE  1
#define FALSE 0

typedef enum type {
    _char,
    _short,
    _int,
    _longlong
}Type;

typedef union shortchar {
    short sh;
    char ch[2];
}ShortChar;

// -Creates a short from an array of two char's
inline short charToShort(char ch[2]) {
    ShortChar sc;
    sc.ch[0] = ch[0];
    sc.ch[1] = ch[1];
    return sc.sh;
}

// -Fills an array of two char's from a short
// -The result is saved in the array 'res_dir' (result direction)
inline void shortToChar(short sh, char res_dir[2]) {
    ShortChar sc; sc.sh = sh;
    res_dir[0] = sc.ch[0];
    res_dir[1] = sc.ch[1];
}

typedef union intchar {
    int  in;
    char ch[4];
}IntChar;

// -Creates a int from an array of two char's
inline int charToInt(char ch[4]) {
    IntChar ic;
    ic.ch[0] = ch[0];
    ic.ch[1] = ch[1];
    ic.ch[2] = ch[2];
    ic.ch[3] = ch[3];
    return ic.in;
}

// -Fills an array of four char's from a int
// -The result is saved in the array 'res_dir' (result direction)
inline void IntToChar(int in, char res_dir[4]) {
    IntChar ic; ic.in = in;
    res_dir[0] = ic.ch[0];
    res_dir[1] = ic.ch[1];
    res_dir[2] = ic.ch[2];
    res_dir[3] = ic.ch[3];
}

typedef union longlongchar {
    long long ll;
    char ch[8];
}LongLongChar;

// -Creates a long long from an array of two char's
inline long long charToLongLong(char ch[8]) {
    LongLongChar llc;
    llc.ch[0] = ch[0];
    llc.ch[1] = ch[1];
    llc.ch[2] = ch[2];
    llc.ch[3] = ch[3];
    llc.ch[4] = ch[4];
    llc.ch[5] = ch[5];
    llc.ch[6] = ch[6];
    llc.ch[7] = ch[7];
    return llc.ll;
}

// -Creates an array of eight char's from a long long
// -The result is saved in the array 'res_dir' (result direction);
inline void longLongToChar(long long ll, char res_dir[8]) {
    LongLongChar llc; llc.ll = ll;
    res_dir[0] = llc.ch[0];
    res_dir[1] = llc.ch[1];
    res_dir[2] = llc.ch[2];
    res_dir[3] = llc.ch[3];
    res_dir[4] = llc.ch[4];
    res_dir[5] = llc.ch[5];
    res_dir[6] = llc.ch[6];
    res_dir[7] = llc.ch[7];
}

// -Length of string.
int len(const char str[]);

// -Maximum length of a family of standard c-strings.
int maxLen(const char* strArray[], int numStrs);

// -Prints the character 'ch' a 'length' amount of times.
void printCharacters(char ch, int length);

// -Printing a 'lenght' amount of spaces.
void printSpaces(int length);

// -Prints 'str' centrally inside a imaginary column of length 'spaceAvailable'
// -If spaceAvailable <= len(str), then the str is printed normally.
void printCentered(const char str[], int spaceAvaible);

// -String representation of the number n
// -Decimal base is used.
void toString(long long n, char buffer[]);

// -Takes an array of char's an prints as if the array where of type int.
void printBytesAsInt(const char* bytes, int byteslen, int speacePerNum);

// -Prints data in a tabular form.
void printTabularForm(void* data,           // -What we want to print.
                      Type  datatype,       // -Determine alignment.
                      int   dataSize,       // -Number of elements in array.
                      int   numColumns,     // -Determine the number of rows.
                      const char* headers[],
                      const char* leftTags[]);
                      //int   enum_rows);     // -0 to not enumerate the rows,
                                            //  not zero to get the enumeration

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
                        const char* leftTags[]);
#endif
