# Code samples from Windows Kernel Programming book by Pavel Yosifovich
I use the older version of the book (2019)

Newer one already published 
## Some useful staff, which should be known before deploying
- So that you woun't need VS libraries on verial machine, you can build your driver in debug version (just if you need DbgPrint output) and the user-mode app in release version
- In my case the most convenient way, which takes less time, is to create a shared folder (because I use VS on my host machine) and when I need to debug a driver, my windbg simply looks for a pdb in the same folder where the file was built, also the source code is near there. And I don't need to copy exe or sys to my machine, because obviously the vm just need to update the shared folder with project
