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
    - The first entry corresponds to the actual Finally, the /swapfile entry mounts the swapfile, equivalent to the pagefile in Windows used for storing overflowed memory pages on disk.

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

2. Look at the image file with hexdump (hexdump –C flash.img | more).  Can you find your files?  How about the ones you deleted?  How did you find them or why couldn’t you?

3. Show a capture of the hexdump of the directory entries for the files you added to the image as well as the ones you added and deleted.  What is different?

4. Show a capture of the hexdump of a portion of the file contents within the image of a file you deleted.  What do you observe?

5. Change directory to the directory that contains the file.  Try to unmount the file system with “umount /media/myimage”.  Did it work successfully?  Why or why not?

## Conclusion

blah
