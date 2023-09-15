


OpenSaveFilesView v1.15
Copyright (c) 2014 - 2017 Nir Sofer
Web site: http://www.nirsoft.net



Description
===========

OpenSaveFilesView is a simple tool that displays the list of files that
you previously opened with the standard open/save dialog-box of Windows.
For every file in the list, the following information is displayed:
Filename, Extension, Order (The order that the files were opened for
every file extension), Open Time (Available only for the last opened file
of every file type), File Modified/Created Time, File Size, and File
Attributes.



System Requirements
===================

This utility works on any version of Windows, starting from Windows 2000
and up to Windows 10. Both 32-bit and 64-bit systems are supported.



Data Source
===========

The data displayed by this tool is taken from the following Registry keys:
HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\ComDlg
32\OpenSaveMRU
HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\ComDlg
32\OpenSavePidlMRU



Versions History
================


* Version 1.15:
  o Added 'File Owner' column.

* Version 1.11:
  o Added 'Open Selected File With' option.

* Version 1.10:
  o Added 'Filename Only' column, which displays the filename without
    the full path.
  o Added option to choose the encoding to save the text/html/xml/csv
    files. (ANSI, UTF-8, UTF-16)

* Version 1.08:
  o Explorer context menu inside OpenSaveFilesView: When you
    right-click on a single item while holding down the shift key,
    OpenSaveFilesView now displays the context menu of Windows Explorer,
    instead of the OpenSaveFilesView context menu. This feature only
    works for existing files.

* Version 1.07:
  o Added 'Copy Filename' option.

* Version 1.06:
  o Added 'Open Filename Folder' option.

* Version 1.05:
  o Added 'Open Selected File' option.

* Version 1.00 - First release.



Start Using OpenSaveFilesView
=============================

OpenSaveFilesView doesn't require any installation process or additional
dll files. In order to start using it, simple run the executable file -
OpenSaveFilesView.exe
After running OpenSaveFilesView, the main window displays the list of
files that you previously selected with the standard open/save dialog-box
of Windows.



The 'Order' Column
==================

The 'Order' column shows you the order that you opened the files. For
every file extension, there is a separated set of order values. There are
also items with '*' extension, which show the order of the last 10 or 20
files for all file extensions.

For example: If you have a file named 'aaa.zip' with 'zip' in the
Extension column and '1' in the order value... and you also have a file
named 'bbb.zip' with 'zip' in the Extension column and '2' in the order
value, it means the last file you opened is aaa.zip, and before that you
opened the 'bbb.zip' file.

Be aware that the 'Open Time' value is only available for the last file
you opened for every extension (items with '1' in the Order column)



Command-Line Options
====================



/stext <Filename>
Save the list of opened/saved files into a regular text file.

/stab <Filename>
Save the list of opened/saved files into a tab-delimited text file.

/scomma <Filename>
Save the list of opened/saved files into a comma-delimited text file
(csv).

/stabular <Filename>
Save the list of opened/saved files into a tabular text file.

/shtml <Filename>
Save the list of opened/saved files into HTML file (Horizontal).

/sverhtml <Filename>
Save the list of opened/saved files into HTML file (Vertical).

/sxml <Filename>
Save the list of opened/saved files into XML file.



Translating OpenSaveFilesView to other languages
================================================

In order to translate OpenSaveFilesView to other language, follow the
instructions below:
1. Run OpenSaveFilesView with /savelangfile parameter:
   OpenSaveFilesView.exe /savelangfile
   A file named OpenSaveFilesView_lng.ini will be created in the folder
   of OpenSaveFilesView utility.
2. Open the created language file in Notepad or in any other text
   editor.
3. Translate all string entries to the desired language. Optionally,
   you can also add your name and/or a link to your Web site.
   (TranslatorName and TranslatorURL values) If you add this information,
   it'll be used in the 'About' window.
4. After you finish the translation, Run OpenSaveFilesView, and all
   translated strings will be loaded from the language file.
   If you want to run OpenSaveFilesView without the translation, simply
   rename the language file, or move it to another folder.



License
=======

This utility is released as freeware. You are allowed to freely
distribute this utility via floppy disk, CD-ROM, Internet, or in any
other way, as long as you don't charge anything for this and you don't
sell it or distribute it as a part of commercial product. If you
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
