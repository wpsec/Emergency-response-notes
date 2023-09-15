


WinPrefetchView v1.37
Copyright (c) 2010 - 2021 Nir Sofer
Web site: https://www.nirsoft.net



Description
===========

Each time that you run an application in your system, a Prefetch file
which contains information about the files loaded by the application is
created by Windows operating system. The information in the Prefetch file
is used for optimizing the loading time of the application in the next
time that you run it.
WinPrefetchView is a small utility that reads the Prefetch files stored
in your system and displays the information stored in them. By looking in
these files, you can learn which files every application is using, and
which files are loaded on Windows boot.



System Requirements
===================

This utility works on any version of Windows, starting from Windows XP,
and up to Windows 10. Earlier versions of Windows are irrelevant for this
utility, because they don't use Prefetch files.



Versions History
================


* Version 1.37:
  o Added 'Hide Programs Under Windows Folder' option. This option
    hides all system .exe files, like svchost.exe, taskhost.exe,
    dllhost.exe, and others.

* Version 1.36:
  o Fixed WinPrefetchView to display the correct 'Run Counter' values
    on the Prefetch files created by the latest builds of Windows 10.

* Version 1.35:
  o The 'Last Run Time' column now shows up to 8 run date/time
    values, for Prefetch files of Windows 8 or later.

* Version 1.32:
  o Fixed bug from version 1.31: WinPrefetchView accessed the floppy
    drive while loading the Prefetch information and possibly hung on
    some systems.

* Version 1.31:
  o Fixed to display the 'Full Path' column for Windows 10 prefetch
    files.

* Version 1.30:
  o Added support to the prefetch files of Windows 10 (Be aware that
    currently WinPrefetchView doesn't detect the correct drive letter on
    Windows 10 and thus the full path column of the file remains empty).

* Version 1.25:
  o Added 'Explorer Copy - Prefetch File' and 'Explorer Copy -
    Process File' options.

* Version 1.21:
  o Fixed to display date/time values according to daylight saving
    time settings.

* Version 1.20:
  o Added x64 build.
  o Added 'Missing Process' column, which displays 'Yes' if the
    process file is missing (On 64-bit systems, you must use the x64
    build to get accurate result for this column)

* Version 1.15:
  o Fixed the 'Run Counter' on the Prefetch files of Windows 8.
  o Added 'Auto Size Columns+Headers' option, which allows you to
    automatically resize the columns according to the row values and
    column headers.
  o Added 'Mark Odd/Even Rows' option, under the View menu. When it's
    turned on, the odd and even rows are displayed in different color, to
    make it easier to read a single line.

* Version 1.12:
  o Fixed bug: The 'Process EXE' column in the upper pane displayed
    the wrong .exe file in some Prefetch files.
  o Fixed issue: The properties and the 'Advanced Options' windows
    opened in the wrong monitor, on multi-monitors system.

* Version 1.11:
  o WinPrefetchView now automatically requires to run as
    administrator under Windows 7/8/Vista. (Without admin rights, it
    cannot read the Prefetch folder)

* Version 1.10:
  o Added /prefetchfile command-line option to export the records of
    specific .pf file.
  o Added 'Add Header Line To CSV/Tab-Delimited File' option. When
    this option is turned on, the column names are added as the first
    line when you export to csv or tab-delimited file.

* Version 1.07 - Removed the wrong encoding from the xml string, which
  caused problems to some xml viewers.
* Version 1.06 - Added /Folder command-line option to load
  WinPrefetchView with another Prefetch folder.
* Version 1.05 - Added 'Run Counter' and 'Last Run Time' columns.
* Version 1.00 - First release.



Using WinPrefetchView
=====================

WinPrefetchView doesn't require any installation process or additional
DLL files. In order to start using it, simply run the executable file -
WinPrefetchView.exe
The main window of WinPrefetchView contains 2 panes: The upper pane
displays the list of all Prefetch files in your system. When you select a
file in the upper pane, the lower pane displays the list of files stored
inside the selected Prefetch file, which represent the files that were
loaded by the application in the previous times that you used it.
These is also special Prefetch file, with 'NTOSBOOT-B00DFAAD.pf'
filename, which can show you the list of files that are loaded during
Windows boot process.

WinPrefetchView also allows you to delete the selected Prefetch files.
However, be aware that even when your delete a Prefetch file, it'll be
created again by the operating system when you run the same program again.



Command-Line Options
====================



/folder <Folder>
Start WinPrefetchView with Prefetch folder from another instance of
Windows operating system.

/prefetchfile <Filename>
You can use this command-line parameter with the other save commands
(/shtml, /stab, and so on) in order to export the records of specific .pf
file into text/html/csv file, for example:
WinPrefetchView.exe /shtml "C:\temp\records.html" /prefetchfile
"C:\windows\Prefetch\NTOSBOOT-B00DFAAD.pf"



/stext <Filename>
Save the list of Prefetch files into a regular text file.

/stab <Filename>
Save the list of Prefetch files into a tab-delimited text file.

/scomma <Filename>
Save the list of Prefetch files into a comma-delimited text file (csv).

/stabular <Filename>
Save the list of Prefetch files into a tabular text file.

/shtml <Filename>
Save the list of Prefetch files into HTML file (Horizontal).

/sverhtml <Filename>
Save the list of Prefetch files into HTML file (Vertical).

/sxml <Filename>
Save the list of Prefetch files into XML file.

/sort <column>
This command-line option can be used with other save options for sorting
by the desired column. If you don't specify this option, the list is
sorted according to the last sort that you made from the user interface.
The <column> parameter can specify the column index (0 for the first
column, 1 for the second column, and so on) or the name of the column,
like "File Size" and "Filename". You can specify the '~' prefix character
(e.g: "~Created Time") if you want to sort in descending order. You can
put multiple /sort in the command-line if you want to sort by multiple
columns.

Examples:
WinPrefetchView.exe /shtml "f:\temp\Prefetch.html" /sort 2 /sort ~1
WinPrefetchView.exe /shtml "f:\temp\Prefetch.html" /sort "~Modified Time"

/nosort
When you specify this command-line option, the list will be saved without
any sorting.



Translating WinPrefetchView to other languages
==============================================

In order to translate WinPrefetchView to other language, follow the
instructions below:
1. Run WinPrefetchView with /savelangfile parameter:
   WinPrefetchView.exe /savelangfile
   A file named WinPrefetchView_lng.ini will be created in the folder of
   WinPrefetchView utility.
2. Open the created language file in Notepad or in any other text
   editor.
3. Translate all string entries to the desired language. Optionally,
   you can also add your name and/or a link to your Web site.
   (TranslatorName and TranslatorURL values) If you add this information,
   it'll be used in the 'About' window.
4. After you finish the translation, Run WinPrefetchView, and all
   translated strings will be loaded from the language file.
   If you want to run WinPrefetchView without the translation, simply
   rename the language file, or move it to another folder.



License
=======

This utility is released as freeware. You are allowed to freely
distribute this utility via floppy disk, CD-ROM, Internet, or in any
other way, as long as you don't charge anything for this. If you
distribute this utility, you must include all files in the distribution
package, without any modification !



Disclaimer
==========

The software is provided "AS IS" without any warranty, either expressed
or implied, including, but not limited to, the implied warranties of
merchantability and fitness for a particular purpose. The author will not
be liable for any special, incidental, consequential or indirect damages
due to loss of data or any other reason.



Feedback
========

If you have any problem, suggestion, comment, or you found a bug in my
utility, you can send a message to nirsofer@yahoo.com
