#include <unistd.h>
#include "byte.h"
#include "open.h"
#include "error.h"
#include "direntry.h"
#include "stralloc.h"
#include "openreadclose.h"
#include "strerr.h"
#include "pathexec.h"

#define FATAL "envdir: fatal: "

void die_usage(void)
{
  strerr_die1x(111,"envdir: usage: envdir dir child");
}
void nomem(void)
{
  strerr_die2x(111,FATAL,"out of memory");
}

static stralloc sa;

int main(int argc,const char *const *argv)
{
  int fdorigdir;
  const char *fn;
  DIR *dir;
  direntry *d;
  int i;

  if (!*argv) die_usage();

  if (!*++argv) die_usage();
  fn = *argv;

  if (!*++argv) die_usage();

  fdorigdir = open_read(".");
  if (fdorigdir == -1)
    strerr_die2sys(111,FATAL,"unable to switch to current directory: ");
  if (chdir(fn) == -1)
    strerr_die4sys(111,FATAL,"unable to switch to directory ",fn,": ");

  dir = opendir(".");
  if (!dir)
    strerr_die4sys(111,FATAL,"unable to read directory ",fn,": ");
  for (;;) {
    errno = 0;
    d = readdir(dir);
    if (!d) {
      if (errno)
        strerr_die4sys(111,FATAL,"unable to read directory ",fn,": ");
      break;
    }
    if (d->d_name[0] != '.') {
      if (openreadclose(d->d_name,&sa,256) == -1)
        strerr_die6sys(111,FATAL,"unable to read ",fn,"/",d->d_name,": ");
      if (sa.len) {
        sa.len = byte_chr(sa.s,sa.len,'\n');
        while (sa.len) {
	  if (sa.s[sa.len - 1] != ' ')
	    if (sa.s[sa.len - 1] != '\t')
	      break;
	  --sa.len;
        }
        for (i = 0;i < sa.len;++i)
	  if (!sa.s[i])
	    sa.s[i] = '\n';
        if (!stralloc_0(&sa)) nomem();
        if (!pathexec_env(d->d_name,sa.s)) nomem();
      }
      else {
        if (!pathexec_env(d->d_name,0)) nomem();
      }
    }
  }
  closedir(dir);

  if (fchdir(fdorigdir) == -1)
    strerr_die2sys(111,FATAL,"unable to switch to starting directory: ");
  close(fdorigdir);

  pathexec(argv);
  strerr_die4sys(111,FATAL,"unable to run ",*argv,": ");
}
