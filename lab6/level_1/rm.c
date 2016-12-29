/*********************rm.c************************************/
int _rm(char* filename);

int _rm(char* filename)
{
  int ino, i;
  int pino;
  MINODE *mip;
  MINODE *pmip;
  INODE *ip;
  INODE *pip;
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
  char temp[64];
  char my_dirname[64];
  char my_basename[64];
  int dev = running->cwd->dev;

    ino = getino(&dev, filename);
    mip = iget(dev,ino);
    printf("mip dev= %d ino=%d \n",mip->dev,mip->ino);
   // printf("link count = %d",mip->INODE.i_links_count);
    ip=&mip->INODE;
    if(ip->i_mode & 004000 == 004000)
      {
        printf("file %s is a dir.Use rmdir instead\n",filename);
	return -1;
      }
    strcpy(&temp,filename);
    printf("file name 1 = %s\n",filename);
    strcpy(&my_dirname, dirname(temp));

    strcpy(&temp, filename);
    strcpy(&my_basename, basename(temp));
    printf("dirname: %s, basename: %s\n",my_dirname,my_basename);
    pino = getino(&dev, my_dirname);
    pmip = iget(dev, pino);
    pip=&pmip->INODE;
    //Removes the child from the parent
    rm_child(pmip, my_basename);

    //Update the time, set dirty, and iput
    pip->i_atime = time(NULL);
    pip->i_mtime = time(NULL);
    pmip->dirty = 1;
    iput(pmip);
    if(!S_ISLNK(ip->i_mode)){
    for(i = 0; i < 12; i++)
      {
    bdealloc(dev, ip->i_block[i]);
      }
    idealloc(dev, ino);
    iput(mip);}
    else{
        ip->i_block[0]=0;
        return 0;
    }

    return 0;

}
/*************************end of rm.c****************************/
