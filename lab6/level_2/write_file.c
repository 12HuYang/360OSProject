/******************write_file.c***************************/
int write_file(int fdnum,char* writestr);
int mywrite(int fdnum,char* writestr,int nbytes);

int write_file(int fdnum,char* writestr)
{
  //1.preperation: ask for a fd and a text string to write;
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
  //2. verify is indeed opened for WR or RW or APPEND mode.
  if(oftpr->mode!=1&&oftpr->mode!=2&&oftpr->mode!=3)
  {
    printf("file is not opened in WR or RW or APPEND mode.\n");
    return -1;
  }
  //3. copy the text string into a buf[] and get its length as nbytes.
  char buf[BLKSIZE];
  strcpy(buf,writestr);
  int nbytes;
  nbytes=strlen(*buf);
  printf("String length is %d\n",nbytes);

  return(mywrite(fdnum,buf,nbytes));
}

int mywrite(int fdnum,char* writestr,int nbytes)
{
  int lbk,startByte,blk,remain;
  char buf[BLKSIZE],wbuf[BLKSIZE];
  char *cq=writestr;
  OFT* oftpr=running->fd[fdnum];
  MINODE* mip=oftpr->mptr;
  INODE* ip=&mip->INODE;
  while(nbytes>0)
  {
    //compute LOGICAL BLOCK (lbk) and the startByte in theat lbk:
   lbk=oftpr->offset/BLKSIZE;
   startByte=oftpr->offset%BLKSIZE;
   //write data to DIRECT blocks, INDIRECT blocks and D_INDIRECT blocks
   if(lbk<12) //direct block
   {
     if(ip->i_block[lbk]==0) //no data block yet
     {
       ip->i_block[lbk]=balloc(mip->dev); //MUST ALLOCATE a block
       //write a block of 0's to blk on disk: OPTIONAL for data block but MUST for I or D blocks      
       blk=ip->i_block[lbk];
       memset(buf,0,sizeof(buf));
       put_block(mip->dev,blk,buf);
     }
     blk=ip->i_block[lbk];
   }
   else if(lbk>12 && lbk<256+12)
   {
     u32 ibuf[256];
     get_block(mip->dev,ip->i_block[12],ibuf);
     if(ibuf[lbk-12]==0) //no data block yet
     {
       ibuf[lbk-12]=balloc(mip->dev);
       blk=ibuf[lbk-12];
       memset(buf,0,sizeof(buf));
       put_block(mip->dev,blk,buf);
     }
     blk=ibuf[lbk-12];
   }else{
     u32 dbuf[256];
     get_block(mip->dev,ip->i_block[13],dbuf);
     lbk-=(12+256);
     int dblk=dbuf[lbk/256];
     get_block(mip->dev,dblk,dbuf);
     if(dbuf[lbk%256]==0) //no data block yet
     {
       dbuf[lbk%256]=balloc(mip->dev);
       blk=dbuf[lbk%256];
       memset(buf,0,sizeof(buf));
       put_block(mip->dev,blk,buf);
     }
     blk=dbuf[lbk%256];
   }
   //write to the data block
   get_block(mip->dev,blk,wbuf);  //read disk block into wblk[]
   char *cp=wbuf+startByte;
   remain = BLKSIZE - startByte;

   while(remain>0)
   {
     *cp++=*cq++;
     nbytes--;
     remain--;
     oftpr->offset++;
     if(oftpr->offset>ip->i_size)  
     {
       ip->i_size++;
     }
     if(nbytes<=0)
       break;
   }
   /*if(remain>0)
   {
     memcpy(cq,cp,remain);
     *cp+=remain;
     *cq+=remain;
     nbytes-=remain;
     oftpr->offset+=remain;
     if(oftpr->offset>ip->i_size)
        ip->i_size=oftpr->offset;
   }*/
   put_block(mip->dev,blk,wbuf);
   //loop back to while to write more ... until nbytes are written
  }
  mip->dirty=1;
  printf("wrote %d char into file descriptor fd=%d\n",remain,fd);
  getchar();
  return nbytes;
}
