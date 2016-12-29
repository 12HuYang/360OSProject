/********************read_file.c********************************/
int myread(int fdnum,char* buf,int nbytes);
int read_file(int fdnum,char *buf,int nbytes);


int read_file(int fdnum,char* buf,int nbytes)
{
  //varify that fd is indeed opened for RD or RW;
  OFT* oftpr=running->fd[fdnum];
  if(!*oftpr)
  {
    printf("fd is invalid.\n");
    return -1;
  }
  if(!oftpr->refCount)
  {
    printf("file is not opened.\n");
    return -1;
  }
  if(oftpr->mode!=0||oftpr->mode!=2)
  {
    printf("file is not opened in RD or RW mode.\n")
    return -1;
  }
  
  return(myread(fdnum,buf,nbytes));  
  
}

int myread(int fdnum,char* buf,int nbytes)
{
  //1.compute # of bytes still available in file. assign a pointer to buf[]
  int count=0;
  //avail=filesize - OFT's offset
  OFT* oftpr=running->fd[fdnum];
  MINODE* mip=oftpr->mptr;
  INODE* ip=&mip->INODE;
  int avail=ip->i_size-oftpr->offset;
  char *cq=buf;
  //2. compute LOGICAL BLOCK number lbk and startByte in the block from offset;
  int lbk=oftpr->offset/BLKSIZE;
  int startByte=oftpr->offset%BLKSIZE;
  //Read DIRECT BLOCK, INDIRECT BLOCK and D_INDIRECT BLOCK
  if(lbk<12) //lbk is a direct block
  {
    blk=ip->i_block[lbk];
  }
  else if(lbk>12 && lbk<256+12) //indirect blocks
  {

  }else{    //double indirect blocks
  
  }

  //get the data block into readbuf[BLKSIZE]
  char readbuf[BLKSIZE];
  get_block(mip->dev,blk,readbuf);

  //copy from startByte to buf[], at most remain bytes in this block
  char *cp=readbuf+startByte;
  remain = BLKSIZE - startByte;  //number of bytes remain in readbuf[]







}
