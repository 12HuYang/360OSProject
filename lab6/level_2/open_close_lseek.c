/***********************open_close_lseek.c********************************/
int _open(char* filename, int flags);
int _lseek(int fdnum,int position);
int _close(int fdnum);

int _close(int fdnum)
{
 // if(!fdnum)
 // {
 //   printf("invalid fdnumber\n");
 //   return -1;
 // }
  if(!running->fd[fdnum]||!running->fd[fdnum]->refCount)
  {
    printf("Invalid, file is not opened.\n");
    return -1;
  }
  if(running->fd[fdnum]!=0)
  {
    OFT* oftp=running->fd[fdnum];
    if(--oftp->refCount==0) //if the last PROC using this OFT
    {
      iput(oftp->mptr);
      printf("Close file with fdnumber %d\n",fdnum);
    }
    running->fd[fdnum]=0;  //clear fd[fd]
  }
  return 0; //return SUCCESS;

}


int _lseek(int fdnum,int position)
{
  if(!fdnum)
  {
    printf("invalid fdnumber\n");
    return -1;
  }
  if(!position)
  {
    printf("invalid position\n");
    return -1;
  }
  if(!running->fd[fdnum]||!running->fd[fdnum]->refCount)
  {
    printf("Invalid, file is not opened.\n");
    return -1;
  }
  OFT* oftp;
  oftp=running->fd[fdnum];
  if(oftp->mode==0)
  {
    if(position>=0||position>oftp->mptr->INODE.i_size)
    {
      printf("Position exceed file size boundary.\n");
      return -1;
    }
  }
  if(oftp->mode==1)
  {
    printf("Move to assigned position.\n");
    oftp->offset=position;
  }
  return 0;
}


int _open(char* filename, int flags)  //flags : 0 for RD, 1 for WR, 2 for RW, 3 for APPEND
{
  int ino,dev[2];
  INODE *ip;
  MINODE* mip;
  u16 mode1,mode2;
  OFT *oftp;
  printf("_open()\n");
  if(!strcmp(filename,""))
  {
    printf("Invalid filename.\n");
    return -1;
  }
  if(flags<0 || flags> 4)
  {
    printf("Invalid flags\n");
    return -1;
  }
  //1. get file's minode
  dev[0]=running->cwd->dev;
  dev[1]=0;
  ino=getino(&dev,filename);
  if(ino==-1 && O_CREAT) //file not exist, and have creat commend
  {
    printf("file %s not exist, going to creat one.\n");
    _creat(filename);
    ino=getino(&dev,filename);
  }
  mip=iget(dev[0],ino); 
  //2. check file's INODE's assess permission
  ip=&(mip->INODE);
  if(ip->i_mode & 0100000 != 0100000)
  {
    iput(mip);
    printf("%s is not a file\n");
    return -1;
  }

  if(flags==0 && ip->i_mode & 0x0400 != 0x0400)
  {
    printf("Do not have user read permission.\n");
    return -1;
  }

  if(flags==1 && ip->i_mode & 0x0200 != 0x0200)
  {
    printf("Do not have user write permission.\n");
    return -1;
  }
  mode1=0x0400;
  mode2=0x0200;
  mode1|=mode1|mode2;

  if(flags == 2 && ip->i_mode & mode1 !=mode1)
  {
    printf("Do not have user read and write permission.\n");
    return -1;
  }

  if(flags == 3 && ip->i_mode & 0x0400 != 0x0400)
  {
    printf("Do not have user read permission.\n");
    return -1;
  }
  //check for incompatible open modes;
  int j;
  for(j=0;j<NFD;j++)
  {
    if(running->fd[j])
    {
      oftp=running->fd[j];
      if(oftp->refCount && oftp->mptr == mip)
      {
	if(oftp->mode){
	  printf("%s was opened as incomptatible mode\n",filename);
	  iput(mip);
	  return -1;
	}
	break;
      }

    }
    oftp=0;
  }
  //3. allocate an openTable entry
  if(oftp)
  {
    oftp->refCount++; //already have available oftp;
    printf("open file ref count is %d",oftp->refCount);
  }else{    //need to allocate one
    oftp=oftalloc();
    if(oftp==0)
    {
      printf("cannot get oftp\n");
      iput(mip);
      return -1;
    }
    printf("oftp refcount is %d\n",oftp->refCount);
    //initilize oftp
    oftp->mode=flags;
    oftp->refCount=1;
    if(flags==3)
    {
      oftp->offset=ip->i_size;
    }else{
      oftp->offset=0;
    }
    oftp->mptr=mip;
    printf("oftp mode = %d, refcount= %d, offset = %d,\n",oftp->mode,oftp->refCount,oftp->offset);
  
  }

  //4. search for a FREE fd[] entry in PROC with lowest index
  for(j=0;j<NFD;j++)
  {
    if(!running->fd[j])
    {
      printf("running->fd[%d] is 0\n",j);
      running->fd[j]=oftp;  //assign initilized oftp to running process fd[];
      printf("running->fd[%d]-> mode = %d, refcount= %d, offset = %d,\n",j,running->fd[j]->mode,running->fd[j]->refCount,running->fd[j]->offset);
      break;
    }
  }

  //5.unlock? upload  minode? and return fd index j
  mip->refCount++;
  ip->i_atime=ip->i_mtime=time(0L);
  mip->dirty = 1;
//  printf("return j is %d",j);
  return j;


}
