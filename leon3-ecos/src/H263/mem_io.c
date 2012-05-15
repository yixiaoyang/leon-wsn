/*
 * xiaoyang yi@2010.12.16
 * 
 * overwrite the io interface of glibc
 */
#include <stdarg.h>

#define MAX_MEMIO_BUFF	(1024*1024*10)//10M
#define MAX_FILES		1
#define RUN_ON_PC		1

//----------------------------------------------------------------
// Author:	xiaoyang yi
// Date:	2010.12.16
// Note:	struct MFILE
//			buffer:	memory buffer for content
//			pos:read/write pointer
//----------------------------------------------------------------

typedef struct _MFILE{
	char* buffer;
	char* filename;
	char* pos;
	unsigned int size;
}MFILE;

//constant files record
struct _mfiles{
	char* filename;
	MFILE* fp;
}MFILES[] = {
	{,}
};

//----------------------------------------------------------------
// Author:	xiaoyang yi
// Date:	2010.12.16
// Note:	interfaces to overwrite
//----------------------------------------------------------------
MFILE* fopen( const char * _Filename,  const char * _Mode);
void fclose( MFILE * _File);

size_t fread ( void * ptr, size_t size, size_t count, MFILE * stream );
size_t fwrite ( const void * ptr, size_t size, size_t count, MFILE * stream );

void rewind( MFILE * _File);//set the r/w pointer at the start
int fseek ( MFILE * stream, long int offset, int origin );

int fscanf ( MFILE * stream, const char * format, ... );
int fprintf ( MFILE * stream, const char * format, ... );
int fflush ( MFILE * stream );
//int putc ( int character, FILE * stream );

//----------------------------------------------------------------
// Author:	xiaoyang yi
// Date:	2010.12.16
// Note:	implementation
//----------------------------------------------------------------

/*
 * fopen() will using malloc to get free space.
 * so function malloc() is needed
 *
 * note:_Mode is unuseful
 */
MFILE* fopen( const char * _Filename,  const char * _Mode)
{
	MFILE* fp = (MFILE*)malloc(sizeof(MFILE));
	if (fp == NULL)
	{
		return NULL;
	}
	fp->
	
}

void fclose( MFILE * _File);

size_t fread ( void * ptr, size_t size, size_t count, MFILE * stream );
size_t fwrite ( const void * ptr, size_t size, size_t count, MFILE * stream );

void rewind( MFILE * _File);//set the r/w pointer at the start
int fseek ( MFILE * stream, long int offset, int origin );

int fscanf ( MFILE * stream, const char * format, ... );
int fprintf ( MFILE * stream, const char * format, ... );
int fflush ( MFILE * stream );


//--------------------------------------------------------------

/*
 * Conver int to string based on radix (usually 2, 8, 10, and 16)
 */
char *itoa(int num, char *str, int radix) 
{
    char string[] = "0123456789abcdefghijklmnopqrstuvwxyz";

    char* ptr = str;
    int i;
    int j;

    while (num)
    {
        *ptr++ = string[num % radix];
        num /= radix;

        if (num < radix)
        {
            *ptr++ = string[num];
            *ptr = '\0';
            break;
        }
    }

    j = ptr - str - 1;

    for (i = 0; i < (ptr - str) / 2; i++)
    {
        int temp = str[i];
        str[i] = str[j];
        str[j--] = temp;
    }

    return str;
}

/*
 * A simple printf function. Only support the following format:
 * Code Format
 * %c character
 * %d signed integers
 * %i signed integers
 * %s a string of characters
 * %o octal
 * %x unsigned hexadecimal
 */
int my_printf( const char* format, ...)
{
    va_list arg;
    int done = 0;

    va_start (arg, format);
    //done = vprintf( format, arg);

    while( *format != '\0')
    {
        if( *format == '%')
        {
            if( *(format+1) == 'c' )
            {
                char c = (char)va_arg(arg, int);
                putc(c);
            } else if( *(format+1) == 'd' || *(format+1) == 'i')
            {
                char store[20];
                int i = va_arg(arg, int);
                char* str = store;
                itoa(i, store, 10);
                while( *str != '\0') putc(*str++); 
            } else if( *(format+1) == 'o')
            {
                char store[20];
                int i = va_arg(arg, int);
                char* str = store;
                itoa(i, store, 8);
                while( *str != '\0') putc(*str++); 
            } else if( *(format+1) == 'x')
            {
                char store[20];
                int i = va_arg(arg, int);
                char* str = store;
                itoa(i, store, 16);
                while( *str != '\0') putc(*str++); 
            } else if( *(format+1) == 's' )
            {
                char* str = va_arg(arg, char*);
                while( *str != '\0') putc(*str++);
            }

            // Skip this two characters.

            format += 2;
        } else {
            putc(*format++);
        }
    }

    va_end (arg);

    return done;
} 

int main(int argc, char* argv[])
{ 
    int n = 255;
    char str[] = "hello, world!";

    printf("n = %d\n", n);
    printf("n = %i\n", n);
    printf("n = %o\n", n);
    printf("n = %x\n", n); 
    printf("first char = %c\n", str[0]); 
    printf("str = %s\n", str);
    printf("%s\tn = %d\n", str, n);

    // Test my_printf function

    printf("---------------my_printf--------------\n");
    
    my_printf("n = %d\n", n);
    my_printf("n = %i\n", n); 
    my_printf("n = %o\n", n);
    my_printf("n = %x\n", n);
    my_printf("first char = %c\n", str[0]); 
    my_printf("str = %s\n", str);
    my_printf("%s\tn = %d\n", str, n);

    return 0;
}

 

