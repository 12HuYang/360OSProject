int insertdir(char* dirname,int ino,int dev);
int deletedir();
int _mkdir();
int _creat();

int insertdir(char* dirname,int ino,int dev)
{
  char lbuf[BLKSIZE],*cp;
  int need_len=4*((8+strlen(dirname)+3)/4);
  MINODE* mip=iget(dev,ino);
  INODE* ip=&(mip->INODE);
  get_block(dev,ip->i_block[0],lbuf);
  dp=(DIR*)lbuf;
  cp=dp;
  int entrycount=0;
  while(*cp) //count entry number
  {
    entrycount++;
    cp+=dp->rec_len;  
    dp=(DIR*)cp;
  }
  if(entrycount==1)
  {
    //enter new entry as first entry in block;
  }else{
    cp-=dp->rec_len;//move to last entry in block;
    dp=(DIR*)cp;
    if(*cp)
    {
      int ideal_len=4*((8+dp->name_len+3)/4);
      printf("last entry ideal_len = %d\n",ideal_len);
      int remain=dp->rec_len-ideal_len;
      printf("remain = %d\n",remain);
      if(remain>need_len|remain==need_len){
	dp->rec_len=ideal_len;
	
      }

    }


  
  }

  for(i=0;i<12;i++)
  {
    get_block(dev,ip->i_block[i],lbuf);

  }
}
