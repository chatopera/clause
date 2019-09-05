#include "iopause.h"
#include "deepsleep.h"

extern void deepsleep(unsigned int s)
{
  struct taia now;
  struct taia deadline;
  iopause_fd x;

  taia_now(&now);
  taia_uint(&deadline,s);
  taia_add(&deadline,&now,&deadline);

  for (;;) {
    taia_now(&now);
    if (taia_less(&deadline,&now)) return;
    iopause(&x,0,&deadline,&now);
  }
}
