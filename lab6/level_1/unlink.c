
/*1. get filenmae's minode:
ino = getino(&dev, filename);
mip = iget(dev, ino);
check it's a REG or SLINK file
2. // remove basename from parent DIR
rm_child(pmip, mip->ino, basename);
pmip->dirty = 1;
iput(pmip);
3. // decrement INODE's link_count
mip->INODE.i_links_count--;
if (mip->INODE.i_links_count > 0){
mip->dirty = 1; iput(mip);
}
4. if (!SLINK file) // assume:SLINK file has no data block
truncate(mip); // deallocate all data blocks
deallocate INODE;
iput(mip);*/

void _unlink(char *filename)
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


    /*ino = getino(&dev, filename);
    mip = iget(dev, ino);
    rm_child(pmip, mip->ino, basename);
    pmip->dirty = 1;
    iput(pmip);
    mip->INODE.i_links_count--;
    if (mip->INODE.i_links_count > 0){
      mip->dirty = 1; iput(mip);
      }*/
    ino = getino(&dev, filename);
    mip = iget(dev,ino);
    printf("mip dev= %d ino=%d \n",mip->dev,mip->ino);
   // printf("link count = %d",mip->INODE.i_links_count);
    ip=&mip->INODE;
    if(ip->i_mode & 004000 == 004000)
      {
        printf("file %s is a dir.\n",filename);
	return;
      }
   // printf("ip->i_size= %d\n",ip->i_size);
    printf("INODE i_links_count = %d",ip->i_links_count);

    strcpy(&temp,filename);
    printf("file name 1 = %s\n",filename);
    strcpy(&my_dirname, dirname(temp));

    strcpy(&temp, filename);
    strcpy(&my_basename, basename(temp));
    printf("dirname: %s, basename: %s\n",my_dirname,my_basename);
    pino = getino(&dev, my_dirname);
    pmip = iget(dev, pino);

    //Removes the child from the parent
    rm_child(pmip, my_basename);

    //Update the time, set dirty, and iput
    (pmip->INODE).i_atime = time(NULL);
    (pmip->INODE).i_mtime = time(NULL);
    pmip->dirty = 1;
    iput(pmip);

    mip->INODE.i_links_count--;
    if(mip->INODE.i_links_count>0)
    {
      mip->dirty=1;
      iput(mip);
    }
    if(!S_ISLNK(ip->i_mode)){
    for(i = 0; i < 12; i++)
      {
    bdealloc(dev, ip->i_block[i]);
      }
    idealloc(dev, ino);
    iput(mip);}
    else{
        ip->i_block[0]=0;
        return;
    }

    return;
}
