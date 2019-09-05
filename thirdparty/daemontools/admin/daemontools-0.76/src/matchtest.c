#include "match.h"
#include "buffer.h"
#include "str.h"

int main(int argc,char **argv)
{
  const char *pattern = "";
  const char *buf = "";

  if (argv[1]) {
    pattern = argv[1];
    if (argv[2])
      buf = argv[2];
  }

  buffer_puts(buffer_1,match(pattern,buf,str_len(buf)) ? "+" : "-");
  buffer_puts(buffer_1,pattern);
  buffer_puts(buffer_1," (");
  buffer_puts(buffer_1,buf);
  buffer_puts(buffer_1,")\n");
  buffer_flush(buffer_1);
  _exit(0);
}
