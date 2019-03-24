# ed-to-grep
## Matthieu Tran MoWe 4PM-5:50PM

A project for my C Programming class (CPSC 223C). We were instructed to refactor the existing ed.c editor code into a mini version of grep.

## Resources Used
I looked through the [Dirent.h documentation](http://pubs.opengroup.org/onlinepubs/7908799/xsh/dirent.h.html) for the directory indexing.

## Usage

```bash
clang grep.c -o grep
./grep [a-z] ./home/matt/filename.txt # (1) Regular Expression String, (2) File name
```

#### Using Multiple Files

```bash
clang grep.c -o grep
./grep [a-z] ./home/matt/all_*.csv # The filename has to begin with "all_" and end with ".csv"
```

## License
[GNU GPLv3](https://choosealicense.com/licenses/gpl-3.0/)

## Credits
Ken Thompson for making the ed editor (:
