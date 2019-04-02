#include "include/string.h"
#include <stdio.h>
#include <stdlib.h>

static char *my_text = "https://www.google.ro/search?client=ubuntu&ei=0PSiXLnjEeihrgSkq7SADA&q=parameters+in+get+request&oq=parameters+in+get+reques&gs_l=psy-ab.1.0.0l2j0i22i30l8.2370.8875..9446...4.0..0.107.2488.27j1......0....1..gws-wiz.......0i71j0i67j0i10.KoheGnwf3to";

int main(int argc, char *argv[])
{
  int sz = strlen(my_text);
  printf("Before tokenize size %d\n===========\n", sz);

  char *copy_text = calloc(sz + 1, 1);
  if (copy_text == NULL)
  {
    printf("Muie psd\n");
    return -1;
  }

  /* Copy text into a non readonline region */

  memcpy(copy_text, my_text, sz);


  /* Tokenize it */

  char *context = NULL;
  char *word = NULL;

  strtok_r(copy_text, "/", &context);
  while ((word = strtok_r(NULL, ":/=?&*", &context)) != NULL)
  {
    if (strlen(word) > 0)
    {
      printf("Extracted word:%s\n", word);
    }
  }

  printf("===========\nAfter tokenize\n");
  free(copy_text);

  return 0;
}
