void _mv(char *_src, char *_dest)
{
  int ino = 0, ino2 = 0;
  char src[1024];
  char dest[1024];
  char* parent;
  int dev[2],dev2[2];
  MINODE *mip,*dip;
  INODE* ip; 
  //Check to make sure there is a sorce and dest defined
  //Strcpy to src and dest so we dont lose them
  PROC* p;
  int j;

   for(j=0; j<NPROC; j++)
   {
     p=&proc[j];
     if(p->uid != 0)
     {
	printf("Incorrect UID.\n");
        return;
     }
   }
  if (!strcmp(_src, ""))
    {
      printf("Invalid source\n");
      return;
    }
  strcpy(src, _src);
  if (!strcmp(_dest, ""))
    {
      printf("Invalid dest\n");
      return;
    }
  strcpy(dest, _dest);

  //Verify that src exists and get its ino
  //Returns out if the src does not exist
  dev2[0]=running->cwd->dev;
  dev2[1]=0;
  parent=dirname(strdup(dest));
  ino2 = getino(&dev2,parent);
  dip=iget(dev2[0],ino2);
  dev[0]=running->cwd->dev;
  dev[1]=0;
  ino=getino(&dev,_src);
  mip=iget(dev[0],ino);
  ip=&mip->INODE;
  if(ino == 0 || ino2==0)
    {
      return;
    }
  
  //strcpy(_dest, dest);
  if(dip->dev==mip->dev)
  {
    printf("dip->dev=%d,mip->dev=%d\n",dip->dev,mip->dev);
    getchar();
    link(src,dest);
    _unlink(src);
    //_rm(src);
    //ip->i_links_count--;
    return;
  }
  printf("dip->dev=%d,mip->dev=%d\n",dip->dev,mip->dev);
  getchar();
  _cp(src, dest);
  _unlink(src);
  return;
}
