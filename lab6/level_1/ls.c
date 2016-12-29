void ls(char *pathname){
    int ino,dev[2];
    dev[0] =running->cwd->dev;
    dev[1]=0;
    char lbuf[BLKSIZE],*cp;
    MINODE *mip = running->cwd;
    int i, j;
    char temp[BLKSIZE],t_temp[BLKSIZE];

    if (strcmp(pathname,"")) {   // ls pathname:
        //if (pathname[0] == '/')
        //    dev = root->dev;
        ino = getino(&dev, pathname);
        printf("dev[0]=%d,dev[1]=%d\n",dev[0],dev[1]);
        printf("ls() dev=%d,ino=%d\n",dev[0],ino);
	mip = iget(dev[0], ino);
        //printf("have pathname\n");
    }

    // mip points at minode;
    // Each data block of mip->INODE contains DIR entries
    // print the name strings of the DIR entries
    INODE *lip=&mip->INODE;

    for(i=0; i<12 ;i++) {

        get_block(dev[0], lip->i_block[i], lbuf);
        //dp = (DIR *) lbuf;
        // cp = (char *) lbuf;
        cp = lbuf;
        dp = (DIR*)lbuf;
        while (cp < lbuf + BLKSIZE && dp->rec_len) {
            strncpy(temp, dp->name, dp->name_len);
            temp[dp->name_len] = 0;
            time_t t;
            u32 dino=dp->inode;
            MINODE* p=iget(dev[0],dino);
            if ((p->INODE.i_mode & 0120000) == 0120000) {
                printf("l");
                //strcat(temp,"->");
                char namebuf[60];
                //readlink(temp,&namebuf);
                char* content = (char*)&p->INODE.i_block;
                //printf("content is %s",content);
                strcpy(&namebuf,content);
                strcat(temp,"->");
                strcat(temp,namebuf);
            } //symbolic link

            else if ((p->INODE.i_mode & 0100000) == 0100000) {
                printf("-");
            }//regular file

            else if ((p->INODE.i_mode & 0040000) == 0040000) {
                printf("d");
            }//directory

            for (j = 8; j >= 0; j--) {
                if (p->INODE.i_mode & (1 << j)) {
                    switch (j % 3) {
                        case 2:
                            printf("r");//read
                            break;
                        case 1:
                            printf("w");//write
                            break;
                        case 0:
                            printf("x");//execute
                            break;
                    }
                } else {
                    printf("-");
                }
            }

            t = p->INODE.i_mtime;
            strcpy(t_temp, ctime(&t));
            t_temp[strlen(t_temp) - 1] = 0; //remove newline
            printf(" %d %d %d %8d %s %s\n", p->INODE.i_links_count, p->INODE.i_uid, p->INODE.i_gid, p->INODE.i_size,t_temp,temp);
            //printf("%s %s\n", t_temp,temp);
            cp += dp->rec_len;
            dp = (DIR *) cp;
            iput(p);
        }
    }
    iput(mip);
}
