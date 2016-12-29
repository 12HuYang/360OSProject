/*********** alloc_dealloc.c file *************/

int tst_bit(char *buf, int bit)
{
  int i, j;

  i = bit / 8;
  j = bit % 8;
  if (buf[i] & (1 << j)){
    return 1;
  }
  return 0;
}

int clr_bit(char *buf, int bit)
{
  int i, j;
  i = bit / 8;
  j = bit % 8;
  buf[i] &= ~(1 << j);
  return 0;
}

int set_bit(char *buf, int bit)
{
  int i, j;
  i = bit / 8;
  j = bit % 8;
  buf[i] |= (1 << j);
  return 0;
}

int incFreeInodes(int dev)
{
  char buf[BLKSIZE];

  // inc free inodes count in SUPER and GD
  get_block(dev, 1, buf);
  sp = (SUPER *)buf;
  sp->s_free_inodes_count++;
  put_block(dev, 1, buf);

  get_block(dev, 2, buf);
  gp = (GD *)buf;
  gp->bg_free_inodes_count++;
  put_block(dev, 2, buf);
}

int decFreeInodes(int dev)
{
  // YOU DO IT
  char buf[BLKSIZE];

  get_block(dev, 1, buf);
  sp = (SUPER *)buf;
  sp->s_free_inodes_count--;
  put_block(dev, 1, buf);

  get_block(dev, 2, buf);
  gp = (GD *)buf;
  gp->bg_free_inodes_count--;
  put_block(dev, 2, buf);

}

int incFreeBlocks(int dev)
{
  char buf[BLKSIZE];

  // inc free block count in SUPER and GD
  get_block(dev, 1, buf);
  sp = (SUPER *)buf;
  sp->s_free_blocks_count++;
  put_block(dev, 1, buf);

  get_block(dev, 2, buf);
  gp = (GD *)buf;
  gp->bg_free_blocks_count++;
  put_block(dev, 2, buf);
}

int decFreeBlocks(int dev)
{
  // YOU DO IT
  char buf[BLKSIZE];

  // dec free block count in SUPER and GD
  get_block(dev, 1, buf);
  sp = (SUPER *)buf;
  sp->s_free_blocks_count--;
  put_block(dev, 1, buf);

  get_block(dev, 2, buf);
  gp = (GD *)buf;
  gp->bg_free_blocks_count--;
  put_block(dev, 2, buf);
}


u32 ialloc(int dev)
{
 int i;
 char buf[BLKSIZE];

 // get inode Bitmap into buf
 get_block(dev, imap, buf);
 
 for (i=0; i < ninodes; i++){
   if (tst_bit(buf, i)==0){
     set_bit(buf, i);
     put_block(dev, imap, buf);

     // update free inode count in SUPER and GD
     decFreeInodes(dev);
     
     printf("ialloc: ino=%d\n", i+1);
     return (i+1);
   }
 }
 return 0;
} 

idealloc(int dev, int ino)
{
  int i;  
  char buf[BLKSIZE];

  if (ino > ninodes){
    printf("inumber %d out of range\n", ino);
    return;
  }

  // get inode bitmap block
  get_block(dev, imap, buf);
  clr_bit(buf, ino-1);

  // write buf back
  put_block(dev, imap, buf);

  // update free inode count in SUPER and GD
  incFreeInodes(dev);
}

u32 balloc(int dev)
{
  // YOU DO IT
 int i;
 char buf[BLKSIZE];

 // get blockbBitmap into buf
 get_block(dev, bmap, buf);
 
 for (i=0; i < nblocks; i++){
   if (tst_bit(buf, i)==0){
     set_bit(buf, i);
     put_block(dev, bmap, buf);

     // update free block count in SUPER and GD
     decFreeBlocks(dev);
     printf("balloc: ino=%d\n", i+1);
     return (i+1);
   }
 }
 return 0;
}


int bdealloc(int dev, int bit)
{
  // YOU DO IT
  int i;  
  char buf[BLKSIZE];

  if (bit > nblocks){
    printf("bit %d out of range\n", bit);
    return;
  }

  // get block bitmap block
  get_block(dev, bmap, buf);
  clr_bit(buf, bit-1);

  // write buf back
  put_block(dev, bmap, buf);

  // update free block count in SUPER and GD
  incFreeBlocks(dev);

}

OFT* oftalloc()
{
  int i;
  for(i=0;i<NOFT;i++)
  {
    printf("oftalloc i=%d\n",i);
    if(oft[i].refCount==0)
    //if(running->fd[i]==0)
    {
      printf("oft[%d] refcount  is 0\n",i);
      //running->fd[i]->refCount=0;
      //OFT* fdoft=running->fd[i];
      return &oft[i];
    }
  }
  printf("OPT is full.\n");
  return 0;
}

