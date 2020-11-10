# Lab 8 Report

Danny Kaehn & James Rector  
CS 3841 011  
11/5/2020  
Fun with File Systems

## Introduction

blah

## Build Instructions

blah

## Analysis

### Create and Mount a RAM disk

1. View the man page for free with “man free”.  What does each column mean?  In your own words, what is the difference between free memory and available memory?
    - the first column is the memory available to the OS and user processes, and the second column shows the memory available and used by the Kernel. Available is the total memory that the os/user processes can use, wheras free shows the unalocatd/unused ram left.
2. View the man page for mount with “man mount”.  Use the information in the man page to record which file system types your Linux installation supports.
    - according to fylesystems in proc as directed by the man page we have: ext2, ext3, ext4, squashfs, vfat, fuseblk, and iso9660.(when omiting nodev)
3. Pick one of the file systems supported by our Linux installation and learn more about how it works (use google to help).  Give a brief summary of the file system.  Document your resources.
    - squashfs is a compressed read only file system. it was introduced in 2002 and has a max file size of 2^64. it supported block sizes from 4KiB to 1MiB. origonaly it used gzip compression but they added support for LZMA, LZo, LZMA2, LZ4, and Zstandard. [source](en.wikipedia.org/wiki/SquashFS)
4. Create the RAM disk as outlined above.  What do each of the parameters to “mount” mean?
    - `-t tempfs` specifies the filesystem type, `-o` begins a potential comma-separated list of options, specifying the size of the disk to be 512 Mb with `size=512m`, `tempfs` speficies the image file (in general, the device) to use, and `/media/ramdisk` specifies the mounting directory.
5. Record the output of free -h after creation of the RAM disk.  What is different?  Why?
    - The output shows the free memory decreasing by 0.5 Gi, which matches the size of our allocated RAM disk. Used memory also increased by 0.1 Gi, implying a fair amount of overhead in mounting the disk.

    ```bash
                  total        used        free      shared  buff/cache   available
    Mem:          5.8Gi       1.4Gi       2.3Gi        76Mi       2.1Gi       4.2Gi
    Swap:         735Mi          0B       735Mi
    ```

6. Based on the observation of the free command before adding the ram disk, and after adding it and adding files to it, in which category of memory does the ram drive appear to reside
    - It appears to reside in the buf/cache category, as that category increased by 0.4 Gi (the full size of 0.5 Gi minus the 0.1 Gi of used overhead).
7. Record the status of the RAM disk with the command df -h.  What can you learn about the file system by running df?
    - It appears that the size, used memory, available memory, how used the disk currently is, and its mounting directory can be ascertained from running df. For this RAM disk, this shows that all of the memory in the disk is free, corresponding to 0% usage.
  
    ```bash
    Filesystem      Size  Used Avail Use% Mounted on
    ...
    tmpfs           512M     0  512M   0% /media/ramdisk
    ```

8. Copy or download a large file into the RAM disk.  I suggest a large pdf file (maybe around 25M).  Interact with this file (such as opening a pdf with a viewer) in the RAM disk as well as a copy on the normal file system.  What differences do you notice (e.g. performance, etc.)?
    - It could be said that loading the PDF and navigating through it was _arguably_ faster, though the difference was not overly noticeable (the PDF was 25M in size).
9. Record the output of free -h and df -h now that the RAM disk has a file in it.  How does the output compare to what you recorded before?  Does the output make sense?  Why?
    - As seen from `free`, Available memory decreased by 0.2 Gi, which corresponse to roughly the size of the PDF downloaded into the disk. In correspondance, buff/cache used memory increased by 0.2 Gi as well. As seen from `df`, the disk now has 5% use and has 25M of it's memory used, as is expected.

    ```bash
                  total        used        free      shared  buff/cache   available
    Mem:          5.8Gi       1.5Gi       2.0Gi       105Mi       2.3Gi       4.0Gi
    Swap:         735Mi          0B       735Mi
    ```

    ```bash
    Filesystem      Size  Used Avail Use% Mounted on
    ...
    tmpfs           512M   25M  488M   5% /media/ramdisk
    ```

10. Record the contents /etc/fstab.  What does each entry mean?  Consult “man fstab” for help.
    - The top two entries correspond to ext4 and vfat virtual drive partitions that the system has access to. In particular, the first one seems to correspond to the filesystem, and the second one seems to correspond to the bootable linux partition. The /swapfile entry mounts the swapfile, equivalent to the pagefile in Windows used for storing overflowed memory pages on disk.

    ```text
    # /etc/fstab: static file system information.
    #
    # Use 'blkid' to print the universally unique identifier for a
    # device; this may be used with UUID= as a more robust way to name devices
    # that works even if disks are added and removed. See fstab(5).
    #
    # <file system> <mount point>   <type>  <options>       <dump>  <pass>
    # / was on /dev/sda5 during installation
    UUID=af31fa01-b485-402d-95c5-ec377b307381 /               ext4    errors=remount-ro 0       1
    # /boot/efi was on /dev/sda1 during installation
    UUID=9B7E-1EB4  /boot/efi       vfat    umask=0077      0       1
    /swapfile                                 none            swap    sw              0       0
    ```

11. What would an entry in /etc/fstab need to look like in order to create the 512MB tmpfs RAM disk when the system boots?
    - tmpfs    /media/ramdisk    tmpfs      size=512m      0 0

### Mount a file as a file system

1. Where are the mounted partitions of ‘/dev/sda1’ mounted?  Why do suspect that two partitions are used?  What is an advantage of partitioning the disk in this way?

    - //todo

2. Look at the image file with hexdump (hexdump –C flash.img | more).  Can you find your files?  How about the ones you deleted?  How did you find them or why couldn’t you?

    - Yes, the files are all visible. At the top, all of the filenames and metadata about the files can be found, even that of the deleted files. The file contents can then be found at later positions within the image (again including the deleted files).

3. Show a capture of the hexdump of the directory entries for the files you added to the image as well as the ones you added and deleted.  What is different?

    - Files test.txt, test2.txt, test3.txt, and test4.txt were all files copied to the drive. test.txt and test2.txt were deleted. As can be seen from the capture below, the directory entry for the file has the first letter of the file replaced with a '.', for example the entry for 'test2.txt' at the top was replaced with '.EST2'.

    ``` bash
    *
    00009e00  e5 74 00 65 00 73 00 74  00 32 00 0f 00 77 2e 00  |.t.e.s.t.2...w..|
    00009e10  74 00 78 00 74 00 00 00  ff ff 00 00 ff ff ff ff  |t.x.t...........|
    00009e20  e5 45 53 54 32 20 20 20  54 58 54 20 00 58 c1 9a  |.EST2   TXT .X..|
    00009e30  6a 51 6a 51 00 00 c1 9a  6a 51 03 00 06 00 00 00  |jQjQ....jQ......|
    00009e40  41 74 00 65 00 73 00 74  00 33 00 0f 00 d3 2e 00  |At.e.s.t.3......|
    00009e50  74 00 78 00 74 00 00 00  ff ff 00 00 ff ff ff ff  |t.x.t...........|
    00009e60  54 45 53 54 33 20 20 20  54 58 54 20 00 58 c1 9a  |TEST3   TXT .X..|
    00009e70  6a 51 6a 51 00 00 c1 9a  6a 51 04 00 06 00 00 00  |jQjQ....jQ......|
    00009e80  41 74 00 65 00 73 00 74  00 34 00 0f 00 1f 2e 00  |At.e.s.t.4......|
    00009e90  74 00 78 00 74 00 00 00  ff ff 00 00 ff ff ff ff  |t.x.t...........|
    00009ea0  54 45 53 54 34 20 20 20  54 58 54 20 00 58 c1 9a  |TEST4   TXT .X..|
    00009eb0  6a 51 6a 51 00 00 c1 9a  6a 51 05 00 06 00 00 00  |jQjQ....jQ......|
    00009ec0  42 6d 00 65 00 65 00 65  00 2e 00 0f 00 71 74 00  |Bm.e.e.e.....qt.|
    00009ed0  78 00 74 00 00 00 ff ff  ff ff 00 00 ff ff ff ff  |x.t.............|
    00009ee0  01 74 00 65 00 73 00 74  00 6c 00 0f 00 71 6f 00  |.t.e.s.t.l...qo.|
    00009ef0  6f 00 6f 00 6f 00 6e 00  67 00 00 00 6e 00 61 00  |o.o.o.n.g...n.a.|
    00009f00  54 45 53 54 4c 4f 7e 31  54 58 54 20 00 58 c1 9a  |TESTLO~1TXT .X..|
    00009f10  6a 51 6a 51 00 00 c1 9a  6a 51 06 00 13 00 00 00  |jQjQ....jQ......|
    00009f20  e5 74 00 65 00 73 00 74  00 2e 00 0f 00 8f 74 00  |.t.e.s.t......t.|
    00009f30  78 00 74 00 00 00 ff ff  ff ff 00 00 ff ff ff ff  |x.t.............|
    00009f40  e5 45 53 54 20 20 20 20  54 58 54 20 00 58 c1 9a  |.EST    TXT .X..|
    00009f50  6a 51 6a 51 00 00 c1 9a  6a 51 07 00 06 00 00 00  |jQjQ....jQ......|
    00009f60  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
    *
    ```

4. Show a capture of the hexdump of a portion of the file contents within the image of a file you deleted.  What do you observe?

    - It appears that noghting has changed in the contents of the file. Text file test2.txt contained the text "test2" and this text is still found on the image. This implies that deleting files does not zero out the bytes that contain the file contents.

    ```bash
    *
    0000e000  74 65 73 74 32 0a 00 00  00 00 00 00 00 00 00 00  |test2...........|
    0000e010  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
    *
    ```

5. Change directory to the directory that contains the file.  Try to unmount the file system with “umount /media/myimage”.  Did it work successfully?  Why or why not?

    - This operation failed with the message `umount: /media/myimage: target is busy.`. This is presumably because of the terminal accessing the mounted image while trying to unmount the file, similar to trying to delete a file while it is open in another application. Changing directories from the mounted image allowed the file to be unmounted.

### Teeny Tiny File System

1. What is the largest file that TTFS can support?  How did you compute this number?

    - //todo

2. How may inodes and data blocks are in the file system?

    - //todo

3. How many inodes are free (not used by files or directories?

    - //todo

4. Draw a diagram of the directory hierarchy, include this diagram with your submission.

    - //todo

5. What are the names and sizes of each file in the file system?

    - //todo

6. How many blocks is each file in the file system?

    - //todo

7. For each file, what is the offset (from the start of the file system) for each data block?  How did you compute this?

    - //todo

8. Find one of the text files in the file system.  Write the data (as a string) for the file contents.  How were you able to find the data?

    - //todo

## Conclusion
