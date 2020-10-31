#ifndef __MTD_H
#define __MTD_H

#include <board.h>

/****************************************************************************
 * Preprocessor Definitons
 ****************************************************************************/

/* The MTD group IOCTL */

#define MTD_GROUP                   (0x100)

/* The device that supports MTD operations should implement this IOCTL to
 * retrieve the mtd ops structure.
 */

#define MTD_GET_OPS                 (0x00 + (MTD_GROUP))

/****************************************************************************
 * Public Types
 ****************************************************************************/

typedef int (* mtd_read_sector)(uint8_t *buffer, uint32_t sector, size_t count);
typedef int (* mtd_write_sector)(const uint8_t *buffer, uint32_t sector,
                                 size_t count);

/* This structure contains the function pointers to access the MTD device */

struct mtd_ops_s {
  mtd_read_sector mtd_read_sector_cb;
  mtd_write_sector mtd_write_sector_cb;
};

#endif /* __MTD_H */
