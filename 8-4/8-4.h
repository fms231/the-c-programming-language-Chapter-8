#define NULL 0

#define EOF (-1)
#define BUFFER_SIZE 1024
#define MAX_NR_OF_OPEN_FILES 20

#define SEEK_SET 0

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

struct _io_buffer_file
{
    int counter;
    char *next_char_pos_p;
    char *base;
    struct
    {
        int _READ : 1;
        int _WRITE : 1;
        int _UNBUF : 1;
        int _EOF : 1;
        int _ERR : 1;
    } flag;
    int file_descriptor;
};

typedef struct _io_buffer_file FILE;

extern FILE _io_buffer[MAX_NR_OF_OPEN_FILES];

int _fill_buffer(FILE *);
int _flush_buffer(int c, FILE *file_p);
int file_flush(FILE *file_p);
FILE *file_open(char *name, char *mode);
int file_close(FILE *file_p);
int file_seek(FILE *file_p, long offset, int whence);

#define stderr (&_io_buffer[2])

#define getc(p) ((--p->counter >= 0) ? (unsigned char)*(p)->next_char_pos_p++ : _fill_buffer(p))
#define putc(x, p) ((--p->counter >= 0) ? *p->next_char_pos_p++ = x : _flush_buffer(x, p))