/**********************chmod.c*********************/
#define SS_IRUSR 000400
#define SS_IWUSR 000200
#define SS_IXUSR 000100
#define SS_IRGRP 000040
#define SS_IWGRP 000020
#define SS_IXGRP 000010
#define SS_IROTH 000004
#define SS_IWOTH 000002
#define SS_IXOTH 000001

int _chmod(char* parameter,char* filename);

int _chmod(char* parameter,char* filename)
{
  int ino,dev;
  INODE* ip;
  MINODE* mip;
  u16 modes[3];
  modes[0]=0;
  modes[1]=0;
  modes[2]=0;
  if(!*filename ||strlen(parameter)==0 || strlen(parameter)>4)
  {
    printf("Invalide command.\n");
    return -1;
  }
  
  printf("para[0] is %c, para[1] is %c, para[2] is %c\n",parameter[1],parameter[2],parameter[3]);
  dev=running->cwd->dev;
  printf("dev = %d\n",dev);
  ino = getino(&dev,filename);
  if(ino!=-1)
 {
  mip=iget(dev,ino);
  ip=&(mip->INODE);

  if(parameter[1]=='0')
    modes[0]=0;
  if(parameter[1]=='1')
    modes[0]=SS_IXUSR;
  if(parameter[1]=='2')
    modes[0]=SS_IWUSR;
  if(parameter[1]=='3')
    modes[0]=SS_IWUSR|SS_IXUSR;
  if(parameter[1]=='4')
    modes[0]=SS_IRUSR;
  if(parameter[1]=='5')
    modes[0]=SS_IRUSR|SS_IXUSR;
  if(parameter[1]=='6')
    modes[0]=SS_IRUSR|SS_IWUSR;
  if(parameter[1]=='7')
    modes[0]=SS_IRUSR|SS_IWUSR|SS_IXUSR;

  if(parameter[2]=='0')
    modes[1]=0;
  if(parameter[2]=='1')
    modes[1]=SS_IXGRP;
  if(parameter[2]=='2')
    modes[1]=SS_IWGRP;
  if(parameter[2]=='3')
    modes[1]=SS_IWGRP|SS_IXGRP;
  if(parameter[2]=='4')
    modes[1]=SS_IRGRP;
  if(parameter[2]=='5')
    modes[1]=SS_IRGRP|SS_IXGRP;
  if(parameter[2]=='6')
    modes[1]=SS_IRGRP|SS_IWGRP;
  if(parameter[2]=='7')
    modes[1]=SS_IRGRP|SS_IWGRP|SS_IXGRP;

  if(parameter[3]=='0')
    modes[2]=0;
  if(parameter[3]=='1')
    modes[2]=SS_IXOTH;
  if(parameter[3]=='2')
    modes[2]=SS_IWOTH;
  if(parameter[3]=='3')
    modes[2]=SS_IWOTH|SS_IXOTH;
  if(parameter[3]=='4')
    modes[2]=SS_IROTH;
  if(parameter[3]=='5')
    modes[2]=SS_IROTH|SS_IXOTH;
  if(parameter[3]=='6')
    modes[2]=SS_IROTH|SS_IWOTH;
  if(parameter[3]=='7')
    modes[2]=SS_IROTH|SS_IWOTH|SS_IXOTH;


  printf("modes is %o",modes);
  ip->i_mode |= modes[0]|modes[1]|modes[2];
  mip->dirty=1;
  iput(mip);
  return 0;
}
  printf("ino is -1, invalid\n");
  return -1;
}
