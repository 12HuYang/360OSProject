#include "functions.h"

/*extern MINODE minodes[NMINODE];
extern MINODE *root;
extern PROC   proc[NPROC], *running;

extern char names[64][128],*name[64];
extern int fd,dev, nblocks, ninodes, bmap, imap, inode_start;
extern char line[256],cmd[32],pathname[32];*/



int pwd(MINODE* cwd){

}

ls(char *pathname){
    int ino, dev = running->cwd->dev;
    char lbuf[BLKSIZE],*cp;
    MINODE *mip = running->cwd;

    if (pathname){   // ls pathname:
        if (pathname[0]=='/')
            dev = root->dev;
        ino         = getino(&dev, pathname);
        MINODE *mip = iget(dev, ino);
    }
    int i;
    INODE *lip=&mip->INODE;
    for(i=0;i<12;i++)
    {
        get_block(dev,lip->i_block[i],lbuf);
        dp=(DIR*)lbuf;
        cp=(char*)lbuf;
        while(*cp)
        {
            char *word=dp->name;
            mystat(word);
            cp+=dp->rec_len;
            dp=(DIR *) cp;
        }

    }

    // mip points at minode;
    // Each data block of mip->INODE contains DIR entries
    // print the name strings of the DIR entries
}

int _chdir(char* pathname)
{
    MINODE* mip;
    //get INODE of pathname into a minode
    if(pathname)
    {
        int ino=getino(&dev,pathname);
        if(ino)
        {
            mip=iget(dev,ino);
            INODE *mino=&(mip->INODE);
            if(mino->i_mode & 0xF000 != 0x0100)//varifiy if it is a dir
            {
                printf("%s is not a dir.\n",pathname);
            }
            //change running process CWD to minode pathname
            iput(running->cwd);
            running->cwd=mip;

        }
        else{
            printf("could not get inode of %s",pathname);
        }
    }
    else{
        //chdir to root
        iput(running->cwd);
        running->cwd=root;
    }

}

get_stat(struct stat *mystat,char* pathname)
{
    INODE* myino;
    int inonum;
    inonum=getino(&dev,pathname);
    if(inonum)
    {
        myino=iget(dev,inonum);
        mystat->st_dev=dev;
        mystat->st_ino=inonum;
        mystat->st_mode=myino->i_mode;
        mystat->st_nlink=myino->i_links_count;
        mystat->st_uid=myino->i_uid;
        mystat->st_gid=myino->i_gid;
        mystat->st_size=myino->i_size;
        mystat->st_blksize=BLKSIZE;
        mystat->st_blocks=myino->i_blocks;
        mystat->st_atim=myino->i_atime;
        mystat->st_ctim=myino->i_ctime;
        mystat->st_mtim=myino->i_mtime;

    }

}

mystat(char* pathname)
{
    struct stat mystat;
    //int ino;
    //MINODE *lino;
    //ino=getino(&dev,pathname);

    //if(ino)
    //{
        //lino=iget(dev,ino);
        get_stat(&mystat,pathname);
        printf("----stat of %s----\n",pathname);
        printf("Device: %d\n",(int)mystat.st_dev);
        printf("INODE: %d\n",(int)mystat.st_ino);
        printf("Permissions:");
        char file_type[11]={0};
        strcpy(file_type,"--------------");
        switch (mystat.st_mode & S_IFMT)
        {
            case S_IFSOCK :
                file_type[0] = 's';
                break;
            case S_IFLNK :
                file_type[0] = 'l';
                break;
            case S_IFBLK :
                file_type[0] = 'b';
                break;
            case S_IFDIR :
                file_type[0] = 'd';
                break;
            case S_IFCHR :
                file_type[0] = 'c';
                break;
            case S_IFIFO :
                file_type[0] = 'f';
                break;
            default :
                break;
        }
        if (mystat.st_mode & S_IRUSR)
        {
            file_type[1] = 'r';
        }
        if (mystat.st_mode & S_IWUSR)
        {
            file_type[2] = 'w';
        }
        if (mystat.st_mode & S_IXUSR)
        {
            file_type[3] = 'x';
        }
        if (mystat.st_mode & S_IRGRP)
        {
            file_type[4] = 'r';
        }
        if (mystat.st_mode & S_IWGRP)
        {
            file_type[5] = 'w';
        }
        if (mystat.st_mode & S_IXGRP)
        {
            file_type[6] = 'x';
        }
        if (mystat.st_mode & S_IROTH)
        {
            file_type[7] = 'r';
        }
        if (mystat.st_mode & S_IWOTH)
        {
            file_type[8] = 'w';
        }
        if (mystat.st_mode & S_IXOTH)
        {
            file_type[9] = 'x';
        }
        printf("%s \n",file_type);
        printf("Links: %d\n",mystat.st_nlink);
        mystat->st_uid=myino->i_uid;
        mystat->st_gid=myino->i_gid;
        mystat->st_size=myino->i_size;
        mystat->st_blksize=BLKSIZE;
        mystat->st_blocks=myino->i_blocks;
        mystat->st_atim=myino->i_atime;
        mystat->st_ctim=myino->i_ctime;
        mystat->st_mtim=myino->i_mtime;
        printf("uid: %d\n",mystat.st_uid);
        printf("id: %d\n",mystat.st_gid);
        printf("Size: %d\n",mystat.st_size);
        printf("Blocksize: %d\n",mystat.st_blksize);
        printf("Num of blocks: %d",mystat.st_blocks);
        printf("aTime: %s\n",ctime(&mystat.st_atim));
        printf("cTime: %s\n",ctime(&mystat.st_ctim));
        printf("mTime: %s\n",ctime(&mystat.st_mtim));
        printf("\n");
        iput(lino);
    //}



}

int quit()
{
    //iput all DIRTY minodes before shutdown
    int i;
    MINODE *mip;
    for(i=0;i<NMINODES;i++) {
        mip=&minode[i];
        if(mip->refCount>0)
            iput(mip);
    }
    exit(0);
}