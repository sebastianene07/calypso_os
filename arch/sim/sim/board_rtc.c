/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <board.h>
#include <scheduler.h>
#include <rtc.h>
#include <vfs.h>
#include <errno.h>
#include <string.h>

/****************************************************************************
 * Pre-Processor Definitions
 ****************************************************************************/

/* The VFS registration path */

#define RTC_REGISTER_PATH       "/dev/rtc0"

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static int rtc_open(struct opened_resource_s *priv, const char *pathname,
                    int flags,
                    mode_t mode);

static int rtc_close(struct opened_resource_s *priv);
static int rtc_read(struct opened_resource_s *priv, void *buf, size_t count);
static int rtc_ioctl(struct opened_resource_s *priv, unsigned long request,
                     unsigned long arg);

/****************************************************************************
 * Private Data
 ****************************************************************************/

/* Supported RTC operation */

static struct vfs_ops_s g_rtc_ops = {
  .open  = rtc_open,
  .close = rtc_close,
  .ioctl = rtc_ioctl,
  .read  = rtc_read,
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: rtc_open
 *
 * Description:
 *   This function opens the RTC peripheral.
 *  
 * Input Parameters:
 *   priv     - the private resource
 *   pathname - the device path
 *   flags    - ignored flags
 *   mode     - open mode
 *
 * Return Value:
 *   OK (0) in case of success otherwise a negative error code.
 *
 ****************************************************************************/

static int rtc_open(struct opened_resource_s *priv, const char *pathname,
                    int flags,
                    mode_t mode)
{
  return OK;
}

/****************************************************************************
 * Name: rtc_close
 *
 * Description:
 *   This function closes the RTC peripheral.
 *  
 * Input Parameters:
 *   priv     - the private resource
 *
 * Return Value:
 *   OK (0) in case of success otherwise a negative error code.
 *
 ****************************************************************************/

static int rtc_close(struct opened_resource_s *priv)
{
  return OK;
}

/****************************************************************************
 * Name: rtc_read
 *
 * Description:
 *   This function reads data from the RTC..
 *  
 * Input Parameters:
 *   priv     - the private resource
 *   buf      - the buffer
 *   count    - the number of bytes that buf can hold
 *
 * Return Value:
 *   OK (0) in case of success otherwise a negative error code.
 *
 ****************************************************************************/

static int rtc_read(struct opened_resource_s *priv, void *buf, size_t count)
{
  return OK;
}

/****************************************************************************
 * Name: rtc_ioctl
 *
 * Description:
 *   This function configures the RTC..
 *  
 * Input Parameters:
 *   priv     - the private resource
 *   request  - the request typer
 *   arg      - the received argumentd
 *
 * Return Value:
 *   OK (0) in case of success otherwise a negative error code.
 *
 ****************************************************************************/

static int rtc_ioctl(struct opened_resource_s *priv, unsigned long request,
                     unsigned long arg)
{
  return OK;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: rtc_init
 *
 * Description:
 *   This function registers the RTC peripheral in the VFS.
 *  
 ****************************************************************************/

void rtc_init(void)
{
  const char *name = RTC_REGISTER_PATH;
  vfs_register_node(name, strlen(name), &g_rtc_ops, VFS_TYPE_CHAR_DEVICE,
                    NULL);
}
