#ifndef SRC_3DVIEWER_KERNEL_UTILS_H_
#define SRC_3DVIEWER_KERNEL_UTILS_H_

#include <stdio.h>

/**
 * @enum
 * @brief Текущее состояние
 * @param OK
 * @param FAIL
 * @param INCONSIST
 * @param MISSING_V
 * @param MISSING_F
 */
typedef enum { OK, FAIL, INCONSIST, MISSING_V, MISSING_F } Status;

/**
 * @brief Открывает файл для чтения
 * @warning Не забудьте закрыть файл
 * @param name Имя файла
 * @param file Указатель на открытый файл
 * @return status #Status OK/FAIL
 */
Status kernel_utils_open_file(FILE **file, char *name);

/**
 * @brief Закрывает файл
 * @param file Указатель на открытый файл
 */
void kernel_utils_close_file(FILE **file);

#endif /* SRC_3DVIEWER_KERNEL_UTILS_H_ */
