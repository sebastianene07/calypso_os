#include <dirent.h>
#include <semaphore.h>
#include <string.h>

/*
 * opendir - open a directory entry
 *
 * @name - the path to a file/device
 *
 * Return a pointer to the DIR entry.
 */
DIR *opendir(const char *name)
{
  return vfs_get_matching_node(name, strlen(name));
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
  return 0;
}
