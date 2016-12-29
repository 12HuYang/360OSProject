/*************************mount_umount.c*****************************/
int _mount(char* filesys,char*parameter);
int _umount(char *filesys);

int _mount(char* filesys,char* parameter)
{
  int i,mdev,dev,ino;
  MINODE* mip;
  MOUNT* mountp;
  INODE* ip;
  //1. Ask for filesys (a pathname) and mount_point (a pathname also).
  //If mount with no parameters: display current mounted filesystems.
  //printf("Input a file system: "); //
  //bzero(line,256);    //zero out line[]
  //fgets(line,128,stdin);  
  //line[strlen(line)-1]=0;
  //if(!strcmp(filesys,"")) //no input
  //{
  //  printf("Empty input.\n");
  //  return -1;
  //}
  //pathname[0]=0; 
  //sscanf(line, "%s %s",pathname,new);  //new is parameter
  if(!strcmp(filesys,"")||!strcmp(parameter,"")||!strcmp(parameter,".")||!strcmp(parameter,".."))
  {
    printf("No mount_point, please input a pathname to mount the file system.\n");
    if(mount[0].dev!=0)
    {
	printf("current mounted filesystems is:\n"); //display current mounted filesystems.
        for(i=0;i<NMOUNT;i++)
	{
	    if(mount[i].dev!=0)
	    {
		printf("--------------------------\n");
		printf("id=%d    dev=%d    name=%s    mountpoint=%s\n",i,mount[i].dev,mount[i].name,mount[i].mount_name);
		printf("--------------------------\n");
	    }
	}

    }
    return -1;
  }
  //2. Check whether filesys is already mounted:
  //store the name of mounted filesys in the MOUNT table entry
  //if already mounted, reject;
  //else allocate a free MOUNT table entry (whose dev = 0 means FREE)
  for(i=0;i<NMOUNT;i++)
  {
    mountp=&mount[i];
    if(mountp->dev!=0){
      if(!strcmp(mountp->name,pathname))  // already exist
      {
	  printf("%s is already mounted\n");
	  return -1;
      }
    }
      if(mountp->dev==0)
      {
	printf("mountp->dev = %d\n",mountp->dev);
	break;  //mountp point to a FREE MOUNT table entry
      }
  }
  //3. open filesys for RW; use its fd number as the new DEV;
  //Check whether it's an EXT2 file system, if not, reject.
  if((fd=open(pathname,O_RDWR))<0)
  {
    printf("open %s failed\n",pathname);
    return -1;
  }
  mdev=fd;
  get_block(mdev,1,buf); //read super block
  sp=(SUPER*)buf;
  //verify it's an ext2 file system
  if(sp->s_magic!=0xEF53){
      printf("magic = %x is not an ext2 file system.\n",sp->s_magic);
      return -1;
  }
  printf("OK\n");
  
  //4. For mount_point: find its ino, then get its minode:
  dev=running->cwd->dev;
  ino=getino(&dev,parameter);
  mip=iget(dev,ino);
  ip=&mip->INODE;
  //5. Check mount_point is a DIR
  if((ip->i_mode & 0040000) != 0040000)
  {
    printf("mount_point %s is not a DIR\n",parameter);
    return -1;
  }
  //Check mount_point is NOT busy (e.g. can't be someone's CWD)
  for(i=0;i<NPROC;i++)
  {
    PROC* cproc=&proc[i];
    if(mip==cproc->cwd)
    {
      printf("%s is busy.\n",parameter);
      return -1;
    }
  }
  //6. Record new DEV in the MOUNT table entry;
  mountp->ninodes=sp->s_inodes_count;
  mountp->nblocks=sp->s_blocks_count;
  mountp->dev=mdev;

  get_block(mdev,2,buf); //get group block
  gp=(GD*)buf;

  mountp->bmap=gp->bg_block_bitmap;
  mountp->imap=gp->bg_inode_bitmap;
  mountp->iblk=gp->bg_inode_table;
  
  mountp->mptr=mip;
  strcpy(mountp->name,filesys);
  strcpy(mountp->mount_name,parameter);
  printf("bmp=%d imap=%d inode_start=%d \n",mountp->bmap,mountp->imap,mountp->iblk);

  printf("ninodes=%d nblocks=%d \n",mountp->ninodes,mountp->nblocks);
  //7. Mark mount_point's minode as being mounted on and let it point at the
  //MOUNT table entry, which points back to the mount_point minode.
  mip->mounted=1;
  mip->mptr=mountp;
  mip->refCount=99;
  iput(mip);
  
  return 0;
}

int _umount(char *filesys)
{
  int i;
  MINODE *mip;
  MOUNT* mpptr;
  //1. Search the MOUNT table to check filesys is indeed mounted.
  for(i=1; i<NMOUNT; i++)
  {
  /*if(mount[i].dev==0)
  {
  continue;
  }*/
    mpptr=&mount[i];
    if(strcmp(mpptr->name,filesys)==0)
    {
      mip=mpptr->mptr;
      if(mip&&mip->mounted==0)
      {
	printf("filesys is not mounted yet.\n");
	return -1;
      }
      //2. Check whether any file is still active in the mounted filesys;
      //      e.g. someone's CWD or opened files are still there,
      //   if so, the mounted filesys is BUSY ==> cannot be umounted yet.
      //   HOW to check?      ANS: by checking all minode[].dev
      if(mip->dev==mpptr->dev)
      {
	printf("busy, cannot be umounted yet.\n");
	return -1;
      }
      //3. Find the mount_point's inode (which should be in memory while it's mounted
      //   on).  Reset it to "not mounted"; then
      //         iput()   the minode.  (because it was iget()ed during mounting)
      //if(mip==mpptr)
      //{
	printf("umount %s\n",filesys);	
	mpptr->dev=0;
	mip->mounted=0;
	//iput(&mount[i]);
	iput(mip);
	break;
      //}
    }

  }

  //4. return 0 for SUCCESS;
  return 0;
}

/************************end of mount_umount.c**************************/
