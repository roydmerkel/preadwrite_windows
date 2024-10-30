#include <stdio.h>
#include <windows.h>
#include <io.h>

ssize_t pwrite (int fd, const void *buf, size_t len, off_t off)
{
  long long filePos = _lseeki64 (fd, 0, SEEK_CUR);
  long long newFilePos = _lseeki64 (fd, off, SEEK_SET);
  int writeResult = -1;
  if(newFilePos == -1L)
  {
         return -1L;
  }

  writeResult = _write(fd, buf, len);
  _lseeki64 (fd, filePos, SEEK_SET);

  return writeResult;
}

ssize_t pread (int fd, void *buf, size_t len, off_t off)
{
  long long filePos = _lseeki64 (fd, 0, SEEK_CUR);
  long long newFilePos = _lseeki64 (fd, off, SEEK_SET);
  int readResult = -1;
  if(newFilePos == -1L)
  {
         return -1L;
  }

  readResult = _read(fd, buf, len);
  _lseeki64 (fd, filePos, SEEK_SET);

  return readResult;
}

