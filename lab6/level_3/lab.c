/****************lab6.c*************************/
#include "type.h"

MINODE minode[NMINODE];
MINODE *root;
PROC   proc[NPROC], *running;
OFT oft[NOFT]; 
MOUNT mount[NMOUNT];

char names[64][128],*name[64];
int fd,dev, nblocks, ninodes, bmap, imap, inode_start,pathcount;
char line[256],cmd[32],pathname[32],new[32];//buf[BLKSIZE];

#include "util.c"
#include "alloc_dealloc.c"
#include "level_1/getcwd.c"
#include "level_1/cd.c"
#include "level_1/mkdir_creat.c"
#include "level_1/rmdir.c"
#include "level_1/link_unlink.c"
#include "level_1/unlink.c"
#include "level_1/symlink.c"
#include "level_1/touch.c"
#include "level_1/chmod.c"
#include "level_1/ls.c"
#include "level_1/rm.c"
#include "level_2/open_close_lseek.c"
#include "level_2/read_file.c"
#include "level_2/write_file.c"
#include "level_2/cp.c"
#include "level_2/mv.c"



void init() // Initialize data structures of LEVEL-1:
{
    //(1). 2 PROCs, P0 with uid=0, P1 with uid=1, all PROC.cwd = 0
    // (2). MINODE minode[100]; all with refCount=0
    //(3). MINODE *root = 0;
    int i,j;
    MINODE *mip;
    PROC *p;
    MOUNT *mountp;

    printf("init()\n");

    for(i=0;i<NMINODE;i++)
    {
        mip=&minode[i];
        mip->dev=mip->ino=0;
        mip->refCount=0;
        mip->mounted=0;
        mip->mptr=0;
    }

    for(i=0;i<NPROC;i++)
    {
        p=&proc[i];
        p->pid=i;
        p->uid=0;
        p->cwd=0;
        p->status=FREE;
        for(j=0;j<NFD;j++)
            p->fd[j]=0;
    }

   for(i=0;i<NOFT;i++)
   {
       oft[i].refCount=0;
   }

   for(i=0;i<NMOUNT;i++)
   {
       mountp=&mount[i];
       mountp->dev=0;
       mountp->ninodes=0;
       mountp->nblocks=0;
       mountp->mptr=0;
       mountp->bmap=0;
       mountp->imap=0;
       mountp->iblk=0;
   }


}


void mount_root()// mount root file system, establish / and CWDs
{
    printf("mount_root()\n");
    root=iget(dev,2);
}


char* disk="mydisk";

int main(int argc,char *argv[])
{
    int ino;
    char buf[BLKSIZE];

    if(argc>1)
        disk=argv[1];

    printf("checking EXT2 FS.....");
    if((fd=open(disk,O_RDWR))<0)
    {
        printf("open %s failed\n",disk);
        exit(1);
    }

    dev=fd;

    get_block(dev,1,buf);  //read super block
    sp=(SUPER*)buf;
    //verify it's an ext2 file system
    if(sp->s_magic!=0xEF53){
        printf("magic = %x is not an ext2 file system.\n");
        exit(1);
    }
    printf("OK\n");
    ninodes=sp->s_inodes_count;
    nblocks=sp->s_blocks_count;

    get_block(dev,2,buf); //get group block
    gp=(GD*)buf;

    bmap=gp->bg_block_bitmap;
    imap=gp->bg_inode_bitmap;
    inode_start=gp->bg_inode_table;
    printf("bmp=%d imap=%d inode_start=%d \n",bmap,imap,inode_start);

    printf("ninodes=%d nblocks=%d \n",ninodes,nblocks);

    init();
    mount_root();
    printf("root refCount = %d\n",root->refCount);

    printf("creating P0 as running process.\n");
    running=&proc[0];
    running->status=READY;
    running->cwd=iget(dev,2);
    printf("root refCount = %d\n",root->refCount);

    printf("Press a key to continue: ");getchar();

    //char line[256];
    while(1)
    {
        root=iget(dev,2);
    //    printf("********  Commands  *********\n");
        printf("[ls|cd|pwd|stat|mkdir|rmdir|creat|link|unlink|symlink|touch|chmod|rm|cat|cp|mv|quit]\n");
    //    printf("********  Commands  *********\n");
        printf("input a line : ");
        bzero(line, 256);                // zero out line[ ]
        fgets(line, 128, stdin);
        line[strlen(line)-1] = 0;        // kill \n at end

        if(line[0]==0)
            continue;
        pathname[0]=0;

        sscanf(line, "%s %s %s",cmd,pathname,new);
        printf("cmd = %s pathname = %s\n",cmd,pathname);

        if(!strcmp(cmd,"ls"))
            ls(pathname);
        if(!strcmp(cmd, "cd"))
            _chdir(&pathname);
        if(!strcmp(cmd,"stat"))
            mystat(&pathname);
        if(!strcmp(cmd,"pwd"))
            getcwd();
	if(!strcmp(cmd,"mkdir"))
	    _mkdir(&pathname);
        if(!strcmp(cmd,"rmdir"))
            _rmdir(pathname);
        if(!strcmp(cmd,"creat"))
            _creat(&pathname);
        if(!strcmp(cmd,"link"))
        {
          // sscanf(line,"%s %s %s",cmd,pathname,new);
           printf("cmd = %s old = %s new = %s\n",cmd,pathname,new);
           link(&pathname,&new);
        }
        if(!strcmp(cmd,"unlink"))
           _unlink(pathname);
        if(!strcmp(cmd,"symlink"))
        {
           printf("cmd = %s old = %s new = %s\n",cmd,pathname,new);
           _symlink(pathname,new);
        }
	if(!strcmp(cmd,"touch"))
	{
	   _touch(pathname);
	}
	if(!strcmp(cmd,"chmod"))
	{
	   printf("cmd = %s para = %s pathname = %s\n",cmd,pathname,new);
           _chmod(pathname,new);
	}  
        if(!strcmp(cmd,"rm"))
        {
	    _rm(pathname);
        }
        if(!strcmp(cmd,"cat"))
	  _cat(pathname); 
        if(!strcmp(cmd,"cp"))
        {
	   printf("cmd = %s source = %s dest = %s\n",cmd,pathname,new);
           _cp(pathname,new);
        }
        if(!strcmp(cmd,"mv"))
        {
	   printf("cmd = %s source = %s dest = %s\n",cmd,pathname,new);
           _mv(pathname,new);   	
        }
        if(!strcmp(cmd,"quit"))
            quit();
    }

}

