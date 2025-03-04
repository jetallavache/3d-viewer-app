#ifndef SRC_3DVIEWER_KERNEL_DATASET_H_
#define SRC_3DVIEWER_KERNEL_DATASET_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

/**
 * @typedef
 * @brief Структура, содержащая массив вершин одной грани
 */
typedef struct {
  int *vrts;
} Face;

/**
 * @typedef
 * @brief Структура, содержащая информацию о центре оси
 */
typedef struct {
  double x;
  double y;
  double z;
} Center;

/**
 * @typedef
 * @brief Структура, содержащая минимальные и максимальные значения,
 * соответсвующие границам объекта
 */
typedef struct {
  double x_min;
  double x_max;
  double y_min;
  double y_max;
  double z_min;
  double z_max;
} Axis;

/**
 * Сокращенные обозначения переменных:
 *
 *   coordinate - координата (crd) | coordinates - координаты (crds)
 *   vertex - вершина (vrt)        | vertexes - вершины (vrts)
 *   face - грань (fc)             | faces - грани (fcs)
 *   poligon - многоугольник (plg) | poligons - многоугольники (plgs)
 */

/**
 * @typedef
 * @brief Структура, содержащая информацию о 3D-объекте
 */
typedef struct {
  int vrts_COUNT; /* Количество вершин v(x,y,z) */
  int fcs_COUNT;  /* Количество граней f(v1,v2,v3) */

  int vrt_MAX_INDEX; /* Максимальный индекс вершины в общем массиве
                       вершин всех граней (f1, f2 и тд) */

  int *num_vrts_forming_fc_ARR; /* Массив количества вершин каждой конкретной
                                    грани (f1, f2 и тд),
                                    т.е. количество вершин грани f1,
                                    количество вершин грани f2 и тд.*/

  int all_vrts_forming_fc_COUNT; /* Общее количество всех вершин,
                                     образующих каждую конкретную грань f1, f2,
                                    ... */

  double *crds_ARR; /* Массив, образованный последовательностью
                        координат [x11, x12, x13, x21, x22, x23, ...]
                        каджой заданной вершины (v1, v2, ...) в списке */

  Face *fcs_ARR; /* Массив, образованный последовательностью указателей
                     на массивы вершин каждой грани из списка */

  Center center_COORD; /* Координаты центра 3D-объекта */

  Axis axis_COORD; /* Определение областей на координатных осях x, y и z */

} WireframeModel;

/**
 * @brief Инициализирует структуру данных на основе .obj файла
 * @warning Не забудьте закрыть файл и освободить выделенную память
 * @param file Указатель на открытый файл
 * @param model Указатель на структуру #WireframeModel
 * @return status #Status OK/FAIL
 */
Status kernel_dataset_file_to_object(FILE *file, WireframeModel *model);

/**
 * @brief Освобождает память, выделенную под соответсвующие структуры данных
 * внутри #WireframeModel
 * @param model Указатель на указатель структуры #WireframeModel
 */
void kernel_dataset_free(WireframeModel **model);

/**
 * @brief Определяет и задает координаты центра 3D-объекта
 * @param model Указатель на структуру #WireframeModel
 */
void kernel_dataset_define_center(WireframeModel *model);

#endif /* SRC_3DVIEWER_KERNEL_DATASET_H_ */
