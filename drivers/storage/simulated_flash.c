#include <board.h>
#include <errno.h>
#include <mtd.h>
#include <semaphore.h>
#include <stdio.h>
#include <string.h>
#include <vfs.h>

#include "simulated_flash.h"

/****************************************************************************
 * Public Definitionse
 ****************************************************************************/

/* These functions are linked with the host symbols */

int host_sim_flash_read_mtd(uint8_t *buffer, uint32_t sector, size_t count);
int host_sim_flash_write_mtd(uint8_t *buffer, uint32_t sector, size_t count);

/****************************************************************************
 * Private Data Types
 ****************************************************************************/

/* The simulated flash private data */

typedef struct sim_flash_priv_s {
  sem_t lock;
  int opened_count;
} sim_flash_priv_t;

/****************************************************************************
 * Private Function Definitions
 ****************************************************************************/

/* The MTD block access functions */

static int sim_flash_mtd_read_block(uint8_t *buffer, uint32_t sector,
                                    size_t count);
static int sim_flash_mtd_write_block(const uint8_t *buffer, uint32_t sector,
                                     size_t count);

/* The virtual file system operations for this device */

static int sim_flash_open(struct opened_resource_s *priv, const char *pathname,
                          int flags,
                          mode_t mode);

static int sim_flash_close(struct opened_resource_s *priv);

static int sim_flash_ioctl(struct opened_resource_s *prov, unsigned long request,
                           unsigned long arg);

/****************************************************************************
 * Private Data
 ****************************************************************************/

/* The simulated flash is an MTD block device and supports these functions */

static struct mtd_ops_s g_sim_flash_mtd_ops = {
  .mtd_read_sec  = sim_flash_mtd_read_block,
  .mtd_write_sec = sim_flash_mtd_write_block,
};

/* The virtual file system ops */

static struct vfs_ops_s g_sim_flash_ops = {
  .open   = sim_flash_open,
  .close  = sim_flash_close,
  .ioctl  = sim_flash_ioctl,
};

/* The private data that is stored in the VFS node */

static sim_flash_priv_t g_sim_private_data;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/*
 * sim_flash_open - Open the simulated flash device.
 *
 * @priv      - the private data kept in the VFS nodee
 * @pathname  - the full path
 * @flags     - not used
 * @mode      - not used
 *
 * This function should handle the device initialization part for the
 * simulated flash memory..
 *
 */
static int sim_flash_open(struct opened_resource_s *priv, const char *pathname,
                          int flags,
                          mode_t mode)
{
  return OK;
}

/*
 * sim_flash_close - Close the simulated flash device.
 *
 * @priv - the private data kept in the VFS node
 *
 * Release the resources for the simulated flash device.
 *
 */
static int sim_flash_close(struct opened_resource_s *priv)
{
  return OK;
}

/*
 * sim_flash_ioctl - IOCTL for the simulated flash device.
 *
 * @priv    - the private data kept in the VFS node
 * @request - the IOCTL request type
 * @arg     - the IOCTL data argument
 *
 */
static int sim_flash_ioctl(struct opened_resource_s *priv,
                           unsigned long request,
                           unsigned long arg)
{
  int ret = -EINVAL;

  if (arg == NULL) {
    return ret;
  }

  switch (request) {
    case MTD_GET_OPS:
      {
        struct mtd_ops_s **mtd_ops = (struct mtd_ops_s **)arg;
        *mtd_ops = &g_sim_flash_mtd_ops;
        ret = OK;
      }
      break;

    default:
      break;
  }

  return ret;
}

/*
 * sim_flash_mtd_read_block - Read blocks from the simulated flash.
 *
 * @buffer    - the place where we store the read datae
 * @sector    - the requested sector number
 * @count     - the size expected to be read
 *
 */
static int sim_flash_mtd_read_block(uint8_t *buffer, uint32_t sector,
                                    size_t count)
{
  return host_sim_flash_read_mtd(buffer, sector, count);
}

/*
 * sim_flash_mtd_write_block - Write blocks from the simulated flash.
 *
 * @buffer    - the data that we want to writee
 * @sector    - the requested sector number
 * @count     - the size of the write data
 *
 */
static int sim_flash_mtd_write_block(const uint8_t *buffer, uint32_t sector,
                                     size_t count)
{
  return host_sim_flash_write_mtd(buffer, sector, count);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/*
 * sim_flash_init - Initialize and register the simulated flash node.
 *
 *  This function initializes the simulated flash and mounts the node
 *  in the virtual file system.
 */
int sim_flash_init(void)
{
  sem_init(&g_sim_private_data.lock, 0, 0);

  return vfs_register_node(CONFIG_SIM_FLASH_NAME,
                           strlen(CONFIG_SIM_FLASH_NAME),
                           &g_sim_flash_ops,
                           VFS_TYPE_BLOCK_DEVICE,
                           &g_sim_private_data);
}
