/********************read_file.c********************************/
int myread(int fdnum,char* buf,int nbytes);
int read_file(int fdnum,char *buf,int nbytes);
int _cat(char* filename);

int _cat(char* filename)
{
  char mybuf[BLKSIZE],dummy=0; // a null char at end of mybuf[]
  int n;

  //1. int fd =  open file name for READ, flag = 0;
  int fdnum = _open(filename,0);
  //printf("fdnum is %d",fdnum);
  n=read_file(fdnum,mybuf,BLKSIZE);
 // printf("_cat: n is %d\n",n);
  while(n>0)
  {
    //printf("_cat n is %d\n",n);
    //getchar();
    mybuf[n]=0;  //as a null terminated string
    //printf("%s\n",mybuf);
    int i=0;
    while(mybuf[i])
	{
	  putchar(mybuf[i]);
	  if(mybuf[i] == '\n')
	    putchar('\r');
	  i++;
	}
    n=read_file(fdnum,mybuf,BLKSIZE);
  }
  _close(fdnum);

}

int read_file(int fdnum,char* buf,int nbytes)
{
  //varify that fd is indeed opened for RD or RW;
  OFT* oftpr=running->fd[fdnum];
  if(!oftpr)
  {
    printf("fd is invalid.\n");
    return -1;
  }
  if(!oftpr->refCount)
  {
    printf("file is not opened.\n");
    return -1;
  }
  if(oftpr->mode!=0&&oftpr->mode!=2)
  {
    printf("file is not opened in RD or RW mode.\n");
    return -1;
  }
  int n=myread(fdnum,buf,nbytes);
 // printf("read_file: n is %d\n",n); 
  return n;  
  
}

int myread(int fdnum,char* buf,int nbytes)
{
  //1.compute # of bytes still available in file. assign a pointer to buf[]
  int count=0,remain;
  //avail=filesize - OFT's offset
  OFT* oftpr=running->fd[fdnum];
  MINODE* mip=oftpr->mptr;
  INODE* ip=&mip->INODE;
  int avail=ip->i_size-oftpr->offset;
  char *cq=buf;
  //2. compute LOGICAL BLOCK number lbk and startByte in the block from offset;
  //printf("myread() fd = %d, nbytes = %d\n",fdnum,nbytes);
  //getchar();
  while(nbytes>0&&avail>0)
  {
    int lbk=oftpr->offset/BLKSIZE;
    int startByte=oftpr->offset%BLKSIZE;
    //Read DIRECT BLOCK, INDIRECT BLOCK and D_INDIRECT BLOCK
    int blk;
    if(lbk<12) //lbk is a direct block
    {
      blk=ip->i_block[lbk];
    }
    else if(lbk>12 && lbk<256+12) //indirect blocks
    {
      u32 ibuf[256];
      get_block(mip->dev,ip->i_block[12],ibuf);
      blk=ibuf[lbk-12];
    }else
    {    //double indirect blocks
      u32 dbuf[256];
      get_block(mip->dev,ip->i_block[13],dbuf);
      lbk-=(12+256);
      int dblk=dbuf[lbk/256];
      get_block(mip->dev,dblk,dbuf);
      blk=dbuf[lbk%256];
    }

    //get the data block into readbuf[BLKSIZE]
    char readbuf[BLKSIZE];
    get_block(mip->dev,blk,readbuf);

    //copy from startByte to buf[], at most remain bytes in this block
    char *cp=readbuf+startByte;
    remain = BLKSIZE - startByte;  //number of bytes remain in readbuf[]
     while(remain>0)  //KC's code need to improve
    {
      *cq++=*cp++;
      oftpr->offset++;
      count++;
      avail--;
      nbytes--;
      remain--;
      if(nbytes<=0 || avail<=0)
	break;
    }
    /*if(remain>0)
    {
      memcpy(cq,cp,remain);
      *cp +=remain;
      *cq +=remain;
      oftpr->offset = oftpr->offset + remain;
      count = count + remain;
      avail = avail-remain;
      nbytes = nbytes - remain;
     // printf("\nremain = %d, nbytes = %d, count = %d, avail = %d\n",remain,nbytes,count,avail);
     // getchar();
    }*/
   
    // if one data block is not enough, loop back to OUTER while for more...

  }
  printf("myread: read %d char from file descriptor %d\n",count,fdnum);
  getchar();
  return count;
}
