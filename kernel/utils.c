#include "utils.h"

Status kernel_utils_open_file(FILE **file, char *name) {
  Status status = OK;

  *file = fopen(name, "r");

  if (!*file) {
    fprintf(stderr, "Файл \'%s\' не найден или поврежден.\n", name);
    status = FAIL;
  } else {
    printf("Файл \'%s\' открыт.\n", name);
  }

  return status;
}

void kernel_utils_close_file(FILE **file) {
  if (*file) {
    fclose(*file);
    fprintf(stderr, "Файловый поток закрыт.\n");
  } else {
    fprintf(stderr, "Ошибка закрытия файлового потока.\n");
  }
}