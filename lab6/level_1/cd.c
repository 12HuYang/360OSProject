void _chdir(char* pathname) //return type int or void?
{
    MINODE* mip;
    //get INODE of pathname into a minode
    if(*pathname && strcmp(pathname,"/"))
    {
        u32 ino;
	int dev[2];
        dev[0]=running->cwd->dev;
        dev[1]=0;
	ino = getino(&dev,pathname);
        printf("chdir pathname: %s\n",pathname);
        printf("chdir ino: %d dev: %d\n", ino,dev[0]);
        if(ino!=-1)
        {
            mip=iget(dev[0],ino);
            //printf("chdir ino: \n",ino);
            INODE *mino=&(mip->INODE);
            //printf("i_mode: %x",mino->i_mode);
            if(mino->i_mode != 0x41ED)//varifiy if it is a dir
            {
                printf("chdir says: %s is not a dir.\n",pathname);
                return;
            }
            //change running process CWD to minode pathname
            iput(running->cwd);
            running->cwd=mip;
            //INODE *lip=&(mip->INODE);
            //printf("lip blocks %d\n",lip->i_blocks);
            printf("running is [%d %d]\n",running->cwd->dev,running->cwd->ino);
	    printf("root is [%d %d]\n",root->dev,root->ino);
            return;

        }
        else{
            printf("could not get inode of %s\n",pathname);
            return;
	}
    }
    else{
        //chdir to root
        iput(running->cwd);
        running->cwd=root;
        return;
    }

    return;

}
