/*1. get in memory INODE of pathname: -
ino = getino(&de, pathanme);
mip = iget(dev,ino);
2. verify INODE is a DIR (by INODE.i_mode field);
minode is not BUSY (refCount = 1);
DIR is empty (traverse data blocks for number of entries = 2);
3. /* get parent's ino and inode 
pino = findino(); //get pino from .. entry in INODE.i_block[0]
pmip = iget(mip->dev, pino);
4. /* remove name from parent directory 
findname(pmip, ino, name); //find name from parent DIR
rm_child(pmip, name);
5. /* deallocate its data blocks and inode 
truncat(mip); // deallocate INODE's data blocks
6. deallocate INODE
idalloc(mip->dev, mip->ino); iput(mip);
7. dec parent links_count by 1;
mark parent dirty; iput(pmip);
8. return 0 for SUCCESS*/

void _rmdir(char* pathname)
{
    u32 ino;
    u32 pino;
    int pdev,cdev;
    MINODE* mip;
    MINODE* pip;
    int i;
    int dev;
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

    //get in memory INODE of pathname
    dev =  running->cwd->dev;
    ino = getino(&dev, pathname);
    mip = iget(dev, ino);
    //verify INODE is a DIR
    if ((mip->INODE).i_mode & 004000 == 004000)
      {
	//minode is not busy and dir is empty
	if((mip->refCount = 1) && emptyDir(mip)==0)
	  {
	    //for non indirect block
	    /* for (i = 0; i < 12; i++)
	      {
		if (0 != (mip->INODE).i_block[i])
		  {
		    bdealloc(mip->dev, (mip->INODE).i_block[i]);
		  }
		  }*/

	    //get parent's ino and inode
	        if(findcpino(mip, &ino, &pino,&pdev,&cdev)!=-1)
		{
		    idealloc(mip->dev, mip->ino);

		    pip = iget(mip->dev, pino);

		    rm_child(pip, basename(pathname));
                    INODE* ip=&pip->INODE;
                    ip->i_links_count--;
		    (pip->INODE).i_atime = time(NULL);
		    (pip->INODE).i_mtime = (pip->INODE).i_atime;
		    pip->dirty = 1;
			
		}
	    iput(pip);
	  }
      }
    iput(mip);
}

int emptyDir(MINODE *mip)
{
    char buf[1024];
    INODE *ip = &mip->INODE;
    char *cp;
    char name[64];
    DIR *dp;

    //link count greater than 2 has files
    if(ip->i_links_count > 2)
      {
	return -1;
      }
    else if(ip->i_links_count == 2)
      {
	//check data blocks
	if(ip->i_block[1])
	  {
	    get_block(dev, ip->i_block[1], buf);

	    cp = buf;
	    dp = (DIR*)buf;

	    while(cp < buf + BLKSIZE)
	      {
		strncpy(name, dp->name, dp->name_len);
		name[dp->name_len] = 0;

		if(strcmp(name, ".") != 0 && strcmp(name, "..") != 0)
		  {
		    return -1;
		  }
	      }
	  }
      }
    return 0;
}

int rm_child(MINODE *pip, char *my_name)
{
    int i;
    char buf[BLKSIZE];
    char replace;
    int j = 0;
    DIR *pdp;

    for (i = 0; i < 12; i++)
    {
        get_block(pip->dev, (pip->INODE).i_block[i], buf);
        j = 0;
        dp = (DIR *) buf;

        while ((char *) dp < (buf + BLKSIZE) && dp->rec_len)
        {
            replace = dp->name[dp->name_len];
            dp->name[dp->name_len] = '\0';
            if (j)
            {
                if (((char *) dp) + dp->rec_len >= buf + BLKSIZE - 1)
                {
                    dp->rec_len += j;
                }
                dp->name[dp->name_len] = replace;
		//move memory left
                memmove(((char *) dp) - j, dp, dp->rec_len);
            }
            else if (0 == strcmp(my_name, dp->name))
            {
                dp->name[dp->name_len] = replace;
                if (BLKSIZE == dp->rec_len)
                {
                    printf("rm_child: i_block[%d] = %d\n",i,(pip->INODE).i_block[i]);
                    bdealloc(pip->dev, (pip->INODE).i_block[i]);
                    (pip->INODE).i_block[i] = 0;
                    (pip->INODE).i_size -= BLKSIZE;
                    return 0;
                }
                else if (((char *) dp) + dp->rec_len >= buf + BLKSIZE)
                {
                    pdp->rec_len += dp->rec_len;
                }
                else
                {
                    j = dp->rec_len;
                }
            }
            else
            {
                dp->name[dp->name_len] = replace;
            }
            pdp = dp;
            dp = (DIR *) (((char *) dp) + dp->rec_len);
        }
        put_block(pip->dev, (pip->INODE).i_block[i], buf);
    }
    return 0;
}
