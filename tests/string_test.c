#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static char *my_text = "https://www.google.ro/search?client=ubuntu&ei=0PSiXLnjEeihrgSkq7SADA&q=parameters+in+get+request&oq=parameters+in+get+reques&gs_l=psy-ab.1.0.0l2j0i22i30l8.2370.8875..9446...4.0..0.107.2488.27j1......0....1..gws-wiz.......0i71j0i67j0i10.KoheGnwf3to";

static void test(const char *test_name, const char *text, const char *delim)
{
  printf("\n[Test %s]\n", test_name);

  int sz = strlen(my_text);
  printf("Before tokenize size %d\n===========\n", sz);

  char *copy_text = calloc(sz + 1, 1);
  if (copy_text == NULL)
  {
    return;
  }

  /* Copy text into a writable region */

  memcpy(copy_text, my_text, sz);

  /* Tokenize it */

  char *context = NULL;
  char *word = NULL;

  word = strtok_r(copy_text, delim, &context);
  printf("First word:%s\n", word);

  while ((word = strtok_r(NULL, delim, &context)) != NULL)
  {
    if (strlen(word) > 0)
    {
      printf("Extracted word:%s\n", word);
    }
  }

  printf("===========\nAfter tokenize\n");
  free(copy_text);
}

int main(int argc, char *argv[])
{
  /* Run some unit tests to verify the functionality against GLibc functions */

  test("1", my_text, "/");
  test("2", my_text, "=");
  test("3", my_text, "?");
  test("4", my_text, "/:=");
  test("5", my_text, "/=?:&+");
  return 0;
}
