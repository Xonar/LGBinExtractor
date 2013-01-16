LGBinExtractor
==============

A tool for extracting bin firmware for LG Phones

http://forum.xda-developers.com/showthread.php?t=1879915

READ THIS BEFORE ANYTHING ELSE
------------------------------
If there isn't support for your phone just ask. I'll be glad to include it. As I don't own many of these phones (Just the P970 is mine) and my bandwidth is limited I need some info from you if I am to add support.

EMail or msg me on XDA with the following info:
	Phone Model
	What firmware you want to extract
	Where can download the firmware
	The first meg of the firmware file
	    -It is rare that I can actually download the very large firmware files so this is important.
	    -Any hosting site will be sufficient. You can also post in the thread and attach the file there.
	Your phone partition info
	    -the output from 'cat /proc/mounts' is usually sufficient
	All known partition starting points.

You should also be prepared for some correspondence so I can use you to test if it actually works. I am doing it for you after all.
		
This tool is pretty picky when it comes to magic checking. I'll add support for forcing a certain magic and ignoring magic checking later. I have a life of my own after all.

CONFIRMED WORKING FOR :
-----------------------
P970
KU5900
AS730

TESTING/PLANNED NEAR FUTURE
---------------------------
Optimus G
P990 (*.tot file)

CONTACT INFO
------------
E-Mail	: xonar.leroux@gmail.com
XDA	: http://forum.xda-developers.com/member.php?u=4669225

If you want common info then please ask it on the thread and don't message me directly.
If you want to make a contribution just message me with what you want to do or make a pull request with what you've done.
