#include <board.h>
#include <console_main.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <semaphore.h>
#include <dirent.h>
#include <list.h>

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int console_ls(int argc, const char *argv[])
{
  const char *pathname = "/";
  int i;

  if (argc == 2) {
    pathname = argv[1];
  } else if (argc != 1) {
    printf("Bad input: ls <pathname>\n");
    return -ENOENT;
  }

  DIR *dirent = opendir(pathname);
  if (dirent == NULL) {
    printf("No such file or directory %s\n", pathname);
    return -ENOENT;
  }

  if (dirent->node_type != VFS_TYPE_DIR) {
    printf("%s is not a directory, type: %d\n", pathname, dirent->node_type);
    closedir(dirent);
    return -EINVAL;
  }

  const char *fmt_1 = "%s%s";
  const char *fmt_2 = "%s/%s";
  const char *fmt;
  int len = strlen(pathname);
  if (pathname[len - 1] == '/') {
    fmt = fmt_1;
  } else {
    fmt = fmt_2;
  }

  if (dirent->num_children == 0) {
    printf("%s is empty\n", pathname);
  }

  printf("\r\n");
  struct vfs_node_s *node = NULL;
  list_for_each_entry(node, &dirent->child, node_child) {

    char print_line[80] = {0};
    snprintf(print_line, 80, fmt, pathname, node->name);
    int line_len = strlen(print_line);
    for (i = line_len; i < 30; i++)
      print_line[i] = ' ';

    printf("%s %c\n", print_line, node->node_type == VFS_TYPE_DIR ? 'd' : 'f');
  }

  return closedir(dirent);
}
