#include "type.h"

typedef unsigned int   u32;

/*extern SUPER *sp;
extern GD    *gp;
extern INODE *ip;
extern DIR   *dp;*/

MINODE minodes[NMINODE];
MINODE *root;
PROC   proc[NPROC], *running;

char names[64][128],*name[64];
int fd,dev, nblocks, ninodes, bmap, imap, inode_start;
char line[256],cmd[32],pathname[32];//buf[BLKSIZE];

int get_block(int fd, int blk, char buf[]);
int put_block(int fd, int blk, char buf[]);

u32 inosearch(INODE *iptr,char *name);
int getino(int *dev,char* pathname);

MINODE* searchMinode(int dev, int ino);
MINODE* findMinode(int dev, int ino);

MINODE* iget(int dev, int ino);  //return MINODE*
MINODE* iput(MINODE* mip);  //release a minode[] pointed by mip