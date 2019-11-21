#include <vfs.h>
#include <semaphore.h>
#include <errno.h>
#include <list.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

/* Virtual file system tree */

static struct vfs_node_s g_root_vfs;

/* VFS mutual exclusion sema */

sem_t g_vfs_sema;

/* VFS default mountpoints */

static struct vfs_init_mountpoint_s g_vfs_default_mtpt = {
  .node_name = {"dev", "mnt", "bin", "otp", "home"},
  .num_nodes = 5,
};

static const char *delim = "/";

/*
 * vfs_init - initialize the root nodes
 *
 * @node_name - pointer to an array of names
 * @num_nodes - the length of the array
 *
 *  This function initializes the virtual file system
 *  and creates the initial nodes. If NULL is specified
 *  the function will use the default mountpoints.
 *
 */
int vfs_init(const char *node_name[], size_t num_nodes)
{
  /* Verify is we use the default mount points */

  if (node_name == NULL)
  {
    struct vfs_init_mountpoint_s *default_mtpt = vfs_get_default();
    node_name = default_mtpt->node_name;
    num_nodes = default_mtpt->num_nodes;
  }

  sem_init(&g_vfs_sema, 0, 1);

  g_root_vfs.parent = NULL;

  g_root_vfs.name       = "/";
  g_root_vfs.node_type  = VFS_TYPE_DIR;

  /* Create the VFS child nodes */

  struct vfs_node_s *new_node;

  new_node = calloc(num_nodes, sizeof(struct vfs_node_s));
  if (new_node == NULL)
  {
    return -ENOMEM;
  }

  g_root_vfs.child         = new_node;
  g_root_vfs.num_children  = num_nodes;

  for (int i = 0; i < num_nodes; ++i)
  {
    new_node[i].parent      = &g_root_vfs;
    new_node[i].name        = node_name[i];
    new_node[i].node_type   = VFS_TYPE_DIR;
  }

  return OK;
}

/*
 * vfs_get_default - get the paths mounted under "/"
 *
 *  The function returns the mounted paths under the root directory.
 *
 */
struct vfs_init_mountpoint_s *vfs_get_default(void)
{
  return &g_vfs_default_mtpt;
}

/*
 * vfs_get_matching_node - get the node by specified path
 *
 * @name      - path name
 * @name_len  - the name length
 *
 *  The function returns the VFS node from the specified name.
 *
 */
struct vfs_node_s *vfs_get_matching_node(const char *name, size_t name_len)
{
  char *olds;

  /* We should do a copy of the path to prevent the string from being
   * changed.
   */

  char *name_copy = calloc(1, strlen(name) + 1);
  memcpy(name_copy, name, strlen(name));

  /* TODO : implement reader-writer with readers priority */
  /* Prevent concurent access to VFS while we iterate through nodes */

  sem_wait(&g_vfs_sema);

  char *ptr_copy = name_copy;
  struct vfs_node_s *current_node = NULL;
  struct vfs_node_s *parent = &g_root_vfs;
  char *node_name = NULL;
  bool not_found;

  if (name_len == 1 && strcmp(name, "/") == 0) {
    current_node = parent;
    goto free_with_sem;
  }

  do {
    node_name = strtok_r(ptr_copy, delim, &olds);
    ptr_copy = NULL;

    if (node_name && strlen(node_name) == 0)
      continue;
    else if (node_name == NULL)
      break;


    /* Look at the names of the child nodes and verify which one has the name
     * 'node_name'.
     */

    not_found = true;
    for (int i = 0; i < parent->num_children; ++i) {
      current_node = &parent->child[i];

      if (node_name && !strcmp(current_node->name, node_name)) {
        not_found = false;
        break;
      }
    }

    if (not_found) {
      current_node = NULL;
      goto free_with_sem;
    }

    parent = current_node;
  } while (node_name != NULL);

free_with_sem:
  sem_post(&g_vfs_sema);
  free(name_copy);

  return current_node;
}

/*
 * vfs_register_node - register a node with the virual file system
 *
 * @name      - path name
 * @name_len  - the name length
 * @ops       - supported node operations
 * @node_type - the type of the node
 *
 *  The function creates a new node entry in the virtual file system.
 *  Returns OK if the node is registered -ENOMEM if we run out of memory and
 *  we can't register the node, -EINVAL if the given arguments are wrong.
 */
int vfs_register_node(const char *name,
                      size_t name_len,
                      struct vfs_ops_s *ops,
                      enum vfs_node_type node_type,
                      void *priv)
{
  /* Extract the name */

  bool is_delim_found = false;
  int i;
  for (i = name_len; i > 0; i--) {
    if (*(name + i) == '/') {
      is_delim_found = true;
      break;
    }
  }

  if (!is_delim_found) {
    /* Bad path name */
    return -EINVAL;
  }

  /* We found the deimiter now we can extract the name */

  char *node_name = (char *)(name + i + 1);
  char *node_name_copy = node_name;

  /* Find the place where we should insert the node */

  char *name_copy = calloc(1, i);
  memcpy(name_copy, name, i);

  sem_wait(&g_vfs_sema);

  char *ptr_copy = name_copy;
  bool not_found = false;
  struct vfs_node_s *current_node = NULL;
  struct vfs_node_s *parent = &g_root_vfs;
  char *olds;

  do {
    node_name = strtok_r(ptr_copy, delim, &olds);
    ptr_copy = NULL;

    if (node_name && strlen(node_name) == 0)
      continue;
    else if (node_name == NULL)
      break;

    not_found = true;
    for (int i = 0; i < parent->num_children; ++i) {
      current_node = &parent->child[i];

      if (node_name && !strcmp(current_node->name, node_name)) {
        not_found = false;
        break;
      }
    }

    if (not_found) {
      current_node = NULL;
      goto free_with_sem;
    }

    parent = current_node;
  } while (node_name != NULL);

free_with_sem:
  sem_post(&g_vfs_sema);
  free(name_copy);

  if (!current_node) {
    return -ENOENT;
  }
  struct vfs_node_s *new_child = realloc(current_node->child,
    (current_node->num_children + 1) * sizeof(struct vfs_node_s));
  if (new_child == NULL) {
    return -ENOMEM;
  }

  struct vfs_node_s *new_node = &new_child[current_node->num_children];

  /* Create and populate the new node */

  new_node->parent       = current_node;
  new_node->num_children = 0;
  new_node->ops          = ops;
  new_node->node_type    = node_type;
  new_node->priv         = priv;
  new_node->name         = node_name_copy;
  new_node->child        = NULL;

  current_node->child = new_child;
  current_node->num_children++;

  return OK;
}
