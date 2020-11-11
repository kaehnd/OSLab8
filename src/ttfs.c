/**
 * Danny Kaehn & James Rector
 * CS 3841-011
 * Lab 8: Fun with File Systems
 * 
 * ttfs.c implements ttfsdump utility to print out contents of ttfs image
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ttfs.h"

#define INITIAL_DIR_ALLOC 5
#define INITIAL_FILE_ALLOC 10

typedef struct
{
    char *name;
    char *contents;
    int size;
} ttfsFile;

typedef struct ttfsDir
{
    char *name;
    int numFiles;
    int numDirectories;
    ttfsFile **files;
    struct ttfsDir **dirs;
} ttfsDir;

static ttfs_super_block *superBlock;

//loads contents of a file represented by given inode
static ttfsFile *loadFile(ttfs_inode *fileInode, FILE *ttfsImg)
{
    ttfsFile *curFile = malloc(sizeof(ttfsFile));

    int nameLength = strlen(fileInode->file_name);
    curFile->name = malloc((nameLength + 1) * sizeof(char));
    strcpy(curFile->name, fileInode->file_name);

    curFile->contents = malloc(fileInode->file_size + 1);
    curFile->contents[fileInode->file_size] = 0;
    curFile->size = fileInode->file_size;

    int sizeRemaining = fileInode->file_size;

    for (int i = 0; i < fileInode->block_count; i++)
    {
        fseek(ttfsImg, superBlock->offset_to_data_blocks + BLOCK_SIZE * fileInode->blocks[i], SEEK_SET);

        int bytesToRead = sizeRemaining > BLOCK_SIZE ? BLOCK_SIZE : sizeRemaining;

        fread(&(curFile->contents[i * BLOCK_SIZE]), bytesToRead, 1, ttfsImg);

        sizeRemaining -= bytesToRead;
    }
    return curFile;
}

//Recursively load a directory, its files, and subdirectories
static ttfsDir *loadDirectory(ttfs_inode *dirInode, FILE *ttfsImg)
{
    ttfsDir *curDir = malloc(sizeof(ttfsDir));
    curDir->numFiles = 0;
    curDir->numDirectories = 0;

    int nameLength = strlen(dirInode->file_name);
    curDir->name = malloc((nameLength + 1) * sizeof(char));
    strcpy(curDir->name, dirInode->file_name);

    int dirAlloc = INITIAL_DIR_ALLOC;
    int fileAlloc = INITIAL_FILE_ALLOC;

    curDir->dirs = malloc(sizeof(ttfsDir *) * dirAlloc);
    curDir->files = malloc(sizeof(ttfsFile *) * fileAlloc);

    for (int i = 0; i < dirInode->block_count; i++)
    {
        fseek(ttfsImg, superBlock->offset_to_data_blocks + BLOCK_SIZE * dirInode->blocks[i], SEEK_SET);
        ttfs_directory_block *dirBlock = malloc(sizeof(ttfs_directory_block));
        fread(dirBlock, sizeof(ttfs_directory_block), 1, ttfsImg);

        for (uint32_t j = 0; j < dirBlock->file_count; j++)
        {
            fseek(ttfsImg, superBlock->offset_to_inode_table + sizeof(ttfs_inode) * dirBlock->file_inodes[j], SEEK_SET);
            ttfs_inode *curNode = malloc(sizeof(ttfs_inode));
            fread(curNode, sizeof(ttfs_inode), 1, ttfsImg);

            if (curNode->is_directory)
            {
                if (curDir->numDirectories++ > dirAlloc)
                {
                    dirAlloc *= 2;
                    curDir->dirs = realloc(curDir->dirs, sizeof(ttfsDir *) * dirAlloc);
                }
                curDir->dirs[curDir->numDirectories - 1] = loadDirectory(curNode, ttfsImg);
            }
            else
            {
                if (curDir->numFiles++ > fileAlloc)
                {
                    fileAlloc *= 2;
                    curDir->files = realloc(curDir->files, sizeof(ttfsFile *) * fileAlloc);
                }
                curDir->files[curDir->numFiles - 1] = loadFile(curNode, ttfsImg);
            }
            free(curNode);
        }
        free(dirBlock);
    }

    curDir->dirs = realloc(curDir->dirs, curDir->numDirectories * sizeof(ttfsDir *));
    curDir->files = realloc(curDir->files, curDir->numFiles * sizeof(ttfsFile *));

    return curDir;
}

//Recursively print the files contained in a directory and its subdirectories
static void printDir(ttfsDir *curDir, char *curPath)
{

    for (int i = 0; i < curDir->numFiles; i++)
    {
        printf("%s/%s.txt - %d bytes\n%s\n\n", curPath, curDir->files[i]->name, curDir->files[i]->size, curDir->files[i]->contents);
    }

    for (int i = 0; i < curDir->numDirectories; i++)
    {
        char * newPath = malloc(strlen(curPath) + strlen(curDir->dirs[i]->name) + 2);
        sprintf(newPath, "%s/%s", curPath, curDir->dirs[i]->name);
        printDir(curDir->dirs[i], newPath);
        free(newPath);
    }
}

//Recursively free a directory, its files, and its subdirectories
static void freeDir(ttfsDir *curDir)
{
    for (int i = 0; i < curDir->numFiles; i++)
    {
        ttfsFile * file = curDir->files[i];
        free(file->contents);
        free(file->name);
        free(file);
    }
    for (int i = 0; i < curDir->numDirectories; i++)
    {
        freeDir(curDir->dirs[i]);
    }
    free(curDir->dirs);
    free(curDir->files);
    free(curDir->name);
    free(curDir);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Too few arguments, must specify a filename\n");
        return 1;
    }

    char *ext = strrchr(argv[1], '.');
    if (strcmp(".iso", ext) != 0 && strcmp(".img", ext) != 0)
    {
        fprintf(stderr, "File provided was not .iso or .img\n");
        return 1;
    }

    FILE *ttfsImg = fopen(argv[1], "r");

    if (ttfsImg == 0)
    {
        fprintf(stderr, "File could not be read\n");
        return 1;
    }

    superBlock = malloc(sizeof(ttfs_super_block));
    fread(superBlock, sizeof(ttfs_super_block), 1, ttfsImg);

    if (superBlock->ttfs_magic[0] != 'T' ||superBlock->ttfs_magic[1] != 'T' ||
        superBlock->ttfs_magic[2] != 'F' ||superBlock->ttfs_magic[3] != 'S')
    {
        fprintf(stderr, "Image was not in a proper TTFS format\n");
        return 1;
    }

    fseek(ttfsImg, superBlock->offset_to_inode_table, SEEK_SET);
    ttfs_inode *rootInode = malloc(sizeof(ttfs_inode));
    fread(rootInode, sizeof(ttfs_inode), 1, ttfsImg);

    ttfsDir * rootDir = loadDirectory(rootInode, ttfsImg);
    fclose(ttfsImg);
    free(rootInode);

    printDir(rootDir, "");

    freeDir(rootDir);
    free(superBlock);
}
