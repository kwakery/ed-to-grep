#include "grep.h"

int main(int argc, char *argv[]) {
  char *p1, *p2;
  argv++;
  if (argc > 2) {
    // printf("Arg 1: %s, Arg 2: %s\n", *argv, *(argv + 1));
    compile(*argv++); // Compile the expression buffer
    // printf("%s\n", expbuf);
    // puts(*argv);
    init(*argv); // Open file
    // search();    // Search for results
  } else {
    puts("Please run the program again with 2 args.\n(1)Regular "
         "Expression\n(2)File name\n");
  }
  exit(0);
  return 0;
}
int begins_with(const char *s1, const char *s2) {
  while (*s1 == *s2 && *s1 != '\0' && *s2 != '\0') {
    ++s1;
    ++s2;
  }
  if (*s2 == '\0')
    return 1; // match
  return 0;
}
int ends_with(const char *s1, const char *s2) {
  char *r = (char *)s2;
  while (*s1 != *s2 && *s1 != '\0') {
    ++s1;
  }
  while (*s1 == *r && *r != '\0' && *s1 != '\0') {
    ++s1;
    ++r;
  }
  if (*s1 == '\0' && *r == '\0')
    return 1;
  if (*s1 == '\0')
    return 0;
  return ends_with(s1, s2);
}
void replace_(char *s1, const char *s2) {
  while (*s2 != '\0') {
    *s1++ = *s2++;
  }
  *s1++ = '\0';
}
/* A somewhat simplified global().
 *Uses execute() to get what we need.
 */
void search() {
  char *gp = genbuf, *lp = linebuf;
  while (*gp != '\0') { // eof
    if (*gp == '\n') {
      *lp++ = '\0';

      if (execute()) {
        if (*currfile != '\0') {
          putchr('[');
          puts(currfile);
          putchr(']');
        } else
          puts(currfile);

        puts(linebuf);
        putchr('\n');
      }
      lp = linebuf; // need to reset lp...
      ++gp;
    } else {
      *lp++ = *gp++;
    }
  }
}
void error(char *s) {
  puts(s);
  if (io > 0) {
    close(io);
    io = -1;
  }
}
int getchr(void) {
  char c;
  if ((lastc = peekc)) {
    peekc = 0;
    return (lastc);
  }
  if (read(0, &c, 1) <= 0) {
    return (lastc = EOF);
  }
  lastc = c & 0177;
  return (lastc);
}
int getfile(void) {
  int c;
  char *lp = linebuf, *fp = nextip;
  do {
    if (--ninbuf < 0) {
      if ((ninbuf = (int)read(io, genbuf, LBSIZE) - 1) < 0) {
        if (lp > linebuf) {
          puts("'\\n' appended");
          *genbuf = '\n';
        } else {
          return (EOF);
        }
      }
      fp = genbuf;
      while (fp < &genbuf[ninbuf]) {
        if (*fp++ & 0200) {
          break;
        }
      }
      fp = genbuf;
    }
    c = *fp++;
    if (c == '\0') {
      continue;
    }
    if (c & 0200 || lp >= &linebuf[LBSIZE]) {
      lastc = '\n';
      error(Q);
    }
    *lp++ = c;
    count++;
  } while (c != '\n');
  *--lp = 0;
  nextip = fp;
  return (0);
}
void init(char *filename) {
  char *ext, *end, *star, *rs;
  if ((io = open(filename, 0)) < 0) { // is directory or does not exist
    end = strrchr(filename, '/');
    star = strchr(end + 1, '*');
    if (star == NULL) {
      error("Invalid file.");
    }
    *end = '\0';
    *star = '\0';
    // printf("%s\n", filename); // Folder
    // printf("%s\n", end + 1);  // left of *
    // printf("%s\n", star + 1); // right of *
    d = opendir(filename);
    if (d) {
      while ((dir = readdir(d)) != NULL) {
        // ext = get_ext(dir->d_name);
        if (begins_with(dir->d_name, end + 1) &&
            ends_with(dir->d_name, star + 1)) {
          replace_(currfile, dir->d_name);
          open_file(currfile);
          search();
        }
      }
      closedir(d);
    }
  } else {
    close(io);
    open_file(filename);
    search();
  }
}
void open_file(char *filename) {
  if ((io = open(filename, 0)) < 0) {
    error(filename);
  }
  getfile(); // store file contents onto general buffer and line buffer
  close(io); // from exfile()
  io = -1;
  // printf("----[%s contents]----\n%s\n", filename, genbuf);
  // printf("----[Current line]----\n%s\n\n", linebuf);
}
void cerror() {
  *expbuf = 0;
  nbra = 0;
  error(Q);
}
void defchar(char **ep, int *c) {
  **ep++ = CCHR;
  **ep++ = *c;
}
/*  Compiles the expression buffer
    What was changed:
    eof in parameter was replaced with a char* to read (no more getchar())
    eof inside was used to check at the end, but it's using null terminated
   strings now, so check with '\0'
*/
void compile(char *s) {
  int c, cclcnt;
  char *ep = expbuf, *lastep, bracket[NBRA], *bracketp = bracket, *sp = s;
  // if ((c = getchr()) == '\n') { peekc = c;  c = eof; }
  // if (c == eof) {  if (*ep==0) { error(Q); }  return; }
  nbra = 0;
  if (*sp == '^') {
    ++sp;
    *ep++ = CCIRC;
  }
  peekc = *sp;
  lastep = 0;
  for (;;) {
    if (ep >= &expbuf[ESIZE]) {
      cerror();
    }
    c = *sp++;
    if (c == '\0') {
      if (bracketp != bracket) { // Error if bracket didn't end
        cerror();
      }

      *ep++ = CEOF;
      return;
    }
    lastep = ep;
    switch (c) {
    case '\\':
      if ((c = *sp++) == '(') {
        if (nbra >= NBRA) {
          cerror();
        }
        *bracketp++ = nbra;
        *ep++ = CBRA;
        *ep++ = nbra++;
        continue;
      }
      if (c == ')') {
        if (bracketp <= bracket) {
          cerror();
        }
        *ep++ = CKET;
        *ep++ = *--bracketp;
        continue;
      }
      if (c >= '1' && c < '1' + NBRA) {
        *ep++ = CBACK;
        *ep++ = c - '1';
        continue;
      }
      *ep++ = CCHR;
      if (c == '\n') {
        cerror();
      }
      *ep++ = c;
      continue;
    case '.':
      *ep++ = CDOT;
      continue;
    case '\n':
      cerror();
    case '*':
      if (lastep == 0 || *lastep == CBRA || *lastep == CKET) {
        defchar(&ep, &c);
      }
      *lastep |= STAR;
      continue;
    case '$':
      if ((peekc = *sp++) != '\0') {
        defchar(&ep, &c);
      }
      --sp; // TODO: might cause a problem - We're going backwards after peeking
      *ep++ = CDOL;
      continue;
    case '[':
      *ep++ = CCL;
      *ep++ = 0;
      cclcnt = 1;
      if ((c = *sp++) == '^') {
        c = *sp++;
        ep[-2] = NCCL;
      }
      do {
        if (c == '\n') {
          cerror();
        }
        if (c == '-' && ep[-1] != 0) {
          if ((c = *sp++) == ']') {
            *ep++ = '-';
            cclcnt++;
            break;
          }
          while (ep[-1] < c) {
            *ep = ep[-1] + 1;
            ep++;
            cclcnt++;
            if (ep >= &expbuf[ESIZE]) {
              cerror();
            }
          }
        }
        *ep++ = c;
        cclcnt++;
        if (ep >= &expbuf[ESIZE]) {
          cerror();
        }
      } while ((c = *sp++) != ']');
      lastep[1] = cclcnt;
      continue;
    default:
      defchar(&ep, &c);
    }
  }
}
/* Parses the compiled expression buffer */
int execute() {
  char *p1 = linebuf, *p2 = expbuf;
  int c;

  for (c = 0; c < NBRA; c++) {
    braslist[c] = 0;
    braelist[c] = 0;
  }

  if (*p2 == CCIRC) {
    loc1 = p1;
    return (advance(p1, p2 + 1));
  }
  if (*p2 == CCHR) { /* fast check for first character */
    c = p2[1];
    do {
      if (*p1 != c) {
        continue;
      }
      if (advance(p1, p2)) {
        loc1 = p1;
        return (1);
      }
    } while (*p1++);
    return (0);
  }
  do { /* regular algorithm */
    if (advance(p1, p2)) {
      loc1 = p1;
      return (1);
    }
  } while (*p1++);
  return (0);
}
int star(char *lp, char *ep, char *curlp) {
  // don't need double star since we return... I think
  do {
    lp--;
    if (advance(lp, ep)) {
      return (1);
    }
  } while (lp > curlp);
  return (0);
}
int advance(char *lp, char *ep) {
  char *curlp;
  int i;
  for (;;) {
    switch (*ep++) {
    case CCHR:
      if (*ep++ == *lp++) {
        continue;
      }
      return (0);
    case CDOT:
      if (*lp++) {
        continue;
      }
      return (0);
    case CDOL:
      if (*lp == 0) {
        continue;
      }
      return (0);
    case CEOF:
      loc2 = lp;
      return (1);
    case CCL:
      if (cclass(ep, *lp++, 1)) {
        ep += *ep;
        continue;
      }
      return (0);
    case NCCL:
      if (cclass(ep, *lp++, 0)) {
        ep += *ep;
        continue;
      }
      return (0);
    case CBRA:
      braslist[*ep++] = lp;
      continue;
    case CKET:
      braelist[*ep++] = lp;
      continue;
    case CBACK:
      if (braelist[i = *ep++] == 0) {
        error(Q);
      }
      if (backref(i, lp)) {
        lp += braelist[i] - braslist[i];
        continue;
      }
      return (0);
    case CBACK | STAR:
      if (braelist[i = *ep++] == 0) {
        error(Q);
      }
      curlp = lp;
      while (backref(i, lp)) {
        lp += braelist[i] - braslist[i];
      }
      while (lp >= curlp) {
        if (advance(lp, ep)) {
          return (1);
        }
        lp -= braelist[i] - braslist[i];
      }
      continue;
    case CDOT | STAR:
      curlp = lp;
      while (*lp++) {
      }
      return star(lp, ep, curlp);
    case CCHR | STAR:
      curlp = lp;
      while (*lp++ == *ep) {
      }
      ++ep;
      return star(lp, ep, curlp);
    case CCL | STAR:
    case NCCL | STAR:
      curlp = lp;
      while (cclass(ep, *lp++, ep[-1] == (CCL | STAR))) {
      }
      ep += *ep;
      return star(lp, ep, curlp);
    default:
      error(Q);
    }
  }
}
int backref(int i, char *lp) {
  char *bp;
  bp = braslist[i];
  while (*bp++ == *lp++) {
    if (bp >= braelist[i]) {
      return (1);
    }
  }
  return (0);
}
int cclass(char *set, int c, int af) {
  int n;
  if (c == 0) {
    return (0);
  }
  n = *set++;
  while (--n) {
    if (*set++ == c) {
      return (af);
    }
  }
  return (!af);
}
void puts(char *sp) {
  col = 0;
  while (*sp)
    putchr(*sp++);
  // putchr('\n');
}
void putchr(int ac) {
  char *lp;
  int c;
  lp = linp;
  c = ac;
  *lp++ = c;
  if (c == '\n' || lp >= &line[64]) {
    linp = line;
    write(oflag ? 2 : 1, line, lp - line);
    return;
  }
  linp = lp;
}
