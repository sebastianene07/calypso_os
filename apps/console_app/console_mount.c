#include <board.h>
#include <console_main.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>

#include <source/ff.h>
#include <vfs.h>
#include <stdio.h>

static FATFS *fs;

static void emit_vfs_node(const char *mount_path, const char *name, enum vfs_node_type node_type)
{
  size_t path_len = strlen(mount_path) + strlen(name) + 2;
  char *path = calloc(1, path_len);
  if (path == NULL) {
    return;
  }

  snprintf(path, path_len, "/%s/%s", mount_path, name);
  int ret = vfs_register_node(path,
                              strlen(path),
                              NULL,
                              node_type,
                              NULL);

  printf("Creating %s: %s status %d\n", node_type == VFS_TYPE_DIR ? "DIR" : "FILE", path, ret);
}

static FRESULT scan_files(char *path)
{
    FRESULT res;
    FILINFO fno;
    DIR dir;
    int i;
    char *fn;   /* This function is assuming non-Unicode cfg. */

    res = f_opendir(&dir, path);                       /* Open the directory */
    if (res == FR_OK) {
        i = strlen(path);
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (fno.fname[0] == '.') continue;             /* Ignore dot entry */

            fn = fno.fname;

            if (fno.fattrib & AM_DIR) {                    /* It is a directory */
                sprintf(&path[i], "/%s", fn);
                emit_vfs_node("mnt", path, VFS_TYPE_DIR);

                res = scan_files(path);
                if (res != FR_OK) break;
                path[i] = 0;
            } else {

              char *m_path = calloc(1, 80);
              if (m_path == NULL) {
                return res;
              }

              sprintf(m_path, "%s/%s", path, fn);
              emit_vfs_node("mnt", m_path, VFS_TYPE_FILE);
              free(m_path);
            }
        }
    }

    return res;
}

int console_mount(int argc, const char *argv[])
{
  fs = malloc(sizeof(FATFS));
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

  printf("Filesystem mounted.\n");

  scan_files(path);
  free(path);

  return OK;
}

int console_umount(int argc, const char *argv[]) {
  if (fs == NULL) {
    printf("There is no FS mounted\n");
  }

  f_mount(NULL, "", 0);
  free(fs);
  fs = NULL;
}
