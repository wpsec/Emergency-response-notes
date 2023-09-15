# How to Decrypt the Stupid Ransomware Family with StupidDecrypter

By [Lawrence Abrams](https://www.bleepingcomputer.com/author/lawrence-abrams/)

- December 21, 2018
- 10:17 AM
- [2](https://www.bleepingcomputer.com/ransomware/decryptor/how-to-decrypt-the-stupid-ransomware-family-with-stupiddecrypter/#comments)



![Stupid Decryptor](https://www.bleepstatic.com/content/posts/2018/12/stupid-header.jpg)

Stupid Ransomware is a family of ransomware infections that are typically utilized by less skilled developers and many utilize themes based on movies, pop-culture, or pretend to be law enforcement. This family of ransomware infections are created using an open source project that was posted to GitHub.

The good thing about this ransomware family is that they static encryption keys and thus are easy to decrypt. You can see an example of a Stupid Ransomware variant below.

![Stupid Ransomware variant](https://www.bleepstatic.com/images/news/ransomware/decryptors/s/stupid-ransomware/stupid-ransomware.jpg)**Stupid Ransomware variant**

[Michael Gillespie](https://twitter.com/demonslay335) has created a decryptor called StupidDecryptor that contains the decryption keys for all known Stupid Ransomware variants that were released. The currently supported variants are the ones that append the following extensions to the names of encrypted files.



![img](https://img.connatix.com/pid-067e5169-ece3-4ce8-87ad-c7961b8bb396/067e5169-ece3-4ce8-87ad-c7961b8bb396/1.png)Top Articles![Windows 11 now warns when typing your password in Notepad, websites](https://img.connatix.com/pid-067e5169-ece3-4ce8-87ad-c7961b8bb396/d59f5d0c-2087-416a-821c-141798bc501e/1ef30988-56d4-4701-afff-722d5960bbc0.jpg?crop=834:469,smart&width=834&height=469&format=jpeg&quality=60&fit=crop)![Microsoft SQL servers hacked in TargetCompany ransomware attacks](https://img.connatix.com/pid-067e5169-ece3-4ce8-87ad-c7961b8bb396/d59f5d0c-2087-416a-821c-141798bc501e/c69e23b6-31e5-4ee0-b81f-f3ab204293f4.jpg?crop=834:469,smart&width=834&height=469&format=jpeg&quality=60&fit=crop)![American Airlines learned it was breached from phishing targets](https://img.connatix.com/pid-067e5169-ece3-4ce8-87ad-c7961b8bb396/d59f5d0c-2087-416a-821c-141798bc501e/1384c35b-9719-45fe-92db-02a9fd86e994.jpg?crop=834:469,smart&width=834&height=469&format=jpeg&quality=60&fit=crop)![The Week in Ransomware - September 23rd 2022 - LockBit leak](https://img.connatix.com/pid-067e5169-ece3-4ce8-87ad-c7961b8bb396/d59f5d0c-2087-416a-821c-141798bc501e/92e14134-3a34-4afd-ac42-148890bfbd28.jpg?crop=834:469,smart&width=834&height=469&format=jpeg&quality=60&fit=crop)![UK Police arrests teen believed to be behind Uber, Rockstar hacks](https://img.connatix.com/pid-067e5169-ece3-4ce8-87ad-c7961b8bb396/d59f5d0c-2087-416a-821c-141798bc501e/ff5639ab-64f6-43d6-be26-d48d2c55c582.jpg?crop=834:469,smart&width=834&height=469&format=jpeg&quality=60&fit=crop)![YouTube down: Live streams hit by worldwide outage](https://img.connatix.com/pid-067e5169-ece3-4ce8-87ad-c7961b8bb396/d59f5d0c-2087-416a-821c-141798bc501e/038c6dfd-2148-4bef-b5c3-647048cae9f6.jpg?crop=834:469,smart&width=834&height=469&format=jpeg&quality=60&fit=crop)[READ MORE](https://www.bleepingcomputer.com/news/technology/youtube-down-live-streams-hit-by-worldwide-outage/?traffic_source=Connatix)[YouTube down: Live streams hit byworldwide outage](https://www.bleepingcomputer.com/news/technology/youtube-down-live-streams-hit-by-worldwide-outage/?traffic_source=Connatix)

![YouTube down: Live streams hit by worldwide outage](https://img.connatix.com/pid-067e5169-ece3-4ce8-87ad-c7961b8bb396/d59f5d0c-2087-416a-821c-141798bc501e/038c6dfd-2148-4bef-b5c3-647048cae9f6.jpg?crop=834:541,smart&width=834&height=541&format=jpeg&quality=60&fit=crop)

```
.666, .adam, .alosia, .android, .ANNABELLE, .anon, .bycicle, .corrupted, .crypted, .CYRON, .deria, .devil, .Doxes, .encrypt, .eTeRnItY, .FailedAccess, .fucked, .fucking, .fun, .H34rtBl33d, .Harzhuangzi, .haters, .iGotYou, .jeepers, .jigsaw, .killedXXX, .lock, .malki, .Malki, .MIKOYAN, .Nazi, .powned, .purge, .slvpawned, .SnakeEye, .Tesla, .whycry, .WINDIE, .XmdXtazX, .xncrypt, _crypt0, _nullbyte
```

Instructions on how to use StupidDecryptor are below.

## How to Decrypt Stupid Ransomware variants

To decrypt a Stupid Ransomware variant you can download the StupidDecryptor from the link below.

[![img](https://www.bleepstatic.com/images/news/ransomware/e/everbe/decrypted/decryptor-logo.png)](https://www.bleepingcomputer.com/download/stupiddecryptor/)

##### STUPIDDECRYPTOR

 

[DOWNLOAD NOW](https://www.bleepingcomputer.com/download/stupiddecryptor/)

Once downloaded, simply double-click on the executable to start the decryptor and you will be greeted with the main screen.

![StupidDecryptor ](https://www.bleepstatic.com/images/news/ransomware/decryptors/s/stupid-ransomware/stupiddecryptor.jpg)StupidDecryptor

Now click on the **Select Directory** button and select the drive you would like to decrypt, such as the C:\ drive. Once you select a drive, the Decrypt button will become available.

![Folder Selected](https://www.bleepstatic.com/images/news/ransomware/decryptors/s/stupid-ransomware/folder-selected.jpg)

Now click on the **Decrypt** button to begin decrypting the selected folder. Once you click Decrypt, the program will decrypt all the encrypted files and display the decryption status in the window.

![Decrypting Files](https://www.bleepstatic.com/images/news/ransomware/decryptors/s/stupid-ransomware/decrypting-files.jpg)**Decrypting Files**

When it has finished, the decryptor will display a summary of the amount of files that have been decrypted. If some of the files were skipped it may be due to permissions configured on those files.

![Decryption Finished](https://www.bleepstatic.com/images/news/ransomware/decryptors/s/stupid-ransomware/decryption-finished.jpg)**Decryption Finished**

Though your files are now decrypted, the original encrypted files will still be on your computer. Once you confirm that your files have been properly decrypted, you can use [CryptoSearch](https://www.bleepingcomputer.com/news/security/cryptosearch-finds-files-encrypted-by-ransomware-moves-them-to-new-location/) to move all the encrypted files into one folder so you can delete or archive them.

You can now close the decryptor and use your computer as normal. If you need help using this decryptor, feel free to leave a comment.

### Related Articles:

[No More Ransom helps millions of ransomware victims in 6 years](https://www.bleepingcomputer.com/news/security/no-more-ransom-helps-millions-of-ransomware-victims-in-6-years/)

[Microsoft SQL servers hacked in TargetCompany ransomware attacks](https://www.bleepingcomputer.com/news/security/microsoft-sql-servers-hacked-in-targetcompany-ransomware-attacks/)

[The Week in Ransomware - September 23rd 2022 - LockBit leak](https://www.bleepingcomputer.com/news/security/the-week-in-ransomware-september-23rd-2022-lockbit-leak/)

[BlackCat ransomware’s data exfiltration tool gets an upgrade](https://www.bleepingcomputer.com/news/security/blackcat-ransomware-s-data-exfiltration-tool-gets-an-upgrade/)

[LockBit ransomware builder leaked online by “angry developer”](https://www.bleepingcomputer.com/news/security/lockbit-ransomware-builder-leaked-online-by-angry-developer-/)