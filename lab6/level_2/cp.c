void _cp(char *_src, char *_dest)
{
  int i = 0;
  int fdDest = 0;
  int fdSrc = 0;
  char buf[1024];
  char src[1024];
  char dest[1024];
  MINODE *mip;
  INODE* ip;
  //Check to make sure there is a sorce and dest defined
  //Strcpy to src and dest so we dont lose them
  printf("cp()\n");
  printf("src is %s, dest is %s \n",_src,_dest);
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

  //We call our touch function to see if the destination exists already
  //If the dest doesnt exist, touch calls creat to make it for us
  if(_touch(dest)==0)
  {
    printf("Invalid, file %s is exist.\n",dest);
    return;
  }
  printf("after touch. src is %s, dest is %s\n",src,dest);
  //Now we open the src for read
  //make sure open mode is read
  //strcpy(_src, "0");

  //open with 0 for RD
  fdSrc = _open(src,0);

  //Now we open the dest for write
  //make sure open mode is write
  //strcpy(_dest, "1");

  //open with 1 for WR
  fdDest = _open(dest,1);

  //This loop reads from the src until there is nothing left to read
  //And writes it into the dest
  while (i = read_file(fdSrc, buf, BLKSIZE))
  {
    mywrite(fdDest, buf, i);
  }

  //Close our files
  _close(fdSrc);
  _close(fdDest);

  return;
}
