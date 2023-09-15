


RegScanner v2.65
Copyright (c) 2004 - 2021 Nir Sofer
Web Site: https://www.nirsoft.net



Description
===========

RegScanner is a small utility that allows you to scan the Registry, find
the desired Registry values that match to the specified search criteria,
and display them in one list. After finding the Registry values, you can
easily jump to the right value in RegEdit, simply by double-clicking the
desired Registry item. You can also export the found Registry values into
a .reg file that can be used in RegEdit.



Advantages over RegEdit find of Windows
=======================================


* RegScanner utility display the entire search result at once, so you
  don't have to press F3 in order to find the next value.
* In addition to the standard string search (Like in RegEdit),
  RegScanner can also find Registry values by data length, value type
  (REG_SZ, REG_DWORD, and so on), and by modified date of the key.
* RegScanner can find a unicode string located inside a binary value.
* RegScanner allows you to make a case sensitive search.
* While scanning the Registry, RegScanner display the current scanned
  Registry key, as opposed to RegEdit, that simply display a boring
  "Searching the registry" dialog-box.



System Requirements
===================

This utility works in any version of Windows, from Windows XP to Windows
10. A separated download for x64 systems is also available.




Versions History
================


* Version 2.65:
  o Added new option: 'Open In RegEdit Without Elevation', which
    allows you to open the selected Registry key in RegEdit without
    Administrator privileges.
  o Fixed bug: RegScanner failed to export to .reg file properly when
    scanning the 64-bit Registry using the 32-bit version of RegScanner.
  o 'Open In RegEdit' option - Fixed to open the 64-bit version of
    RegEdit when scanning the 64-bit Registry using the 32-bit version of
    RegScanner.

* Version 2.61:
  o Added option to choose another font (name and size) to display in
    the main window (Under the View menu).

* Version 2.60:
  o Added option to filter the search by the owner of the Registry
    key. You can choose to search only Registry key with specific owners,
    or to exclude specific owners from the search. You can specify
    multiple Registry key owners as comma-delimited list, for example:
    SYSTEM, TrustedInstaller

* Version 2.55:
  o Added 'Key Owner' column, which displays the owner name of the
    Registry key.
  o Fixed to export properly REG_NONE values.

* Version 2.51:
  o Fixed bug: RegScanner saved the settings in the Registry instead
    of .cfg file.
  o Fixed RegScanner to use the .cfg file in the current directory if
    the folder is not specfied in the /cfg command-line option.

* Version 2.50:
  o Added new search option: 'Registry item contains remote network
    path'. When you select this option, RegScanner searches and displays
    any string that looks like a path to remote computer on your network,
    for example: \\192.168.0.10\c$ , Z:\Windows (Only if Z: is remote
    network drive).
  o This feature can be useful if your system freezes when your
    network is disconnected and you want to find references to remote
    computers in the Registry in order to try to fix the problem.

* Version 2.47:
  o Fixed the 'Automatically start remote Registry service' option to
    start the service on the remote computer when it's disabled.

* Version 2.46:
  o Added /RunAsSystem command-line option for running RegScanner as
    SYSTEM user from command line.
  o Added /RunAsAdmin command-line option for running RegScanner as
    Administrator (elevation) from command line.

* Version 2.45:
  o Added 'Run As System' option (Under the help menu), which allows
    you to easily run RegScanner as system user in order to search
    Registry hives accessible only with system user
    (HKEY_LOCAL_MACHINE\SAM and HKEY_LOCAL_MACHINE\Security ).

* Version 2.40:
  o Added 'Copy As .Reg File Format' option (Ctrl+R)
  o Added /sreg command-line option.

* Version 2.36:
  o Added option to start the remote Registry service when scanning
    the Registry of remote machine.

* Version 2.35:
  o Added option to scan the 32-bit or 64-bit view of the Regsitry.
  o By default (When you don't choose the 32-bit or 64-bit option) -
    the 32-bit version of RegScanner scans the 32-bit Registry and the
    64-bit version of RegScanner scans the 64-bit Registry.

* Version 2.31:
  o Fixed issue (RegScanner 32-bit without elevation): When scanning
    the HKEY_CURRENT_USER\Software\Classes\VirtualStore\Machine\SOFTWARE
    key, RegScanner displayed the keys stored under the
    HKEY_LOCAL_MACHINE\Software key. This problem was caused by a bug in
    Registry API of Windows, and a workaround was added to RegScanner so
    the Registry scan will work properly with this bug.

* Version 2.30:
  o Added option to search Registry keys modified in the last xx
    seconds/minutes/hours/days.
  o Added 'Today' button to set the time range fields for today.

* Version 2.26:
  o Fixed bug: 'Exact Match' search didn't work properly for REG_SZ
    data.

* Version 2.25:
  o Added option to scan the Registry from command-line and export he
    result to csv/tab-delimited/html file.

* Version 2.20:
  o Added 'Export Value To Binary File' option.
  o Fixed the status bar display during scanning to be smoother and
    without flickering.

* Version 2.17:
  o Added 'New RegScanner Instance' under the File menu, for opening
    a new window of RegScanner.

* Version 2.16
  o Fixed bug: When exporting value names with '\' character,
    RegScanner exported it as one '\' character instead of 2 '\'
    characters.

* Version 2.15
  o You can now specify multiple base keys to scan, delimited by
    comma. (For example: HKLM\Software\Microsoft\Windows\CurrentVersion,
    HKCU\Software\Microsoft\Windows\CurrentVersion )
    Be aware that if the base key you want to scan contains a comma
    character, you have enclose the key with quotes (For example:
    "HKLM\Software\My,Key")

* Version 2.10
  o The date/time values are now saved to the config file.
  o Added 'Save Settings' button for saving the settings without scan.
  o Fixed the edit mode in date/time controls.

* Version 2.07
  o Added 'Run As Administrator' option (Ctrl+F11), which allows you
    to easily run RegScanner as administrator on Windows Vista/7/8/2008.

* Version 2.06
  o RegScanner now remembers the last position of the scan options
    window.
  o Fixed bug: RegScanner failed to remember the last size/position
    of the main window if it was not located in the primary monitor.

* Version 2.05
  o Fixed bug: RegScanner crashed when searching values using
    wildcard.

* Version 2.04
  o Fixed to display date/time values according to daylight saving
    time settings.

* Version 2.03
  o Added 'Clear Recent Files List' option to the 'Recent Config
    Files' menu.

* Version 2.02
  o The find string is now displayed in the title bar of the main
    window.

* Version 2.01
  o When running RegScanner on Windows Vista/7/8/2008 without 'Run As
    Administrator', it now allows you to use the 'Open In RegEdit'
    feature (Elevation window will appear when using the 'Open In
    RegEdit' option).

* Version 2.00
  o Added option to search Registry items by wildcards. (For example:
    *num* , abc????)

* Version 1.96
  o RegScanner now remembers the last 20 find strings you used and
    allows you to easily choose them again from a combo-box.
  o Fixed the flickering occurred while scrolling the Registry items.

* Version 1.95
  o Fixed the export and delete features to work properly on the
    Registry of remote computer.
  o Fixed issue: The properties and the other windows opened in the
    wrong monitor, on multi-monitors system.

* Version 1.90
  o Added 'Recent Config Files' menu, which allows you to easily open
    the last 10 configuration files that you used.

* Version 1.85
  o Added option to exclude one or more keys from the Registry
    scanning. You can specify the Registry keys to exclude as
    comma-delimited list. By using this feature you can skip some major
    Registry keys (like HKLM\Software\Classes) to make the search much
    faster.

* Version 1.83
  o The status bar now displays the total data size of selected
    Registry items in KB/MB.

* Version 1.82
  o Added the 'Delete Selected Keys/Values' option into the context
    menu, as well as this options is now disabled if there is no any
    selected Registry item.

* Version 1.81
  o Fixed problem: On Windows 7/Vista, RegScanner entered into a
    recursive scan of VirtualStore key , something like
    HKCR\VirtualStore\MACHINE\SOFTWARE\Classes\VirtualStore\MACHINE\SOFTWAR
    E\Classes\VirtualStore...

* Version 1.80
  o Added 'Delete Selected Keys/Values' option. (See Below)

* Version 1.77
  o Fixed bug: When searching a key in 'Exact Match' mode, RegScanner
    compared the search string with the full key path, and thus it never
    found the key.

* Version 1.76
  o Export Selected Items - String values are now exported as strings
    and DWord values are now exported as DWords. (In previous versions
    they exported as binary)
  o Fixed bug: When using 'Export Selected Items' more than once, the
    first key was missed.

* Version 1.75
  o New option: Show found items during the scan process

* Version 1.70
  o Added 2 regular expression options

* Version 1.65
  o Fixed bug in x64 version: RegScanner returned wrong search
    results.

* Version 1.64
  o Fixed bug: The main window lost the focus when the user switched
    to another application and then returned back to RegScanner.

* Version 1.63:
  o Added support for saving as comma-delimited file.

* Version 1.62:
  o Fixed bug: RegScanner displayed a crash window immediately after
    running it.

* Version 1.61:
  o Fixed bug: RegScanner remained in memory if you closed the main
    window during the Registry scan.

* Version 1.60:
  o Fixed bug: opening HKLM in RegEdit failed if there was no '\'
    character.
  o New option: 'Open Clipboard Text In RegEdit' - Allows you to open
    RegEdit with the key that is currently in the clipboard.
  o New command-line option: /clipregedit

* Version 1.56:
  o Removed the text length limit from the 'Base Key' combo-box

* Version 1.55:
  o Added 'Registry item contains any value' in the matching
    combo-box. This option replace the 'Load all keys...' option in the
    previous versions.
  o Added 'Add only keys' option - allows you to scan only the
    Registry keys (without values).
  o Added a separated download for x64.

* Version 1.51:
  o Fixed RegEdit open problem with HKEY_USERS key.

* Version 1.50:
  o New option: Support Reg: Links - Allows you to use reg: links for
    opening Registry keys in RegEdit. For example:
    reg:HKLM\Software\Microsoft\Windows\CurrentVersion
  o New command-line option: /regedit
  o New option: Create Delete .Reg File - Creates a .reg file that
    deletes all selected Registry values.

* Version 1.40:
  o New search option: Registry item contains the specified DWORD
    value.
  o New search option: Registry item contains the specfied binary
    value
  o New option: Scan a remote computer.
  o New option: Copy the selected key to the clipbard (Ctrl+K).
  o As opposed to previous versions that only scanned the
    HKEY_LOCAL_MACHINE and HKEY_CURRENT_USER keys, now you can also
    choose to search under HKEY_CLASSES_ROOT, HKEY_USERS, and
    HKEY_CURRENT_CONFIG.

* Version 1.30:
  o New search option: search by value type.
  o New option: Save/load RegScanner configuration.
  o A tooltip is displayed when a string in a column is longer than
    the column length.
  o The configuration of RegScanner utility is now saved into a file
    (regscanner.cfg - located in the same folder of the executable file)
    instead of saving it to the Registry.
    (If from some reason, you still want that RegScanner utility will
    save the configuration data into the Registry, run it with /usereg
    parameter)

* Version 1.20:
  o New option: Export selected items to a .reg file that you can use
    in RegEdit.
  o New column: Data Length.
  o New search option: search by data length.
  o Dramatically reduced memory footprint during search. (Fixed bug
    from previous versions)

* Version 1.10:
  o New column: Key Modified Time (Only for Windows NT/2000/XP)
  o New search option: by key modified time (Only for Windows
    NT/2000/XP)
  o New search option: Add entry for each found key (Only when you
    select to search by key name)
  o New search option: Search for Unicode strings in binary values.
  o Added support for Windows XP visual styles.

* Version 1.00: First Release.


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



Using RegScanner
================

RegScanner is a standalone executable, and it doesn't require any
installation process or additional DLLs. In order to start using it, just
run the executbale file - regscanner.exe

After running RegScanner, you should select the desired scan/search
options, and click OK to start the scan.



Delete Selected Keys/Values
===========================

The delete feature was added in version 1.80 after many requests from
RegScanner users. This feature allows you to easily delete unwanted
Registry keys and values that you found in the search process. However,
in order to decrease the chance of damage from accidental delete action,
this feature works as follows:
* Before any delete action, RegScanner will ask you if you want to
  delete, and the default answer is No.
* When you delete a value item, only the value is deleted, when you
  delete a key, the entire key is deleted.
* A key won't be deleted if it has one or more subkeys.
* For every delete, the delete values/keys are added into a .bak file
  located in the folder of RegScanner.exe
  If RegScanner fails to create the .bak file, it won't delete the
  selected items.



Tips for using RegScanner
=========================


* If you want to view all Registry values under a specific key, choose
  'Registry item contains any value' in the matching combo-box, uncheck
  the 'Scan the following base keys', and in the Base Key combo-box type
  the desired key (For example:
  HKLM\Software\Microsoft\Windows\CurrentVersion\Explorer)
* If you want to view all Registry values that contains large amount of
  data, uncheck the 'Add entry for each found key' option, choose
  'Registry item contains any value' in the matching combo-box, choose
  the 'Display only data with the following lenth range', and type the
  data length range that you want to find, for example: from 1000 bytes
  to 100000 bytes.
* If you want to view the list of all Registry keys that have been
  modified in the last few hours or days, check the 'Add entry for each
  found key' and 'Add only keys' options, choose 'Registry item contains
  any value' in the matching combo-box, choose the 'Display only keys
  that their modified time is within the following range' option, and
  then select the modified date/time range that you want to view.



Searching DWORD/Binary values
=============================

Starting from version 1.40, you can search DWORD/Binary values. In order
to specify a DWORD value to search, you can type (In the Find string
text-box) a decimal value (for example: 255) or an hexadecimal value (for
example: 0xff).
In order to specify a binary value that you want to search, type (In the
Find string text-box) a sequence of hexadecimal bytes, with or without
spaces between them.
For example:
013fc7a127cc4a
01 3f c7 a1 27 cc 4a



Searching the Registry in a remote computer
===========================================

Starting from version 1.40, you can search the Registry in a remote
computer on your network. In order to do that, you should login to the
remote computer with admin user. Also, be aware that searching the
Registry in a remote computer might be quite slow.

For more information about connecting to the Registry of a remote
computer, read this:
How to connect a remote Windows 7/Vista/XP computer with NirSoft
utilities.



Using reg: Links
================

Starting from version v1.50, RegScanner allows you to use reg: URL as a
link to the Registry key in RegEdit editor.
For Example: Clicking the link below will open RegEdit with the specified
Registry key:
reg:HKEY_LOCAL_MACHINE\Software\Microsoft\Windows\CurrentVersion
You can also type the same reg: string in the address bar of Internet
Explorer or in the Run dialog-box of Windows.

In order to enable/disable this feature, go to the File menu of
RegScanner utility, and choose the 'Support Reg: Links' option.
Be aware that changing this option only affects the current logged on
user.



Using the /regedit and /clipregedit Command-Line Options
========================================================

Starting from version 1.50, you can use RegScanner.exe utility to open
the desired key in RegEdit, by using the /regedit command-line option.
For example:
RegScanner.exe /regedit
"HKEY_LOCAL_MACHINE\Software\Microsoft\Windows\CurrentVersion"
Starting from version 1.60, you can also use the /clipregedit
command-line option if you want to open a key that you copied into the
clipboard.



Notice about transition effects
===============================

If you want that RegEdit will be opened as fast as possible with the
right Registry value, you should disable the 'transition effects' on your
system. (On Windows 2000, go to the 'Display Properties' window, select
the 'Effects' tab, and uncheck the 'Use transition effects...' option.



Command-Line Options
====================



/cfg <Filename>
Start RegScanner with the specified config file.

/sreg <Filename>
Scan the Registry and export the result into .reg file.

/stext <Filename>
Scan the Registry and export the result into a simple text file.

/stab <Filename>
Scan the Registry and export the result into a tab-delimited text file.

/scomma <Filename>
Scan the Registry and export the result into a comma-delimited text file
(csv).

/stabular <Filename>
Scan the Registry and export the result into a tabular text file.

/shtml <Filename>
Scan the Registry and export the result into HTML file (Horizontal).

/sverhtml <Filename>
Scan the Registry and export the result into HTML file (Vertical).

/sxml <Filename>
Scan the Registry and export the result into XML file.

/RunAsAdmin
Run the RegScanner tool as Administrator (elevation)

/RunAsSystem
Run the RegScanner tool as SYSTEM user.



Translating RegScanner to other languages
=========================================

In order to translate RegScanner to other language, follow the
instructions below:
1. Run RegScanner with /savelangfile parameter:
   regscanner.exe /savelangfile
   A file named regscanner_lng.ini will be created in the folder of
   RegScanner utility.
2. Open the created language file in Notepad or in any other text
   editor.
3. Translate all string entries to the desired language. Optionally,
   you can also add your name and/or a link to your Web site.
   (TranslatorName and TranslatorURL values) If you add this information,
   it'll be used in the 'About' window.
4. After you finish the translation, Run RegScanner, and all
   translated strings will be loaded from the language file.
   If you want to run RegScanner without the translation, simply rename
   the language file, or move it to another folder.



Feedback
========

If you have any problem, suggestion, comment, or you found a bug in my
utility, you can send a message to nirsofer@yahoo.com
