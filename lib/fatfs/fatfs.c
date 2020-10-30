#include "fatfs.h"

#define FATFS_MBR_LENGTH                (512U)
#define FATFS_MBR_SIGNATURE_LOW         (0x55)
#define FATFS_MBR_SIGNATURE_HIGH        (0xAA)
#define FATFS_MBR_LBA_SECTOR            (0x0)
#define FATFS_MBR_PART_TABLE_OFFSET     (446U)

/*
 * fatfs_read_mbr - read the master boot record get the signature and the
 *                  partition table. 
 *
 * @fat_fs - place where we store the mount structure
 *
 */
static int fatfs_read_mbr(fatfs_priv_t * const fat_fs)
{
  struct mtd_ops_s *mtd_ops = fat_fs->mount->mtd_ops;
  int ret;
  uint8_t mbr_buffer[FATFS_MBR_LENGTH];

  ret = mtd_ops->mtd_read_sector_cb(&mbr_buffer[0],
                                    FATFS_MBR_LBA_SECTOR,
                                    sizeof(mbr_buffer));
  if (ret < 0)
  {
    return ret;
  }

  fatfs_mbr_parttable_t *partition_table =
    &mbr_buffer[FATFS_MBR_PART_TABLE_OFFSET]; 

  if (partition_table->mbr_boot_signature_low != FATFS_MBR_SIGNATURE_LOW ||
      partition_table->mbr_boot_signature_high != FATFS_MBR_SIGNATURE_HIGH)
  {
    return -1;
  }
}

/*
 * fatfs_init - initialize the file system 
 *
 * @fat_fs - place where we store the mount structure
 * @mount  - container that keeps the MTD operation pointer and the place
 *           where we mount the file system.@
 *
 */
int fatfs_init(fatfs_priv_t *fat_fs,
               const struct vfs_mount_filesystem_s *mount)
{
  int ret;

  fat_fs->mount = mount;

  /* Read the MBR and get the partition table */

  ret = fatfs_read_mbr(fat_fs);
  return ret;
}
