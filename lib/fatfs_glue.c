#include <board.h>

#include <errno.h>
#include <filesystems.h>
#include <stdio.h>
#include <string.h>
#include <vfs.h>
#include <unistd.h>

#include "fatfs/source/ff.h"
#include "fatfs/source/diskio.h"

/****************************************************************************
 * Preprocessor Definitions
 ****************************************************************************/

/* The size of a sector in bytes */

#define SECTOR_SIZE_BYTES                (512U)

/* The max path depth */

#define FATFS_MAX_PATH                   (256U)

/* Friendly name for file system */

#define FILESYSTEM_TYPE_FAT              "FAT"
#define FILESYSTEM_TYPE_EXFAT            "EXFAT"

/****************************************************************************
 * Private Method Prototypes
 ****************************************************************************/

static int open_fs_node(struct opened_resource_s *priv, const char *pathname,
                        int flags, mode_t mode);
static int read_fs_node(struct opened_resource_s *priv, void *buf, size_t count);
static int write_fs_node(struct opened_resource_s *priv, const void *buf,
                         size_t count);
static int close_fs_node(struct opened_resource_s *priv);

/****************************************************************************
 * Private Data
 ****************************************************************************/

/* The MTD operations structure */

static struct mtd_ops_s *g_mtd_ops;

/* Fat file system instance */

static FATFS *g_fatfs;

/* Mounted file system */

struct vfs_mount_filesystem_s *g_mounted_fs;

/* Supported file system operations */

static struct vfs_ops_s g_fs_ops = {
  .open   = open_fs_node,
  .close  = close_fs_node,
  .read   = read_fs_node,
  .write  = write_fs_node,
};

/****************************************************************************
 * Private Methods
 ****************************************************************************/

/*
 * emit_vfs_node - creates a new node in the virtual file system for a FS object
 *
 * @mount_path - the readable location where we mount the file system
 * @name       - the file name with the path relative to this file system
 * @node_type  - the type of the VFS object (regular file or dir)
 *
 */
static int emit_vfs_node(const char *mount_path, const char *name,
                         enum vfs_node_type node_type)
{
  size_t mnt_path_len = strlen(mount_path);
  size_t name_len = strlen(name);
  size_t path_len = strlen(mount_path) + strlen(name) + 2;
  char *path = calloc(1, path_len);
  if (path == NULL) {
    return -ENOMEM;
  }

  int j = 0;
  for (int i = 0; i < mnt_path_len; i++) {

    path[j] = mount_path[i];

    if (mount_path[i] == '/' && j > 0 && path[j - 1] == '/') {
      continue;
    }

    j++;
  }

  if (path[j] == '\0') {
    path[j] = '/';
  } else if (path[j] != '/') {
    path[++j] = '/';
  } 

  for (int i = 0; i < name_len; i++) {
    path[j] = name[i];

    if (name[i] == '/' && j > 0 && path[j - 1] == '/') {
      continue;
    }

    j++;
  }

  int ret = vfs_register_node(path,
                              strlen(path),
                              &g_fs_ops,
                              node_type,
                              path);

  printf("Creating %s: %s status %d\n",
         node_type == VFS_TYPE_DIR ? "DIR" : "FILE", path, ret);
  return ret;
}

/*
 * scan_files - this method lists the internal file system contents
 *
 * @path - the readable location where we mount the file system
 *
 * WARNING: This method is recursive and it can blow the stack for deep
 *          directory structures. !!! FIXME
 */
static FRESULT scan_files(char *path)
{
    FRESULT res;
    static FILINFO fno;
    DIR dir;
    int i;
    char *fn;

    /* Open the directory */
    res = f_opendir(&dir, path);
    if (res == FR_OK) {
        i = strlen(path);
        for (;;) {

            res = f_readdir(&dir, &fno);
            if (res != FR_OK || fno.fname[0] == 0) break;
            if (fno.fname[0] == '.') continue;

            fn = fno.fname;

            if (fno.fattrib & AM_DIR) {
                sprintf(&path[i], "/%s", fn);
                emit_vfs_node(g_mounted_fs->mount_path, path, VFS_TYPE_DIR);

                res = scan_files(path);
                if (res != FR_OK) break;
                path[i] = 0;
            } else {

              char *m_path = calloc(1, 80);
              if (m_path == NULL) {
                return res;
              }

              sprintf(m_path, "%s/%s", path, fn);
              emit_vfs_node(g_mounted_fs->mount_path, m_path, VFS_TYPE_FILE);
              free(m_path);
            }
        }
    }

    return res;
}

static int open_fs_node(struct opened_resource_s *file, const char *pathname,
  int flags, mode_t mode)
{
  FRESULT fr;
  FIL *fatfs_file;
  int ret = OK;
  volatile BYTE fatfs_mode = FA_READ;
  fatfs_file = malloc(sizeof(FIL));
  if (fatfs_file == NULL) {
    return -ENOMEM;
  }

  /* Extract only the file path without the mount path */

  int mnt_path_len = strlen(g_mounted_fs->mount_path);
  if (mnt_path_len > strlen(pathname)) {
    return -EINVAL;
  }

  pathname += mnt_path_len;

  if (flags & O_APPEND)
    fatfs_mode = FA_OPEN_APPEND | FA_WRITE;
  else if (flags & O_CREATE)
    fatfs_mode = FA_CREATE_NEW | FA_WRITE;
  else if (flags & O_WRONLY)
    fatfs_mode = FA_WRITE;

  fr = f_open(fatfs_file, pathname, fatfs_mode);
  if (fr) {
    ret = -EINVAL;
    goto clean_mem;
  }

  file->vfs_node->priv = fatfs_file;
  return ret;

clean_mem:
  free(fatfs_file);
  return ret;
}

static int read_fs_node(struct opened_resource_s *file, void *buf, size_t count)
{
  FRESULT fr;
  UINT br;

  fr = f_read(file->vfs_node->priv, buf, count, &br);
  if (fr == OK) {
    return br;
  }

  return -EINVAL;
}

static int write_fs_node(struct opened_resource_s *file, const void *buf,
  size_t count)
{
  FRESULT fr;
  UINT br;

  fr = f_write(file->vfs_node->priv, buf, count, &br);
  if (fr == OK) {
    return br;
  }

  return -EINVAL;
}

static int close_fs_node(struct opened_resource_s *file)
{
  if (file->vfs_node->priv) {
    f_close(file->vfs_node->priv);
    free(file->vfs_node->priv);
    file->vfs_node->priv = NULL;
  }

  return OK;
}

/*
 * mount_fs - creates the FATFS structure and scan for files and folders
 *
 * @mount - the mount structure
 *
 *  This function scans for files and folders and creates entries in the
 *  Virtual file system for them. It attaches the corresponding operation
 *  pointers (file_ops).
 */
static int mount_fs(struct vfs_mount_filesystem_s *mount)
{
  int ret = OK;

  /* Check for already in use */

  if (g_fatfs != NULL) {
    return -EINVAL;
  }

  g_fatfs = malloc(sizeof(FATFS));
  if (g_fatfs == NULL) {
    return -ENOMEM;
  }

  g_mtd_ops = mount->mtd_ops;
  FRESULT retfs = f_mount(g_fatfs, "", 1);
  if (retfs != FR_OK) {
    ret = -ENOSYS;
    goto free_with_mem;
  }

  char *mnt_path = (char *)mount->mount_path;
  struct vfs_node_s *node = vfs_get_matching_node(mnt_path, strlen(mnt_path));
  if (node == NULL) {
    ret = -ENOSYS;
    goto free_with_mem;
  }

  char *scan_path = calloc(FATFS_MAX_PATH, sizeof(char));
  if (scan_path == NULL) {
    ret = -ENOMEM;
    goto free_with_mem;
  }
  
  strncpy(scan_path, VFS_PATH_DELIM, 1);
  g_mounted_fs = mount;
  scan_files(scan_path);

  free(scan_path);

  return OK;

free_with_mem:
  free(g_fatfs);
  g_mounted_fs = NULL;
  g_fatfs = NULL;
  return ret;
}

/*
 * umount_fs - unmounts the FATFS structure and remove the files & folders
 *
 * @mount - the mount structure
 *
 *  This function removes the entries from VFS for the specified mount path
 *  and it de-allocates memory used by the FAT fs
 */
static int umount_fs(const char *mount_path)
{
  f_mount(NULL, "", 0);
  free(g_fatfs);
  g_fatfs      = NULL;
  g_mounted_fs = NULL;

  return OK;
}

/****************************************************************************
 * Public GLUE Methods that FatFS links to
 ****************************************************************************/

#if !FF_FS_READONLY && !FF_FS_NORTC
DWORD get_fattime(void)
{
  return 0;
}
#endif

DSTATUS MMC_disk_initialize(void)
{
  return 0;
}

DSTATUS MMC_disk_status(void)
{
  /* Schedule the following operation on the initialization thread */

  /* Verify if the file descriptor is opened if it's opened send an ioctl
   * to the device to verify if the disk is write protected.
   */

  return 0;
}

DRESULT MMC_disk_read(BYTE *buff, DWORD sector, BYTE count)
{
  /* Schedule the following operation on the initialization thread */

  int ret = g_mtd_ops->mtd_read_sec(buff, sector, count * SECTOR_SIZE_BYTES);
  if (ret < 0) {
    return RES_PARERR;
  }

  return RES_OK;
}

DRESULT MMC_disk_write(const BYTE *buff, DWORD sector, BYTE count)
{
  /* Schedule the following operation on the initialization thread */

  int ret = g_mtd_ops->mtd_write_sec(buff, sector, count * SECTOR_SIZE_BYTES);
  if (ret < 0) {
    return RES_PARERR;
  }

  return 0;
}

DRESULT MMC_disk_ioctl(BYTE ctrl, void *buff)
{
  return 0;
}

/****************************************************************************
 * Public Methods
 ****************************************************************************/

/*
 * fatfs_init - Register the FAT file system with the virtual file system
 *
 *  This method will store the FAT filesystem in a list that will allow
 *  mounting at a later time.
 *
 */
int fatfs_filesystem_register(void)
{
  vfs_register_filesystem(FILESYSTEM_TYPE_FAT,
                          &g_fs_ops,
                          mount_fs,
                          umount_fs);

  vfs_register_filesystem(FILESYSTEM_TYPE_EXFAT,
                          &g_fs_ops,
                          mount_fs,
                          umount_fs);
  return OK;
}
