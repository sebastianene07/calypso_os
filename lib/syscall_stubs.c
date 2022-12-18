#include <stdint.h>
#include <sys/types.h>
#include <semaphore.h>
#include <dirent.h>

struct stat
{
  /* Required, standard fields */

  mode_t    st_mode;    /* File type, atributes, and access mode bits */
  off_t     st_size;    /* Size of file/directory, in bytes */
  blksize_t st_blksize; /* Blocksize used for filesystem I/O */
  blkcnt_t  st_blocks;  /* Number of blocks allocated */
  time_t    st_atime;   /* Time of last access */
  time_t    st_mtime;   /* Time of last modification */
  time_t    st_ctime;   /* Time of last status change */

  /* Internal fields.  These are part this specific implementation and
   * should not referenced by application code for portability reasons.
   */

  uint8_t   st_count;   /* Used internally to limit traversal of links */
};

void _exit(int const status)
{

}

int sched_create_task(int (*task_entry_point)(int argc, char **argv),
	uint32_t stack_size, int argc, char **argv, const char *task_name)
{
	return 0;
}

int sem_init(sem_t *sem, int pshared, unsigned int value)
{
	return 0;
}

int sem_wait(sem_t *sem)
{
	return 0;
}

int sem_timedwait(sem_t *sem, int timeout_abs)
{
	return 0;
}

int sem_trywait(sem_t *sem)
{
	return 0;
}

int sem_post(sem_t *sem)
{
	return 0;
}

DIR *opendir(const char *name)
{
	return NULL;
}

int closedir(DIR *dirp)
{
	return 0;
}

int usleep(useconds_t microseconds)
{
	return 0;
}

int mkdir(const char *pathname, mode_t mode)
{
	return 0;
}

int _write(int file, char *ptr, int len)
{
	return 0;
}

int _read(int file, char* ptr, int len)
{
	return 0;
}

int _open(const char* path, int flags, ...)
{
    return 0;
}

/* fd, is a user file descriptor. */
int _close(int fd)
{
	return 0;
}

void *_sbrk(int incr)
{
	return NULL;
}

int _isatty(int fd)
{
	return 0;
}

int _fstat (int fd, struct stat* st)
{
	return 0;
}

int _stat(const char*fname, struct stat *st)
{
	return 0;
}

int _kill(int pid, int sig)
{
	return 0;
}

int _link(char* existing, char* _new)
{
	return 0;
}

int _unlink(char* name)
{
    return 0;
}

int _lseek(int file, int ptr, int dir)
{
	return 0;
}
