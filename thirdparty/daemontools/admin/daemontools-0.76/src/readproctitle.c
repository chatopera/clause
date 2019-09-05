#include <unistd.h>
#include "error.h"

int main(int argc,char **argv)
{
  char *buf;
  unsigned int len;
  int i;
  char ch;

  if (argc < 2) _exit(100);
  buf = argv[argc - 1];
  len = 0;
  while (buf[len]) buf[len++] = '.';
  if (len < 5) _exit(100);

  for (;;)
    switch(read(0,&ch,1)) {
      case 1:
	if (ch) {
	  for (i = 4;i < len;++i) buf[i - 1] = buf[i];
	  buf[len - 1] = ch;
	}
	break;
      case 0:
	_exit(0);
      case -1:
	if (errno != error_intr) _exit(111);
    }
}
