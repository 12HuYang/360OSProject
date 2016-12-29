/**********************lini_unlink.c***************************/
int link(char* old_file,char* new_file);
int unlink(char* filename);
int enter_link(MINODE* pmip,int ino, char* basename);

int enter_link(MINODE* pmip, int ino, char* basename)
{
  printf("making link..\n");
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
	getchar(); 
	cp+=dp->rec_len;
	dp=(DIR*)cp;
      }
      if(entries==0 && dp->inode==0)
      {
    //    ip->i_links_count++;
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
      //  ip->i_links_count++;
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

 
int link(char* old_file,char* new_file)
{
  //1. verify old_file exist and is not DIR;
  printf("link()\n");
  int oino,dev,nino;
  MINODE* omip,nmip;
  INODE *ip; 
  
  dev=running->cwd->dev;
  oino=getino(&dev,old_file);
  omip=iget(dev,oino);
  
  ip=&(omip->INODE);
  if(ip->i_mode == 0x41ED) //old file is a dir
  {
    printf("%s is a dir\n",old_file);
    return -1;
  }

  //2. new file must not exist
  nino=getino(&dev,new_file);

  if(nino==-1) //not exist, 3. create entry in new_parent DIR with same inode
  {
    char strcp[32],strcpp[32],*pname,*cname;
    strcpy(&strcp,new_file);
    strcpy(&strcpp,new_file);
    pname=dirname(&strcp);
    //printf("pname is %s\n",pname);
    cname=basename(&strcpp);
    int pino;
    MINODE* pmip;
    pino=getino(&dev,pname);
    pmip=iget(dev,pino);
    ip->i_links_count+=1;
    enter_link(pmip,oino,cname);
    printf("link count = %d",omip->INODE.i_links_count);
    omip->dirty=1;
    //omip->refCount++;
    iput(omip);
    iput(pmip);
    //omip->INODE.i_links_count=33;
   // printf("link count = %d",omip->INODE.i_links_count);
   // omip->dirty=1;
    //omip->refCount++;
   // iput(omip);
    return 0; 
  }
  return -1;
}

