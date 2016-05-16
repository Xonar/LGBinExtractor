LGBinExtractor
==============

A tool for extracting bin firmware for LG Phones

http://forum.xda-developers.com/showthread.php?t=1879915

##NOTES

 1. Sometimes partition require that they fill the entire 'size' even though the last bit is whitespace. The bin/tot files often don't have that last bit of whitespace. The problem is only noticed so far in tot partitions that are broken up into seperate parts and there is a prompt to ask you if you want to add the data if there is a GPT header available to get correct size from. I intend to later analyze the partition itself to see if it's neccesary and how much 'space' should there be.

 2. My database of bin files is still to small to analyze them when it's unkown and only supported bin/tot files will work. Just follow the steps present in Support to get it added

##Contributing
Made a change? </br>
Fixed a bug?</br>
Made a improvement?</br>

Don't keep it to yourself. Make a pull request.

https://github.com/Xonar/LGBinExtractor/pull/new/master

###HALL OF HELPERS
 - navossoc : for his tool that extracts the kdz and wdb files.
 - SnowLeopardJB : for lots and lots of testing

##Bugs
Found a bug? Please report it!

https://github.com/Xonar/LGBinExtractor/issues

I'll do my best to fix it as quickly as possible.

###Known bugs
 - Doesn't work with Big Endian Systems
 - Some partitions require that they are their full size (see Note 1)

##Support
If there isn't support for your phone just ask. I'll be glad to include it. As I don't own many of these phones (Just the P970 is mine) and my bandwidth is limited I need some info from you if I am to add support.

######EMail or msg me on XDA with the following info:
- Phone Model
- What firmware you want to extract
- The first meg of the firmware file (Theres a option in the tool for that now)</br>
 - It is rare that I can actually download the very large firmware files so this is important</br>
 - Any hosting site will be sufficient. You can also post in the thread and attach the file there.
- Your phone partition info</br>
 - the output from 'cat /proc/partitions' is usually sufficient
- All known partition starting points within the file.

You should also be prepared for some correspondence so I can use you to test if it actually works. I am doing it for you after all.
    	
This tool is pretty picky when it comes to magic checking. I'll add support for forcing a certain magic and ignoring magic checking later. I have a life of my own after all.

###CONFIRMED WORKING FOR :
See Note 2 and Support

#####BIN:
 - P970
 - KU5900
 - AS730
 - P940
 - LG Lucid

#####TOT:
- Optimus G (Sprint,AT&T,Bell)
- Optimus G Pro
- Nexus 4
- MS770

###TESTING/PLANNED NEAR FUTURE
- P990

##CONTACT INFO
E-Mail : xonar.leroux@gmail.com</br>
XDA	: http://forum.xda-developers.com/member.php?u=4669225

If you want common info then please ask it on the thread and don't message me directly.
If you want to make a contribution just message me with what you want to do or make a pull request with what you've done.
