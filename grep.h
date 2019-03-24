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

char Q[] = "";
char T[] = "TMP";

int peekc;              // next char
int lastc;              // last char
char linebuf[LBSIZE];   // line buffer
char expbuf[ESIZE + 4]; // parsed expression buffer
char genbuf[LBSIZE];    // general buffer
long count;             // line count
char *nextip;
int ninbuf;
int io; // file opened?
int open(char *, int);
int read(int, char *, int);
int write(int, char *, int);
int close(int);

int oflag;
int col;
char *globp;
char *tfname;
char *loc1;
char *loc2;
char *braslist[NBRA];
char *braelist[NBRA];
int nbra;
char line[70];
char *linp = line;

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
void putchr(int ac);
int putline(void);
void puts(char *sp);
void search();
