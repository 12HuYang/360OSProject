/*Algorithm of symlink(old_file, new_file)
{
1. check: old_file must exist and new_file not yet exist;
2. create new_file; change new_file to SLINK type;
3. // assume length of old_file name <= 60 chars
store old_file name in newfile's INODE.i_block[ ] area.
mark new_file's minode dirty;
iput(new_file's minode);
4. mark new_file parent minode dirty;
put(new_file's parent minode);
}*/

//check function
//symlink creat function

u32 symlink_creat(MINODE* pip, char* name)
{
    int i;
    MINODE* mip;
    u32 inumber, bnumber;
    char buf[BLKSIZE];
    char* cp;
    int need_length, ideal_length, rec_len;

    inumber = ialloc(pip->dev);

    mip = iget(pip->dev, inumber);
    mip->INODE.i_mode = 0x8000;
    mip->INODE.i_uid = running->uid;
    mip->INODE.i_gid = running->gid;
    mip->INODE.i_size = 0;

    mip->INODE.i_links_count = 1;
    mip->INODE.i_atime = mip->INODE.i_ctime = mip->INODE.i_mtime = time(NULL);

    mip->INODE.i_blocks = 0;
    for (i = 0; i < 15; i++)
        mip->INODE.i_block[i] = 0;
    mip->dirty = 1;

    iput(mip);

    // enter name into parent's directory
    need_length = 4 * ((8 + strlen(name) + 3) / 4);

    //put_rec(pip, name, inumber);
    for (i = 0; i < 12; i++)
    {
        get_block(pip->dev, (pip->INODE).i_block[i], buf);
        cp = buf;
        dp = (DIR*)buf;
        while (cp < (buf + BLKSIZE))
        {
            dp = (DIR*)cp;
            cp += dp->rec_len;
        }
        cp -= dp->rec_len;

        // dp now points to the LAST entry
        ideal_length = 4 * ((8 + dp->name_len + 3) / 4);
        rec_len = dp->rec_len;
        if (rec_len - ideal_length >= need_length)
        {
            // enter the new entry as the LAST entry and trim the previous
            // entry to its ideal length
            dp->rec_len = ideal_length;
            cp += dp->rec_len;
            dp = (DIR*)cp;
            dp->inode = inumber;
            dp->name_len = strlen(name);
            dp->file_type = EXT2_FT_REG_FILE;
            strncpy(dp->name, name, dp->name_len);
            dp->rec_len = rec_len - ideal_length;
            put_block(pip->dev, (pip->INODE).i_block[i], buf);
            break;
        }
        else
        {
            if (0 == (pip->INODE).i_block[i + 1])
            {
                // allocate a new data block
                // enter the new entry as the first entry in the new data block
                get_block(pip->dev, (pip->INODE).i_block[i + 1], buf);
                dp = (DIR*)buf;
                dp->inode = inumber;
                dp->name_len = strlen(name);
                dp->file_type = EXT2_FT_REG_FILE;
                strncpy(dp->name, name, dp->name_len);
                dp->rec_len = BLKSIZE;
                put_block(pip->dev, (pip->INODE).i_block[i + 1], buf);
                break;
            }
        }
    }

    pip->INODE.i_atime = time(NULL);
    pip->dirty = 1;

    //iput(pip);
    return inumber;
}


void _symlink(char*old_name, char*filename)
{
   int ino, i;
   int lino;
   int pino;
   char temp[64], parent[64], child[64];

   MINODE *mip;
   MINODE *pmip;
   MINODE *lmip;

   INODE *ip;
   INODE *lip;
   int dev= running ->cwd->dev;

   strcpy(temp, old_name);
   //strcpy(old_name, basename(filename));

   //get inode of old file
   ino = getino(&dev, old_name);
   mip = iget(dev, ino);

   if(strlen(filename) >= 60)
     {
       printf("ERROR: name too long\n");
       return;
     }

   if(!mip)
     {
       printf("ERROR: %s does not exist\n", filename);
       return;
     }

   //get parent and child of old file pathname
   //strcpy(temp, old_name);
   strcpy(parent, dirname(temp));
   strcpy(child, basename(old_name));

   printf("parent is %s,  child is %s\n", parent, child);

   pino = getino(&dev, parent);
   pmip = iget(dev, pino);

   if(!pmip)
     {
       printf("ERROR: can't get parent mip\n");
       return;
     }

   if(pmip->INODE.i_mode & 004000 != 004000)
     {
       printf("ERROR: parent is not a directory\n");
       return;
     }

   if(getino(&dev, filename) > 0)
     {
       printf("ERROR: %s already exists\n", filename);
       return;
     }

   //strcat(filename,"->");
   //strcat(filename,child);
   //lino = symlink_creat(pmip,filename);
   lino=kcreat(pmip,filename);
   //gets the newfile minode to set it's variables
   lmip = iget(dev, lino);
   lip = &lmip->INODE;

   //set the link mode
   lip->i_mode = 0120000;
   //set the link size, which is the size of the oldfiles name
   lip->i_size = strlen(child);
   char* block = (char*)&lip->i_block;
   char ttemp[256];
   strcpy(&ttemp,"/");
   if(strcmp(parent,".")&&strcmp(parent,".."))
   {
     strncpy(&ttemp,parent,strlen(parent));
     strcat(&ttemp,"/");
   }
   child[strlen(child)]='\0';
   strcat(&ttemp,child);  
   printf("ttemp is %s",ttemp);   
   
   strcpy(block,ttemp);
   lmip->dirty = 1;
   iput(lmip);
   iput(mip);
   

}

//Alg for readlink (file,buffer)
//1. get file's MINODE, verify it's a SLINK file
//2. copy target filename in i_block into a buffer;
//3. return strlen(char* mip->INODE.i_block)
int readlink(char* filename,char* buf)
{
  MINODE* mip;
  int ino,dev,content;
  INODE* ip;
  
  if(!*filename)
  {
    printf("Invalid filename.\n");
    return -1;
  }
  ino=getino(&dev,filename);
  mip=iget(dev,ino);
  ip=&mip->INODE;
  if((ip->i_mode & 0xA000) != 0xA000){
    iput(mip);
    printf("file %s is not a SLINK.\n",filename);
    return -1;
  }
  content = ip->i_block[0];
  printf("LINK: %s\n",(char*)&content);
  strcpy(buf,(char*)&content);
  //printf("finish strcpy\n");
  //printf("readlink: %s symlink to %s\n",filename,(char*)&ip->i_block[16]);
  iput(mip);
}


