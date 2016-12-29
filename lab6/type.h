/*************** type.h file ******************/
/* type.h is ALSO available in samples/type.h  */
#include <stdio.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>   // NOTE: Ubuntu users MAY NEED "ext2_fs.h"
#include <libgen.h>
#include <string.h>
#include <sys/stat.h>

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

typedef struct ext2_super_block SUPER;
typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;

SUPER *sp;
GD    *gp;
INODE *ip;
DIR   *dp;

#define FREE        0
#define READY       1
#define RUNNING     2

#define BLKSIZE  1024

//Block number of EXT2 FS on FD

#define NMINODE   100
#define NFD        10
#define NPROC      10
#define NOFT      100
#define NMOUNT     10

char buf[BLKSIZE];

typedef struct minode{
  INODE INODE;
  int dev, ino;
  int refCount;
  int dirty;
  int mounted;
  struct MOUNT *mptr;
}MINODE;

typedef struct oft{
  int  mode;
  int  refCount;
  MINODE *mptr;
  int  offset;
}OFT;

typedef struct proc{
  int          pid;
  int          ppid;
  int          status;
  int          uid, gid;
  MINODE      *cwd;
  OFT         *fd[NFD];
}PROC;

// In-memory inodes structure
typedef struct mount{
  int ninodes;
  int nblocks;
  int dev;  //busy DOUBLE CHECK
  int bmap,imap,iblk;
  MINODE* mptr;
  char name[64];
  char mount_name[64];
}MOUNT;
//================= end of type.h ===================
