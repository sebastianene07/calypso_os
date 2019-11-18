#include <board.h>
#include <console_main.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>

#include <source/ff.h> 

static FRESULT scan_files(char* path)
{
    FRESULT res;
    FILINFO fno;
    DIR dir;
    int i;
    char *fn;   /* This function is assuming non-Unicode cfg. */
#if _USE_LFN
    static char lfn[_MAX_LFN + 1];
    fno.lfname = lfn;
    fno.lfsize = sizeof lfn;
#endif


    res = f_opendir(&dir, path);                       /* Open the directory */
    if (res == FR_OK) {
        i = strlen(path);
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (fno.fname[0] == '.') continue;             /* Ignore dot entry */
#if _USE_LFN
            fn = *fno.lfname ? fno.lfname : fno.fname;
#else
            fn = fno.fname;
#endif
            if (fno.fattrib & AM_DIR) {                    /* It is a directory */
                sprintf(&path[i], "/%s", fn);
                res = scan_files(path);
                if (res != FR_OK) break;
                path[i] = 0;
            } else {                                       /* It is a file. */
                printf("%s/%s\n", path, fn);
            }
        }
    }

    return res;
}

int console_mount(int argc, const char *argv[])
{
  FATFS *fs = malloc(sizeof(FATFS));
  if (fs == NULL) {
    printf("Error: FatFS not initialized, not enough mem\n");
    return -ENOMEM;
  } else {
    FRESULT ret = f_mount(fs, "", 1);
    if (ret != FR_OK) {
      printf("Error: %d cannot mount FatFS\n", ret);
      free(fs);
      return -ENOSYS;
    }
  }

  char *path = malloc(256);
  memset(path, 0, 256);
  strncpy(path, "/", 1);

  printf("Filesystem mounted, now read files\n");

  scan_files(path);
  free(path);

  f_mount(NULL, "", 0);
  free(fs);

  return OK;
}
