#ifndef __FAT_FS
#define __FAT_FS

#include <board.h>

#include <errno.h>
#include <filesystems.h>
#include <stdio.h>
#include <string.h>
#include <vfs.h>
#include <unistd.h>

typedef struct fatfs_priv_s {
  struct vfs_mount_filesystem_s *mount;
} fatfs_priv_t;

typedef struct fatfs_mbr_partentry_s
{
  uint8_t partition_state;
  uint8_t begining_partition_head;
  uint16_t begining_partition_cylinder;
  uint8_t partition_type;
  uint8_t end_partition_head;
  uint16_t end_partition_cylinder;
  uint32_t num_reserved_sectors;
  uint32_t num_sectors_in_partition;
} __attribute__((packed)) fatfs_mbr_partentry_t;

typedef struct fatfs_mbr_parttable_s
{
  fatfs_mbr_partentry_t entry[4];
  uint16_t mbr_boot_signature_low;
  uint16_t mbr_boot_signature_high;
} __attribute__((packed)) fatfs_mbr_parttable_t;

#endif /* __FAT_FS */
