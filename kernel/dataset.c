#include "dataset.h"

/* private */

void _dataset_init(WireframeModel *model);
Status _dataset_allocate_memory(WireframeModel *model);
void _dataset_max_min(double *max, double *min, double value);
void _dataset_set_max_min_values(WireframeModel *model, double x, double y,
                                 double z, int first_line);
int _dataset_parsing_vertexes(WireframeModel *model, char *buffer, int index);
Status _dataset_read_faces(WireframeModel *model, char *buffer, int index);
Status _dataset_read_file(FILE *file, WireframeModel *model);
double _dataset_calculate_center(double *min, double *max);

void _dataset_init(WireframeModel *model) {
  model->vrts_COUNT = 0;
  model->fcs_COUNT = 0;
  model->vrt_MAX_INDEX = 0;
  model->num_vrts_forming_fc_ARR = NULL;
  model->all_vrts_forming_fc_COUNT = 0;
  model->crds_ARR = NULL;
  model->fcs_ARR = NULL;
  model->center_COORD.x = 0;
  model->center_COORD.y = 0;
  model->center_COORD.z = 0;
  model->axis_COORD.x_max = 0;
  model->axis_COORD.x_min = 0;
  model->axis_COORD.y_max = 0;
  model->axis_COORD.y_min = 0;
  model->axis_COORD.z_max = 0;
  model->axis_COORD.z_min = 0;
}

void kernel_dataset_free(WireframeModel **model) {
  if ((*model)->fcs_ARR) {
    for (int i = 0; i < (*model)->fcs_COUNT; ++i) {
      if ((*model)->fcs_ARR[i].vrts) free((*model)->fcs_ARR[i].vrts);
    }
  }

  if ((*model)->crds_ARR) {
    free((*model)->crds_ARR);
    (*model)->crds_ARR = NULL;
  }

  if ((*model)->num_vrts_forming_fc_ARR) {
    free((*model)->num_vrts_forming_fc_ARR);
    (*model)->num_vrts_forming_fc_ARR = NULL;
  }

  if ((*model)->fcs_ARR) {
    free((*model)->fcs_ARR);
    (*model)->fcs_ARR = NULL;
  }

  if (*model) {
    free((*model));
    (*model) = NULL;
  }

  printf("Блок памяти WireframeModel освобожден.\n");
}

Status _dataset_allocate_memory(WireframeModel *model) {
  Status status;

  model->crds_ARR = (double *)malloc((model->vrts_COUNT * 3) * sizeof(double));

  if (model->crds_ARR)
    model->num_vrts_forming_fc_ARR =
        (int *)malloc(model->fcs_COUNT * sizeof(int));

  if (model->num_vrts_forming_fc_ARR)
    model->fcs_ARR = (Face *)malloc(model->fcs_COUNT * sizeof(Face));

  status = (model->fcs_ARR) ? OK : FAIL;

  if (status) kernel_dataset_free(&model);

  return status;
}

void _dataset_max_min(double *max, double *min, double value) {
  if (*max < value) *max = value;

  if (*min > value) *min = value;
}

void _dataset_set_max_min_values(WireframeModel *model, double x, double y,
                                 double z, int first_line) {
  if (first_line) {
    _dataset_max_min(&model->axis_COORD.x_min, &model->axis_COORD.x_max, x);
    _dataset_max_min(&model->axis_COORD.y_min, &model->axis_COORD.y_max, y);
    _dataset_max_min(&model->axis_COORD.z_min, &model->axis_COORD.z_max, z);
  } else {
    model->axis_COORD.x_max = x;
    model->axis_COORD.x_min = x;
    model->axis_COORD.y_max = y;
    model->axis_COORD.y_min = y;
    model->axis_COORD.z_max = z;
    model->axis_COORD.z_min = z;
  }
}
int _dataset_count_vertexes_of_face(char *buffer) {
  char str[BUFSIZ] = {0};
  int count = 0;
  buffer += 2;

  while (*buffer && sscanf(buffer, "%s", str)) {
    int length = 0;
    length = strlen(str) + 1;
    buffer += length;

    if (length > 1) ++count;

    strcpy(str, "");
  }

  return count;
}

int _dataset_parsing_vertexes(WireframeModel *model, char *buffer, int index) {
  char str[BUFSIZ] = {0};
  int count = 0;
  int curr_vrt;
  buffer += 2;

  while (*buffer) {
    int length = 0;
    sscanf(buffer, "%s", str);
    length = strlen(str) + 1;
    buffer += length;

    if (length > 1) {
      sscanf(str, "%u", &curr_vrt);
      model->fcs_ARR[index].vrts[count] = curr_vrt;
      ++count;
    }

    if (curr_vrt > model->vrt_MAX_INDEX) model->vrt_MAX_INDEX = curr_vrt;

    strcpy(str, "");
  }

  return count;
}

Status _dataset_read_faces(WireframeModel *model, char *buffer, int index) {
  Status status = FAIL;
  int count;

  count = _dataset_count_vertexes_of_face(buffer);
  model->all_vrts_forming_fc_COUNT += count;
  model->num_vrts_forming_fc_ARR[index] = count;

  if (count > 0) model->fcs_ARR[index].vrts = malloc(count * sizeof(int));

  if (model->fcs_ARR[index].vrts) {
    _dataset_parsing_vertexes(model, buffer, index);
    status = OK;
  }

  if (model->vrt_MAX_INDEX > model->vrts_COUNT) status = INCONSIST;

  return status;
}

Status _dataset_read_file(FILE *file, WireframeModel *model) {
  Status status = FAIL;
  char buffer[BUFSIZ] = {0};
  int first_line, i, j;
  double *array = model->crds_ARR;

  first_line = 0;
  i = 0;
  j = 0;

  fseek(file, 0, SEEK_SET);

  while (fgets(buffer, BUFSIZ, file)) {
    if (buffer[0] == 'v' && buffer[1] == ' ') {
      sscanf(buffer + 2, "%lf%lf%lf", &array[i], &array[i + 1], &array[i + 2]);
      _dataset_set_max_min_values(model, array[i], array[i + 1], array[i + 2],
                                  first_line);
      first_line = 1;
      i += 3;
    } else if (buffer[0] == 'f' && buffer[1] == ' ') {
      status = _dataset_read_faces(model, buffer, j++);
    }
  }

  return status;
}

double _dataset_calculate_center(double *min, double *max) {
  double center = *min + (*max - *min) / 2;
  *min -= center;
  *max -= center;

  return center;
}

void kernel_dataset_define_center(WireframeModel *model) {
  model->center_COORD.x = _dataset_calculate_center(&model->axis_COORD.x_min,
                                                    &model->axis_COORD.x_max);
  model->center_COORD.y = _dataset_calculate_center(&model->axis_COORD.y_min,
                                                    &model->axis_COORD.y_max);
  model->center_COORD.z = _dataset_calculate_center(&model->axis_COORD.z_min,
                                                    &model->axis_COORD.z_max);

  for (int i = 0; i < model->vrts_COUNT * 3; i += 3) {
    model->crds_ARR[i] -= model->center_COORD.x;
    model->crds_ARR[i + 1] -= model->center_COORD.y;
    model->crds_ARR[i + 2] -= model->center_COORD.z;
  }
}

Status kernel_dataset_file_to_object(FILE *file, WireframeModel *model) {
  Status status = FAIL;
  char buffer[BUFSIZ] = {0};

  _dataset_init(model);

  while (fgets(buffer, BUFSIZ, file)) {
    if (buffer[0] == 'v' && buffer[1] == ' ')
      ++(model->vrts_COUNT);
    else if (buffer[0] == 'f' && buffer[1] == ' ')
      ++(model->fcs_COUNT);
  }

  if (!model->vrts_COUNT) {
    status = MISSING_V;
    printf(
        "Ошибка инициализации данных. В файле не объявлены типы занчений "
        "[v1][v2][..]\n");
  } else if (!model->fcs_COUNT) {
    status = MISSING_F;
    printf(
        "Ошибка инициализации данных. В файле не объявлены типы занчений "
        "[f1][f2][..]\n");
  } else
    status = _dataset_allocate_memory(model);

  if (status == OK) {
    status = _dataset_read_file(file, model);
    kernel_dataset_define_center(model);
    if (status == OK)
      printf("Инициализация данных WireframeModel прошла успешно.\n");
    else
      printf(
          "Ошибка инициализации данных. Общее количество вершин [vi1][vi2][..] "
          "не соответсвует объявленным индексам в типе [f vi1 vi2 vi3]\n");
  }

  return status;
}
