/**************util.c************************/

int get_block(int fd, int blk, char buf[]);
int put_block(int fd, int blk, char buf[]);

int search(MINODE *iptr,char *name); //return a dir or file inode number by given minode and file/dir name
int getino(int *dev,char* pathname);

MINODE* iget(int dev, int ino);  //return MINODE*
void iput(MINODE* mip);

void get_stat(struct stat *mystat,char* pathname);
void mystat(char* pathname);

int findcpino(MINODE* mip,int* chino,int* pino,int* pdev,int* cdev);
int getinoname(MINODE* pmid,int chino,char* inoname);

int quit();

int findcpino(MINODE* mip,int* chino,int* pino,int* pdev,int* cdev)
{
    char *cp;//*temp;
    char lbuf[BLKSIZE];
    //int i;

    INODE *lip=&mip->INODE;
    //printf("lip blocks %d\n",lip->i_blocks);

    if(lip->i_block[0]==0) return -1;
    if(mip==root) return -1;
    //printf("findcpino() mip dev=%d,ino=%d\n",mip->dev,mip->ino);
    get_block(mip->dev,lip->i_block[0],lbuf);
    dp=(DIR*)lbuf;
    cp=lbuf;
    *chino=dp->inode;
    *cdev=mip->dev;
    //printf("cino=%d cdev=%d \n",*chino,*cdev);
    cp+=dp->rec_len;
    dp=(DIR*)cp;
    *pino=dp->inode;
    *pdev=mip->dev;
    if(*pino==2)
    {
      MOUNT* fmpt=&mount[0];
      if(mip->dev!=fmpt->dev)
      {
	//printf("inonum = %d, dev=%d, true dev=%d\n",*pino,mip->dev,fmpt->dev);
	fmpt=&mount[1];
	mip=fmpt->mptr;
	*pino=mip->ino;
	*pdev=mip->dev;
      }
    }
   //printf("pino is %d\n",*pino);

    return 0;
}

int getinoname(MINODE* pmid,int chino,char* inoname)
{
    int i;
    char *cp;
    char lbuf[BLKSIZE];

    ip=&(pmid->INODE);
    for(i=0;i<12;i++)
    {
        if(ip->i_block[i]==0) break;
        get_block(pmid->dev,ip->i_block[i],lbuf);
        dp=(DIR*)lbuf;
        cp=lbuf;

        while(*cp)
        {
            //inoname[strlen(inoname)-1]=0;
            if(dp->inode==chino)
            {
                strncpy(inoname,dp->name,dp->name_len);
                //printf("inoname is %s",inoname);
                return 0;
            }

            cp+=dp->rec_len;
            dp=(DIR*)cp;
        }
    }
    return -1;

}



int get_block(int fd, int blk, char buf[])
{
    lseek(fd, (long)blk*BLKSIZE, SEEK_SET);
    read(fd, buf, BLKSIZE);
}

int put_block(int fd, int blk, char buf[])
{
    lseek(fd,(long)blk*BLKSIZE,SEEK_SET);
    write(fd,buf,BLKSIZE);

}

void tokenize(char* pathname)
{
    pathcount=0;
    char copy[256],tcpy[256],*temp;
    strcpy(copy,pathname);
    temp=(char*)&tcpy;


    temp=strtok(copy,"/");
    //printf("tokenize() temp:%s\n",temp);
    strcpy((char*)&names[pathcount],temp);

    //printf("tokenize() names[0]:%s\n",names[pathcount]);

    name[pathcount]=(char*)&names[pathcount];
    //printf("tokenize() name[0]:%s\n",name[pathcount]);
    pathcount++;

    //printf("count is %d\n",pathcount);
    while(temp=strtok(NULL,"/"))
    {
        strcpy(names[pathcount],temp);
       // names[pathcount][strlen(names[pathcount]-1)]=0;
        name[pathcount]=&names[pathcount];
        pathcount++;
    }
    printf("count is %d\n",pathcount);
    int i;
    for(i=0;i<pathcount;i++) printf("pathname[%d] is %s\n",i,name[i]);
}


int search(MINODE* mip,char* name)
{
    int i;
    char *cp;
    char lbuf[BLKSIZE];
    char temp[128];
    int mino,parent;
    printf("search() name is %s\n",name);
    ip=&(mip->INODE);
    dev=mip->dev;
    //dev=mip->dev;
    printf("i_number rec_len name_len name\n");
    for(i=0;i<ip->i_blocks;i++)
    {
        printf("dev = %d, i = %d, i_block = %d\n",dev,i,ip->i_block[i]);
        get_block(dev,ip->i_block[i],lbuf);
        dp=(DIR*)lbuf;
        cp=lbuf;
        while(*cp)
        {
	    memset(temp,'\0',sizeof(temp));
            strncpy(temp,dp->name,dp->name_len);
            //temp[strlen(temp)-1]=0;
            //printf("temp is %s, dpname: %s\n",temp,dp->name);
            printf("%6d ", dp->inode);
            printf("%6d ", dp->rec_len);
            printf("%6o ", dp->name_len);
            printf("%s \n ", dp->name);
            if(!strcmp(name,temp))
            {
                //printf("search checkpoint3\n");
                int dpino=dp->inode;
                MINODE* dmip=iget(dev,dpino);
                INODE* dip=&(dmip->INODE);
                if(dip->i_mode & 004000!=004000)//varify if it is a dir
                {
                    printf("search checkpoint 1\n");
                    getchar();
                    return -1;
                }
                //printf("inode for %s is %d\n",name,dp->inode);
                iput(dmip);
                return dp->inode;
            }
            cp+=dp->rec_len;
            dp=(DIR*)cp;
        }
    }
    printf("%s not exist.\n",name);
    return -1;
}

int getino(int *dev,char* pathname)  //get ino number
{
    char *token, cpypah[256];
    int inonum;
    INODE* pnext;
    strcpy(&cpypah,pathname);
    //token=strtok(cpypah,"/");
    printf("dev is %d, cpypah is %s\n",dev[0],cpypah);
    if(!strcmp(cpypah,"/"))
    {
        inonum=root->ino; //root ino
        dev[0]=root->dev;
        printf("root [%d %d]\n",dev[0],inonum);
    }
    else
    {
        inonum=running->cwd->ino;
        dev[0]=running->cwd->dev;
        printf("running cwd ino= %d,dev=%d\n",inonum,dev[0]);
        //strcpy(cpypah,pathname);
        //int count;
	if(cpypah[0]=='/')
	{
          inonum=root->ino; //root ino
          dev[0]=root->dev;
          inonum=root->ino;
          printf("root [%d %d]\n",dev[0],inonum);
	}
        tokenize(cpypah);
        //printf("back to getino\n");
        if(pathcount==0)
        {
            printf("check point 1\n");
            getchar();
            return -1;
        }
        int i;
        int cdev=dev[0];
        MINODE* mip=iget(cdev,inonum);
        printf("mip dev=%d, ino=%d\n",cdev,inonum);
	MOUNT *mountp;
	//if(mip->mounted)            
       //	{
	//    printf("1_have mounted disk: %s\n",mountp->name);
	//    mountp=mip->mptr;         //travers downward 
	//    mip=iget(mountp->dev,2);  //get root of the mounted disk
       //	}
        inonum=search(mip,name[0]);  //search has problem
        printf("inonum=%d\n",inonum);
        if(inonum==-1)
         {
            printf("%s  is not exist\n",name[0]);
            getchar();
            iput(mip);
	    return -1;
         }
        if(inonum==2&&running->cwd->ino==inonum)  //upwards ?root ino, check dev
	{
	  MOUNT* fmpt=&mount[0];
	  if(cdev!=fmpt->dev)
	  {
	    printf("inonum = %d, dev=%d, true dev=%d\n",inonum,cdev,fmpt->dev);
	    fmpt=&mount[1];
	    MINODE* nmip=fmpt->mptr;
	    dev[0]=nmip->dev;
	    cdev=dev[0];
	    inonum=nmip->ino;
            int chino,pino,chdev,pdev;
	    if(findcpino(nmip,&chino,&pino,&pdev,&chdev)!=-1)
            {   
		dev[0]=pdev;
	        cdev=dev[0];
 	        inonum=pino;
	    }
	  }
	}
	iput(mip);
        mip=iget(cdev,inonum);
        if(mip->mounted&&strcmp(name[0],".."))             //traverse downward
        {
	   mountp=mip->mptr;
	   mip=iget(mountp->dev,2);  //get root of the mounted disk
	   printf("1_have mounted disk: %s\n",mountp->name);
	   dev[0]=mountp->dev;
           cdev=dev[0];
	   inonum=2;
        }
        for(i=1;i<pathcount;i++)
        {
	   mip=iget(cdev,inonum);
	   if(mip->mounted)             //traverse downward
	   {
	      mountp=mip->mptr;
	      mip=iget(mountp->dev,2);  //get root of the mounted disk
	      printf("2_have mounted disk: %s\n",mountp->name);
	      dev[0]=mountp->dev;
	      inonum=2;
	      break;
	   }
	   inonum=search(mip,name[i]);  //search has problem
           if(inonum==-1)
            {
               printf("%s  is not exist\n",name[i]);
               getchar();
               iput(mip);
	       return -1;
            }
	   iput(mip);
	 }
    }
        printf("%s [%d %d]\n",pathname,dev[0],inonum);

    return inonum;
}

MINODE* iget(int dev, int ino)  //return MINODE*
{
    u32 blk, offset;
    MINODE *mip;
    MOUNT *mountp;
    int i;
    for(i=0;i<NMOUNT;i++)
    {
       mountp=&mount[i];
      if(mountp->dev==dev)
      {
    	//printf("mountp->dev=%d\n",mountp->dev);
	break;
      }
    }
    for(i=0;i<NMINODE;i++)
    {
	mip=&minode[i];
	//if(mip->refCount>0 && mip->dev==dev && mip->ino==ino)
       if(mip->dev==dev && mip->ino==ino)
        {
	    if(mip->mounted)
	    {
		printf("minode %d is mounted.\n",i);
	        mountp=mip->mptr;
                return mountp->mptr;	        
	    }
            mip->refCount++;
            //return &minode[i];
	    return mip;
        }
    }
    

    blk=(ino-1)/8+mountp->iblk;//blk=(ino-1)/8+inode_start;//be_inode_table;
    offset=(ino-1)%8;
    get_block(dev, blk, buf);
    ip=(INODE* )buf+offset;

    for(i=0;i<NMINODE;i++)  //look for empty minode[i]
    {
	mip=&minode[i];
        if(mip->refCount==0)
        {
	 //   printf("add new minode to %d\n",i);
            mip->INODE=*ip;
            mip->dev=dev;
            mip->dirty=0;
            mip->ino=ino;
            mip->mounted=0;
            mip->mptr=0;
            mip->refCount=1;
            return mip;
        }
    }

    return NULL; }

void iput(MINODE* mip)  //release a minode[] pointed by mip
{
    if(mip->mounted)
	return;
    mip->refCount--;
    //printf("iput(),refcount %d,dirty %d",mip->refCount,mip->dirty);
    if(mip->dirty==1)
    {
        //must write the inode back to disk
        printf("iput mip\n");
        int ldev=mip->dev;
        int lino=mip->ino;
        int lblk=(lino-1)/8+inode_start;
        mip->dirty=0;
        //printf("lblk is %d",lblk);
        int loffset=(lino-1)%8;
        get_block(ldev,lblk,buf);
        INODE *nip=(INODE *)buf+loffset;
        //copy mip->INODE to nip
        memcpy(nip,&mip->INODE, sizeof(INODE));
        printf("nip link count = %d\n",nip->i_links_count); 
        put_block(ldev,lblk,buf);
        //int seek=lblk*1024+loffset*128;  //find the position of inode,
        //each block has 8 set
        //lseek(dev,seek,SEEK_SET);
        //write(mip->dev,&mip->INODE, sizeof(INODE));

    }
    else if(mip->refCount>0)
    {
        return;
    }
    else{
       /* int i;
        for(i=0;i<NMINODE;i++)
        {
           if(minode[i].dirty==1)
            {
                iput(&minode[i]);
            }
        }*/
        return;
    }


}

int quit()
{
    //iput all DIRTY minodes before shutdown
    int i;
    MINODE *mip;
    for(i=0;i<NMINODE;i++) {
        mip=&minode[i];
        if(mip->mounted)
	   mip->mounted=0;
        if(mip->refCount>0)
            iput(mip);
    }
    exit(0);
}

void get_stat(struct stat *mystat,char* pathname)
{
    MINODE* myino;
    int inonum;
    inonum=getino(&dev,pathname);
    if(inonum)
    {
        myino=iget(dev,inonum);
        mystat->st_dev=dev;
        mystat->st_ino=inonum;
        mystat->st_mode=myino->INODE.i_mode;
        mystat->st_nlink=myino->INODE.i_links_count;
        mystat->st_uid=myino->INODE.i_uid;
        mystat->st_gid=myino->INODE.i_gid;
        mystat->st_size=myino->INODE.i_size;
        mystat->st_blksize=BLKSIZE;
        mystat->st_blocks=myino->INODE.i_blocks;
        //mystat->st_mtim=ctime(&myino->INODE.i_mtime);
        time_t t=myino->INODE.i_mtime;
        char tt[64];
        strcpy(tt,ctime(&t));
        tt[strlen(tt)-1]=0;
        printf("----file stat----\n");
        printf("dev=%d, ino=%d, i_mode=%x\n",dev,inonum,myino->INODE.i_mode);
        printf("links=%d, uid=%d, gid=%d\n",myino->INODE.i_links_count,myino->INODE.i_uid,myino->INODE.i_gid);
        printf("size=%d, blocknum=%d, time: %s\n",myino->INODE.i_size,myino->INODE.i_blocks,tt);
        printf("-----------------\n");
        iput(myino);
    }

    printf("----stat of %s----\n", pathname);
    printf("Device: %d\n", (int) mystat->st_dev);
    printf("INODE: %d\n", (int) mystat->st_ino);
    printf("Permissions:");
    char file_type[11] = {0};
    strcpy(file_type, "--------------");
    switch (mystat->st_mode & S_IFMT) {
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
    if (mystat->st_mode & S_IRUSR) {
        file_type[1] = 'r';
    }
    if (mystat->st_mode & S_IWUSR) {
        file_type[2] = 'w';
    }
    if (mystat->st_mode & S_IXUSR) {
        file_type[3] = 'x';
    }
    if (mystat->st_mode & S_IRGRP) {
        file_type[4] = 'r';
    }
    if (mystat->st_mode & S_IWGRP) {
        file_type[5] = 'w';
    }
    if (mystat->st_mode & S_IXGRP) {
        file_type[6] = 'x';
    }
    if (mystat->st_mode & S_IROTH) {
        file_type[7] = 'r';
    }
    if (mystat->st_mode & S_IWOTH) {
        file_type[8] = 'w';
    }
    if (mystat->st_mode & S_IXOTH) {
        file_type[9] = 'x';
    }
    printf("%s \n", file_type);
    printf("Links: %d\n", mystat->st_nlink);
    printf("Size: %d\n", mystat->st_size);
    printf("Blocksize: %d\n", mystat->st_blksize);
    printf("Num of blocks: %d\n", mystat->st_blocks);
    printf("--------stat end------\n");

}


void mystat(char* pathname) {
    struct stat lmystat;
    if(!*pathname)
    {
      printf("Invalid pathname.\n");
      return;
    }
    get_stat(&lmystat, pathname);
    return;
}
