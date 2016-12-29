/*****************mkdir_creat.c*****************/

int enter_child(MINODE* pmip, int ino, char* basename);
//int insertdir(char* dirname,int ino, MINODE* mip);
int deletedir();
int _mkdir(char* pathname);
int _creat(char* pathname);
int kmkdir(MINODE* pmip,char* basename);
int kcreat(MINODE* pmip,char* basename);


int kcreat(MINODE* pmip,char*basename)
{
  //1) allocate an INODE and a disk block
  u32 inumber,bnumber,ino;
  inumber=ialloc(pmip->dev);
  //bnumber=balloc(pmip->dev);
  ino=inumber;
  if(inumber!=0)
  {
    //2) initialize mip->INODE as a DIR INODE
    MINODE* mip=iget(pmip->dev,inumber);
    if(mip==0) printf("mip is null\n");
    INODE *ip=&mip->INODE;
    ip->i_mode=0x81A4;
    printf("i_mode set to %x\n",ip->i_mode);
    ip->i_uid=running->uid;
    ip->i_gid=running->gid;
    ip->i_size=0;
    ip->i_links_count=1;
    ip->i_atime=ip->i_ctime=ip->i_mtime=time(0L);
   // ip->i_blocks=2;
    mip->dirty=1;
    mip->refCount++;

    getchar();
    iput(mip);

    //3) make i_block[0] contain . and ..
    //4) enter child, which enters (inumber, basename) as a DIR entry to the parent INODE.
    enter_child(pmip, inumber, basename);
   
    }else
    {
      printf("cannot get inumber\n");
      return -1;  
    }

    return inumber;
}


int _creat(char* pathname)
{
  char cpyname[128],cpname[128],*pname,*cname;
  int ldev,pio,cio;
  
  strcpy(&cpyname,pathname);
  strcpy(&cpname,pathname);
  //cpyname[strlen(cpyname)-1]='\0';
  //cpname[strlen(cpname)-1]='\0';
  if(*cpyname=="/")
    ldev=root->dev;
  else
    ldev=running->cwd->dev;
  pname=dirname(&cpyname);
  cname=basename(&cpname);
  printf("pname is %s\n",pname);
  printf("cname is %s\n",cname);
  //check dirname exist
  pio=getino(&ldev,pname);
  if(pio!=-1)
  {
    MINODE* mpip=iget(ldev,pio);
    INODE* pip=&mpip->INODE;
    if(pip->i_mode & 004000!=004000) //varify if dirname is a dir
    {
      printf("%s is not a dir.\n",dirname);
      return -1;
    }
    //check basename is not exist
    if(search(mpip,cname)==-1)
    {
      int km=kcreat(mpip,cname); //call kmkdir to creat a dir
      if(km==-1)
      {
	printf("fail to create\n");
	return -1;     
      }
      //increment parent INODE link_count +1 and mark pmip dirty
      
      printf("creat success!\n");
      //pip->i_links_count++;
      mpip->refCount++;
      mpip->dirty=1;
      iput(mpip);

      return 0;
    }else
    {
      printf("basename %s already exist.\n",cname);
      return -1;
    }
    
  }
  else
  {
    printf("mkdir cannot make a dir, since %s not exist.\n",dirname);
    return -1;
  }
  
}

int kmkdir(MINODE* pmip,char* basename)
{
  //1) allocate an INODE and a disk block
  u32 inumber,bnumber,ino;
  inumber=ialloc(pmip->dev);
  bnumber=balloc(pmip->dev);
  ino=inumber;
  if(inumber!=0 && bnumber!=0)
  {
    //2) initialize mip->INODE as a DIR INODE
    MINODE* mip=iget(pmip->dev,inumber);
    if(mip==0) printf("mip is null\n");
    INODE *ip=&mip->INODE;
    ip->i_mode=0x41ED;
    ip->i_uid=running->uid;
    ip->i_gid=running->gid;
    ip->i_size=1024;
    ip->i_links_count=2;
    ip->i_atime=ip->i_ctime=ip->i_mtime=time(0L);
    ip->i_blocks=2;
    mip->dirty=1;
    mip->refCount++;

    ip->i_block[0]=bnumber;
    int i;
    for(i=1;i<15;i++)
    {
      ip->i_block[i]=0;
    }
    getchar();
    iput(mip);

   // mip=iget(pmip->dev,inumber);
   // INODE *tip=&mip->INODE;
   // printf("mip content: uid %d, gid %d, size %d, i_block[0] %d\n",tip->i_uid,tip->i_gid,tip->i_size,tip->i_block[0]);
    //3) make i_block[0] contain . and ..
    char lbuf[BLKSIZE]={ 0 }, *cp;
    dp=(DIR*)lbuf;
    cp=(char*)lbuf;
    dp->inode=inumber;
    strncpy(dp->name,".",1);
    dp->name_len=1;
    dp->rec_len=12;
    cp+=dp->rec_len;
    dp=(DIR*)cp;
    dp->inode=pmip->ino;
    strncpy(dp->name,"..",2);
    dp->name_len=2;
    dp->rec_len=BLKSIZE-12;
    put_block(pmip->dev,bnumber,lbuf);
    printf("....finish making block...\n dp->inode = %d\n",dp->inode);
    getchar();
    //4) enter child, which enters (inumber, basename) as a DIR entry to the parent INODE.
    enter_child(pmip, inumber, basename);
   
    //pmip->refCount++;
    //pmip->INODE.i_atime=time(0L);
    //pmip->dirty=1;
    //iput(pmip);
    }else
    {
      printf("cannot get inumber and bnumber\n");
      return -1;  
    }

    return 0;
}


int _mkdir(char* pathname)
{
  char cpyname[128],cpname[128],*pname,*cname;
  int ldev,pio,cio;
  
  strcpy(&cpyname,pathname);
  strcpy(&cpname,pathname);
  //cpyname[strlen(cpyname)-1]='\0';
  //cpname[strlen(cpname)-1]='\0';
  //if(!strcmp(cpyname,"/"))
  //  ldev=root->dev;
  //else
  //  ldev=running->cwd->dev;
  pname=dirname(&cpyname);
  cname=basename(&cpname);
  printf("pname is %s\n",pname);
  printf("cname is %s\n",cname);
  printf("pname[0] is %c\n",pname[0]);
  if(!strcmp(pname,"/")||pname[0]=="/")
    ldev=root->dev;
  else
    ldev=running->cwd->dev;
  //check dirname exist
  pio=getino(&ldev,pname);
  if(pio!=-1)
  {
    MINODE* mpip=iget(ldev,pio);
    printf("parent mip: dev= %d ino=%d\n",mpip->dev,mpip->ino);
    INODE* pip=&mpip->INODE;
    if(pip->i_mode & 004000!=004000) //varify if dirname is a dir
    {
      printf("%s is not a dir.\n",dirname);
      return -1;
    }
    //check basename is not exist
    if(search(mpip,cname)==-1)
    {
      int km=kmkdir(mpip,cname); //call kmkdir to creat a dir
      if(km==-1)
      {
	printf("fail to kmdir\n");
	return -1;     
      }
      //increment parent INODE link_count +1 and mark pmip dirty
      
      printf("mkdir success!\n");
      pip->i_links_count++;
      mpip->refCount++;
      mpip->dirty=1;
      iput(mpip);

      //update all mips
      int i;
      MINODE* mip;
      for(i=0;i<NMINODE;i++)
      {
	mip=&minode[i];
	if(mip->refCount>0)
	  iput(mip);
      }

      return 0;
    }else
    {
      printf("basename %s already exist.\n",cname);
      return -1;
    }
    
  }
  else
  {
    printf("mkdir cannot make a dir, since %s not exist.\n",pname);
    return -1;
  }

}

int deletedir(char* name)
{

}

int enter_child(MINODE* pmip, int ino, char* basename)
{
  printf("making dir...\n");
  printf("pmip ino = %d dev = %d\n",pmip->ino,pmip->dev);
  getchar();
  char lbuf[BLKSIZE],*cp;
  int need_len=4*((8+strlen(basename)+3)/4);//new entry need length
  int i,rv=-1;
  printf("need_len is %d\n",need_len);
  ip=&(pmip->INODE);
    for(i=0;i<12&&ip->i_block[i];i++)
    {
      printf("pmip->dev = %d,i= %d,i_block=%d\n",pmip->dev,i,ip->i_block[i]);
      get_block(pmip->dev,ip->i_block[i],lbuf);
      dp=(DIR*)lbuf;
      cp=(char*)lbuf;
      //int blk=pmip->INODE.i_block[i];
      //printf("kmkdir...of data block %d \n",blk);
      u16 lastentry=0;
      int entries=0;
      while(cp<lbuf+BLKSIZE && dp->rec_len) //count entry number, check shift problem.
      {
	int current_len=4*((8+dp->name_len+3)/4);
        char temp[256]={ 0 };
        strncpy(temp,dp->name,dp->name_len);
        printf("shifting %s\n",temp);
        entries++;
        lastentry=dp->rec_len;
	//getchar(); 
	cp+=dp->rec_len;
	dp=(DIR*)cp;
      }
      if(entries==0 && dp->inode==0)
      {
        //ip->i_links_count++;
 	dp->inode=ino;
	dp->name_len=strlen(basename);
	strncpy(dp->name,basename,dp->name_len);
	dp->rec_len=BLKSIZE;
	put_block(pmip->dev,ip->i_block[i],lbuf);
        rv=0;
	break;
      }
      //cp+=dp->rec_len;
      //lastentry=dp->rec_len;
      printf("cp jump back %d\n",lastentry);
      cp-=lastentry;//go to last entry
      dp=(DIR*)cp;
      printf("last entry is %s, %d\n",dp->name,dp->name_len);
      u16 ideal_len=4*((8+dp->name_len+3)/4);
      printf("last entry ideal_len = %d\n",ideal_len);
      u16 remain=dp->rec_len-ideal_len;
      printf("remain = %d\n",remain);
      if(remain>=need_len)
      {
       // ip->i_links_count++;
	dp->rec_len=ideal_len;//trim last entry's rec_len to ideal_len
	cp+=ideal_len; //move to next entry, which is last one
	dp=(DIR*)cp;
	dp->inode=ino;//enter new entry as last entry with rec_len = remian
	dp->name_len=strlen(basename);
	strncpy(dp->name,basename,dp->name_len);
	dp->rec_len=remain;
	rv=0;
        put_block(pmip->dev,ip->i_block[i],lbuf);
        break;
      }
      else //allocate a new block
      {
       // ip->i_links_count++;
	u32 bnumber=balloc(pmip->dev);
	if(bnumber==0) break;
	ip->i_block[i+1]=bnumber;
	char llbuf[BLKSIZE];
	get_block(pmip->dev,bnumber,llbuf);
	dp=(DIR*)llbuf;
	dp->inode=ino;
	//dp->name=dirname;
	dp->rec_len=1024;
	dp->name_len=strlen(basename);
	strncpy(dp->name,basename,dp->name_len);
	ip->i_size+=BLKSIZE;
	put_block(pmip->dev,bnumber,llbuf);
	put_block(dev,ip->i_block[i+1],lbuf);
	rv=0;
	break;
      }
    }
    pmip->refCount++;
    pmip->dirty=1;
    //iput(pmip);
    return rv;
}
