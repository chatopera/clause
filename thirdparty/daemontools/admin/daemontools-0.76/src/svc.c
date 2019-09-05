#include <unistd.h>
#include "ndelay.h"
#include "strerr.h"
#include "error.h"
#include "open.h"
#include "sgetopt.h"
#include "buffer.h"
#include "byte.h"
#include "sig.h"

#define FATAL "svc: fatal: "
#define WARNING "svc: warning: "

int datalen = 0;
char data[20];

buffer b;
char bspace[1];

int fdorigdir;

int main(int argc,const char *const *argv)
{
  int opt;
  int fd;
  const char *dir;

  sig_ignore(sig_pipe);

  while ((opt = getopt(argc,argv,"udopchaitkx")) != opteof)
    if (opt == '?')
      strerr_die1x(100,"svc options: u up, d down, o once, x exit, p pause, c continue, h hup, a alarm, i interrupt, t term, k kill");
    else
      if (datalen < sizeof data)
        if (byte_chr(data,datalen,opt) == datalen)
          data[datalen++] = opt;
  argv += optind;

  fdorigdir = open_read(".");
  if (fdorigdir == -1)
    strerr_die2sys(111,FATAL,"unable to open current directory: ");

  while (dir = *argv++) {
    if (chdir(dir) == -1)
      strerr_warn4(WARNING,"unable to chdir to ",dir,": ",&strerr_sys);
    else {
      fd = open_write("supervise/control");
      if (fd == -1)
        if (errno == error_nodevice)
          strerr_warn4(WARNING,"unable to control ",dir,": supervise not running",0);
        else
          strerr_warn4(WARNING,"unable to control ",dir,": ",&strerr_sys);
      else {
        ndelay_off(fd);
        buffer_init(&b,buffer_unixwrite,fd,bspace,sizeof bspace);
        if (buffer_putflush(&b,data,datalen) == -1)
          strerr_warn4(WARNING,"error writing commands to ",dir,": ",&strerr_sys);
        close(fd);
      }
    }
    if (fchdir(fdorigdir) == -1)
      strerr_die2sys(111,FATAL,"unable to set directory: ");
  }

  _exit(0);
}
