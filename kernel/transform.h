#ifndef SRC_3DVIEWER_KERNEL_TRANSFORM_H_
#define SRC_3DVIEWER_KERNEL_TRANSFORM_H_

#include <math.h>

#include "dataset.h"
#include "utils.h"

/**
 * @brief Задает новые координаты вершин объекта,
 * соответсвующие смещенному по оси x объекту на заданное значение
 * @param model Указатель на структуру #WireframeModel
 * @param value Значение смещения по оси x
 */
void kernel_transform_move_x(WireframeModel *model, double value);

/**
 * @brief Задает новые координаты вершин объекта,
 * соответсвующие смещенному по оси y объекту на заданное значение
 * @param model Указатель на структуру #WireframeModel
 * @param value Значение смещения по оси y
 */
void kernel_transform_move_y(WireframeModel *model, double value);

/**
 * @brief Задает новые координаты вершин объекта,
 * соответсвующие смещенному по оси z объекту на заданное значение
 * @param model Указатель на структуру #WireframeModel
 * @param value Значение смещения по оси z
 */
void kernel_transform_move_z(WireframeModel *model, double value);

/**
 * @brief Задает новые координаты вершин объекта,
 * соответсвующие увеличенному/уменьшенному объекту на заданное значение
 * @param model Указатель на структуру #WireframeModel
 * @param value Значение, во сколько раз изменится размер объекта
 */
void kernel_transform_rescale(WireframeModel *model, double value);

/**
 * @brief Задает новые координаты вершин объекта,
 * соответсвующие повернутому по оси x объекту на заданный угол
 * @param model Указатель на структуру #WireframeModel
 * @param value Значение угла
 */
void kernel_transform_rotate_x(WireframeModel *model, double value);

/**
 * @brief Задает новые координаты вершин объекта,
 * соответсвующие повернутому по оси y объекту на заданный угол
 * @param model Указатель на структуру #WireframeModel
 * @param value Значение угла
 */
void kernel_transform_rotate_y(WireframeModel *model, double value);

/**
 * @brief Задает новые координаты вершин объекта,
 * соответсвующие повернутому по оси z объекту на заданный угол
 * @param model Указатель на структуру #WireframeModel
 * @param value Значение угла
 */
void kernel_transform_rotate_z(WireframeModel *model, double value);

#endif /* SRC_3DVIEWER_KERNEL_TRANSFORM_H_ */