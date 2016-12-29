int findpath(MINODE* cwd,char* cwdpath)
{
    MINODE *pmip,*cmip;
    int chino,pino,pdev,cdev;
    char dirname[64];
    int i;
    dirname[0]='\0';
    //INODE* lip=&(cwd->INODE);
    //printf("lip blocks %d\n",lip->i_blocks);
    if(findcpino(cwd,&chino,&pino,&pdev,&cdev)!=-1)
    {
        //printf("chino=%d,cdev=%d,pino=%d,pdev=%d\n",chino,cdev,pino,pdev);
        //getchar();
        pmip=iget(pdev,pino);
	cmip=iget(cdev,chino);
        findpath(pmip,cwdpath);
//       printf("first cwdpath is %s",cwdpath);
	   // strcat(cwdpath,"/");
	    memset(&dirname,'\0',sizeof(dirname));
 	    if(pdev!=cdev)  //across mount point
	    {
		MINODE* tpmip=iget(cdev,2);
		if(getinoname(tpmip,chino,&dirname)!=-1&&strcmp(dirname,"."))
		{
	            strcat(cwdpath,"/");
		    strcat(cwdpath,dirname);
                }
	    }
	    else{ 
	    if(getinoname(pmip,chino,&dirname)!=-1)
	     {
	        strcat(cwdpath,"/");
	        strcat(cwdpath,dirname);
	     }
	   }
        iput(pmip);
    }

}
int getcwd()
{
    char cwdpath[128];
    memset(cwdpath,'\0', sizeof(cwdpath));
    printf("root dev: %d ino: %d\n",root->dev,root->ino);
    if(running->cwd->dev==root->dev && running->cwd->ino==root->ino) {
        printf("/\n");
        return 0;
    }
    findpath(running->cwd,&cwdpath);
    printf("%s\n",cwdpath);

}
