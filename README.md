# Code samples from Windows Kernel Programming book by Pavel Yosifovich
I use the older version of the book (2019)

Newer one already published 
## Some useful staff, which should be known before deploying
- So that you woun't need VS libraries on your virtual machine, you can build your driver in debug version for your .sys file (just if you need DbgPrint output) and the user-mode app for your client .exe file in release version
- In my case the most convenient way, which takes less time, is to create a shared folder (because I use VS on my host machine) and when I need to debug a driver, my windbg simply looks for a pdb in the same folder where the file was built, also the source code is near there. And I don't need to copy exe or sys to my machine, because obviously the vm just need to update the shared folder with project
- In automatic_scripts folder you can find bat scripts to create/start/stop/delete/query drivers. Just more handy than OSR Driver Loader for me
