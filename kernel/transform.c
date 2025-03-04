#include "transform.h"

#include <stdio.h>

/* private */

double _transform_rescale_find_max_axis_distance(WireframeModel *model);
void _transform_rescale_apply(WireframeModel *model, double value);

void kernel_transform_move_x(WireframeModel *model, double value) {
  model->axis_COORD.x_max += value;
  model->axis_COORD.x_min += value;

  for (int i = 0; i < model->vrts_COUNT * 3; i += 3) {
    model->crds_ARR[i] += value;
  }
}

void kernel_transform_move_y(WireframeModel *model, double value) {
  model->axis_COORD.y_max += value;
  model->axis_COORD.y_min += value;

  for (int i = 0; i < model->vrts_COUNT * 3; i += 3) {
    model->crds_ARR[i + 1] += value;
  }
}

void kernel_transform_move_z(WireframeModel *model, double value) {
  model->axis_COORD.z_max += value;
  model->axis_COORD.z_min += value;

  for (int i = 0; i < model->vrts_COUNT * 3; i += 3) {
    model->crds_ARR[i + 2] += value;
  }
}

double _transform_rescale_find_max_axis_distance(WireframeModel *model) {
  double result = 0.0;

  double distance_x = model->axis_COORD.x_max - model->axis_COORD.x_min;
  double distance_y = model->axis_COORD.y_max - model->axis_COORD.y_min;
  double distance_z = model->axis_COORD.z_max - model->axis_COORD.z_min;

  if (distance_x > distance_y) {
    if (distance_x > distance_z) {
      result = distance_x;
    } else {
      result = distance_z;
    }
  } else {
    if (distance_y > distance_z) {
      result = distance_y;
    } else {
      result = distance_z;
    }
  }

  return result;
}

void _transform_rescale_apply(WireframeModel *model, double value) {
  model->axis_COORD.x_max *= value;
  model->axis_COORD.x_min *= value;
  model->axis_COORD.y_max *= value;
  model->axis_COORD.y_min *= value;
  model->axis_COORD.z_max *= value;
  model->axis_COORD.z_min *= value;

  for (int i = 0; i < model->vrts_COUNT * 3; i += 3) {
    model->crds_ARR[i] *= value;
    model->crds_ARR[i + 1] *= value;
    model->crds_ARR[i + 2] *= value;
  }
}

void kernel_transform_rescale(WireframeModel *model, double value) {
  double scale = (value - (value * (-1))) /
                 _transform_rescale_find_max_axis_distance(model);
  _transform_rescale_apply(model, scale);
}

void kernel_transform_rotate_x(WireframeModel *model, double value) {
  double sin_x = sin(value);
  double cos_x = cos(value);

  double _y_max = model->axis_COORD.y_max;
  double _y_min = model->axis_COORD.y_min;
  double _z_max = model->axis_COORD.z_max;
  double _z_min = model->axis_COORD.z_min;

  model->axis_COORD.y_max = cos_x * _y_max - sin_x * _z_max;
  model->axis_COORD.y_min = cos_x * _y_min - sin_x * _z_min;
  model->axis_COORD.z_max = sin_x * _y_max + cos_x * _z_max;
  model->axis_COORD.z_min = sin_x * _y_min + cos_x * _z_min;

  for (int i = 0; i < model->vrts_COUNT * 3; i += 3) {
    double _y = model->crds_ARR[i + 1];
    double _z = model->crds_ARR[i + 2];
    model->crds_ARR[i + 1] = cos_x * _y - sin_x * _z;
    model->crds_ARR[i + 2] = sin_x * _y + cos_x * _z;
  }
}

void kernel_transform_rotate_y(WireframeModel *model, double value) {
  double sin_y = sin(value);
  double cos_y = cos(value);

  double _x_max = model->axis_COORD.x_max;
  double _x_min = model->axis_COORD.x_min;
  double _z_max = model->axis_COORD.z_max;
  double _z_min = model->axis_COORD.z_min;

  model->axis_COORD.x_max = cos_y * _x_max + sin_y * _z_max;
  model->axis_COORD.x_min = cos_y * _x_min + sin_y * _z_min;
  model->axis_COORD.z_max = sin_y * (-_x_max) + cos_y * _z_max;
  model->axis_COORD.z_min = sin_y * (-_x_min) + cos_y * _z_min;

  for (int i = 0; i < model->vrts_COUNT * 3; i += 3) {
    double _x = model->crds_ARR[i + 0];
    double _z = model->crds_ARR[i + 2];
    model->crds_ARR[i + 0] = cos_y * _x + sin_y * _z;
    model->crds_ARR[i + 2] = sin_y * (-_x) + cos_y * _z;
  }
}

void kernel_transform_rotate_z(WireframeModel *model, double value) {
  double sin_z = sin(value);
  double cos_z = cos(value);

  double _x_max = model->axis_COORD.x_max;
  double _x_min = model->axis_COORD.x_min;
  double _y_max = model->axis_COORD.y_max;
  double _y_min = model->axis_COORD.y_min;

  model->axis_COORD.x_max = cos_z * _x_max - sin_z * _y_max;
  model->axis_COORD.x_min = cos_z * _x_min - sin_z * _y_min;
  model->axis_COORD.y_max = sin_z * _x_max + cos_z * _y_max;
  model->axis_COORD.y_min = sin_z * _x_min + cos_z * _y_min;

  for (int i = 0; i < model->vrts_COUNT * 3; i += 3) {
    double _x = model->crds_ARR[i + 0];
    double _y = model->crds_ARR[i + 1];
    model->crds_ARR[i + 0] = cos_z * _x - sin_z * _y;
    model->crds_ARR[i + 1] = sin_z * _x + cos_z * _y;
  }
}