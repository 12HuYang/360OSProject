void my_cat(char *filename)
{
  int n, i;
  int fd = 0;
  char buf[1024];

  //check for path
  if(!filename)
    {
      return;
    }

  //open with 0 for RD
  fd = _open(filename, 0);

  //printf("fd is %d\n", fd);
  while(n = my_read(fd, buf, 1024))
    {
      //null terminate the buffer
      buf[n] = '\0';

      i = 0;
      //print each char in the buffer, this is to handle \n
      while(buf[i])
	{
	  putchar(buf[i]);
	  if(buf[i] == '\n')
	    putchar('\r');
	  i++;
	}
    }

  _close(fd);

  return;
}
