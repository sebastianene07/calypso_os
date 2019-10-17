#include <dirent.h>
#include <semaphore.h>
#include <string.h>

extern sem_t g_vfs_sema;

/*
 * opendir - open a directory entry
 *
 * @name - the path to a file/device
 *
 * Return a pointer to the DIR entry.
 */
DIR *opendir(const char *name)
{
  size_t name_len = strlen(name);
  struct vfs_node_s *node = vfs_get_matching_node(name,
                                                  name_len);
  if (node != NULL) {
    sem_wait(&g_vfs_sema);
  }

  return node;
}

/*
 * closedir - closes a directory entry
 *
 * @dirp - the directory that will close
 *
 * Limitation:
 *
 * If a directory is opened the application should call this function
 * after all the job is done otherwise no other app will access the VFS.
 */
int closedir(DIR *dirp)
{
  if (dirp != NULL) {
    sem_post(&g_vfs_sema);
  }

  return 0;
}
