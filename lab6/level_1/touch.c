/*********************touch.c***************************/

int _touch(char* filename);

int _touch(char* filename)
{
  int dev[2],fino;
  MINODE* mip;
  INODE* ip;
  dev[0]=running->cwd->dev;
  dev[1]=0;
  if(!*filename)
  {
    printf("Invalid command.\n");
    return -1;
  }
  if(*filename)
  {
    fino=getino(&dev,filename);
    if(fino==-1)
    {
      printf("No file %s",filename);
      return -1;
    }
    printf("_touch()\n");
    mip=iget(dev[0],fino);
    ip=&mip->INODE;
    ip->i_atime=ip->i_mtime=time(0L);
    mip->refCount++;
    mip->dirty=1;
    iput(mip);
    return 0;
  }

}
