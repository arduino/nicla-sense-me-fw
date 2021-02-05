#ifndef FILE_UTILS_H_
#define FILE_UTILS_H_

#include "mbed.h"

#include "common.h"

class FileUtils
{
  public:

  FileUtils();

  long getFileLen(FILE *file);
  char getFileCRC(FILE *file);
  char computeCRC(FILE *file);

  private:

};

#endif //FILE_UTILS_H_
