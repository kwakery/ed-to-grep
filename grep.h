#include <dirent.h>
#include <stdlib.h>
#include <string.h>

#define LBSIZE 4096 // line buffer size
#define ESIZE 256   // expression buffer size
#define NBRA 5      // bracket size (?)
#define EOF -1      // end of file

/* used in the expression buffer*/
#define CBRA 1
#define CCHR 2
#define CDOT 4
#define CCL 6
#define NCCL 8
#define CDOL 10
#define CEOF 11
#define CKET 12
#define CBACK 14
#define CCIRC 15

#define STAR 01

char *getline(unsigned int tl);
int advance(char *lp, char *ep);
int backref(int i, char *lp);
int cclass(char *set, int c, int af);
void compile(char *s);
void error(char *s);
int execute();
void exfile(void);
void filename(int comm);
int getchr(void);
int getfile(void);
void init(char *);
void open_file(char *);
void putchr(int ac);
int putline(void);
void puts(char *sp);
void search();
char *strrchr_(const char, char);
char *strchr_(const char, char);
void replace_(char *, const char *);
char *get_ext(const char *);

int open(char *, int);
int read(int, char *, int);
int write(int, char *, int);
int close(int);
