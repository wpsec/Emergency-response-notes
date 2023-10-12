HiperDrop.exe 1620 -o -1f -b
HiperDrop 4660 -d -1f -b


HiperDrop process memory downloader tool 
code by gynvael.coldwind//vx
mailto: gynvael@coldwind.pl
www   : http://gynvael.coldwind.pl/
license: see License.txt

Current version: 0.0.1


-=) About HiperDrop
HiperDrop is a simple console application that can be used to acquire
a full memory dump and a memory map of an external process.
In current version HiperDrop can attach to a process in two different
ways, download the memory using two searching "algorithms", and write
the output in three different ways.

-=) Attachment methods
Currently there are two supported attachment methods:

1. OpenProcess, Option -o (DEFAULT)
Attachment to a process is done using OpenProcess with two flags:
PROCESS_VM_READ - for reading memory
PROCESS_QUERY_INFORMATION - for queries about the memory
In most case this method should be sufficient.

2. Debugger API, Option -d
Attachment to a process is done using the debugger API, or to be exact,
the DebugActiveProcess function. When HyperDrop finished downloading
memory it tries to detach from the process using DebugActiveProcessStop.
Since DebugActiveProcessStop is available starting from Windows XP,
HiperDrop will not work on Windows 2000 or earlier.
In case the detachment fails, HyperDump will let the user decide when
to exit (and kill the attached process at the same time).
TODO: Make HiperDrop work on Windows 2000, just without detaching

3. and 4. are on the TODO list - kernel mode and sending data
from the inside of the process via pipes.

-=) Read methods
Currently there are two read algorithms supported:

1. VirtualQueryEx, Option -q (DEFAULT)
The function VirtualQueryEx is used to query blocks of memory and
quickly skip unused parts, and find the size of allocated blocks.
This method is very fast, and so, it's also default.

2. Page by page, Option -b
A brute-force style method, that works by calling ReadProcessMemory on
each page, without prior queries to check if the page even exists.
This is much slower than the previous method, but in some cases (e.g.
when VirtualQueryEx is concealing some area due to unknown reasons) it
may come in handy.
To tell you the truth, I never did see any such situation, but, as they
say, better safe than sorry :)

-=) Output methods
There are three supported output methods:

1. Multiple files, Option -m (DEFAULT)
For every block of memory a single file is created. Such a file may
be from 1 page long (4096 bytes), to multiple page long. This is the
default, since it's handy in most (not all) cases.

2. Single file, Option -1
All the blocks are written to a single file. The free pages are skipped,
so the blocks are not really on any predifined offsets in the file. In
the map file there is information on which offset what memory area was
written to.
This method is quite good if you are searching for something in the
memory, but you totally don't care about where it was placed (or
you afford to parse the map file).

3. Single huge file, Option -1f
Like the above method, except that free pages are written to as
zero-filled to the file, meaning that memory address and file offsets
are exactly the same, but the file will be 2 to 3 GB large.

-=) ToDo list
Grep the source file for TODO phrase, and also see the begining of the
file :)

-=) Credits
Currently all code is by gynvael.coldwind//vx :)




