#include <unistd.h>
#include "strerr.h"
#include "error.h"
#include "open.h"

#define FATAL "svok: fatal: "

int main(int argc,char **argv)
{
  int fd;

  if (!argv[1])
    strerr_die1x(100,"svok: usage: svok dir");

  if (chdir(argv[1]) == -1)
    strerr_die4sys(111,FATAL,"unable to chdir to ",argv[1],": ");

  fd = open_write("supervise/ok");
  if (fd == -1) {
    if (errno == error_noent) _exit(100);
    if (errno == error_nodevice) _exit(100);
    strerr_die4sys(111,FATAL,"unable to open ",argv[1],"/supervise/ok: ");
  }

  _exit(0);
}
