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

typedef union intchar {
    int  in;
    char ch[4];
}IntChar;

typedef union longlongchar {
    long long ll;
    char ch[8];
}LongLongChar;

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
