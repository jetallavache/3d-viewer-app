#include "viewport.h"

using namespace std::placeholders;

Viewport::Viewport(QWidget *parent, QStatusBar *widget, QDoubleSpinBox *dx,
                   QDoubleSpinBox *dy, QDoubleSpinBox *dz, QDoubleSpinBox *rx,
                   QDoubleSpinBox *ry, QDoubleSpinBox *rz, QDoubleSpinBox *s)
    : QOpenGLWidget(parent),
      statusBar(widget),
      spinPositionX(dx),
      spinPositionY(dy),
      spinPositionZ(dz),
      spinTurnX(rx),
      spinTurnY(ry),
      spinTurnZ(rz),
      spinScale(s) {}

Viewport::~Viewport() {
  if (data3DObject != nullptr) {
    kernel_dataset_free(&data3DObject);
  }

  if (allVertexesArray != nullptr) {
    free(allVertexesArray);
    allVertexesArray = nullptr;
  }
}

void Viewport::fileTo3DObject(QString fileName) {
  QByteArray inputBytes = fileName.toUtf8();
  char *convertedFileName = strdup(inputBytes.constData());

  FILE *file = NULL;

  if (!kernel_utils_open_file(&file, convertedFileName)) {
    if (data3DObject != nullptr) {
      kernel_dataset_free(&data3DObject);
    }

    if (allVertexesArray != nullptr) {
      free(allVertexesArray);
      allVertexesArray = nullptr;
    }

    data3DObject = (WireframeModel *)malloc(sizeof(WireframeModel));

    Status status = kernel_dataset_file_to_object(file, data3DObject);

    if (status == OK) {
      initDataset3DObject();
    } else {
      if (status == INCONSIST) {
        QMessageBox::information(
            nullptr, "INCONSIST: Ошибка инициализации",
            "Несоответсвие данных. Не удалось создать объект " + fileName);

      } else if (status == MISSING_V) {
        QMessageBox::information(
            nullptr, "MISSING_V: Ошибка инициализации",
            "Отсутсвуют объявления вершин. Не удалось создать объект " +
                fileName);

      } else if (status == MISSING_F) {
        QMessageBox::information(
            nullptr, "MISSING_F: Ошибка инициализации",
            "Отсутсвуют объявления граней. Не удалось создать объект " +
                fileName);

      } else {
        QMessageBox::information(nullptr, "FAIL: Ошибка инициализации",
                                 "Не удалось создать объект " + fileName);
      }

      kernel_dataset_free(&data3DObject);
    }
  }

  functionsTransf.push_back(
      std::bind(kernel_transform_move_x, data3DObject, _1));
  functionsTransf.push_back(
      std::bind(kernel_transform_move_y, data3DObject, _1));
  functionsTransf.push_back(
      std::bind(kernel_transform_move_z, data3DObject, _1));
  functionsTransf.push_back(
      std::bind(kernel_transform_rotate_x, data3DObject, _1));
  functionsTransf.push_back(
      std::bind(kernel_transform_rotate_y, data3DObject, _1));
  functionsTransf.push_back(
      std::bind(kernel_transform_rotate_z, data3DObject, _1));
  functionsTransf.push_back(
      std::bind(kernel_transform_rescale, data3DObject, _1));

  free(convertedFileName);
  kernel_utils_close_file(&file);
}

void Viewport::initDataset3DObject() {
  xRot = 22.0f;
  yRot = 22.0f;
  zRot = 0.0f;
  xTra = 0.0f;
  yTra = 0.0f;
  zTra = 0.0f;
  spinPositionX->setValue(0);
  spinPositionY->setValue(0);
  spinPositionZ->setValue(0);
  spinTurnX->setValue(0);
  spinTurnY->setValue(0);
  spinTurnZ->setValue(0);
  spinScale->setValue(0);

  if (projection == Projection::PARALLEL)
    nSca = 0.15f;
  else
    nSca = 1.5f;

  vertexesCount = data3DObject->vrts_COUNT;
  facesCount = data3DObject->fcs_COUNT;

  if (allVertexesArray != nullptr) {
    free(allVertexesArray);
    allVertexesArray = nullptr;
  }

  allVertexesCount = data3DObject->all_vrts_forming_fc_COUNT * 2;

  allVertexesArray = (int *)malloc(allVertexesCount * sizeof(int));

  for (int i = 0, k = 0; i < facesCount; i++) {
    for (int j = 0, l = 0; j < data3DObject->num_vrts_forming_fc_ARR[i];
         j++, l += 2) {
      allVertexesArray[(k + l)] = data3DObject->fcs_ARR[i].vrts[j] - 1;

      if (j + 1 == data3DObject->num_vrts_forming_fc_ARR[i]) {
        allVertexesArray[(k + l) + 1] = data3DObject->fcs_ARR[i].vrts[0] - 1;
      } else {
        allVertexesArray[(k + l) + 1] =
            data3DObject->fcs_ARR[i].vrts[j + 1] - 1;
      }
    }

    k += data3DObject->num_vrts_forming_fc_ARR[i] * 2;
  }
}

void Viewport::initializeGL() { glEnable(GL_DEPTH_TEST); }

void Viewport::resizeGL(int w, int h) {
  widthViewport = w;
  heightViewport = h;

  glViewport(0, 0, w, h);
}

void Viewport::paintGL() {
  /* Устанавливаем проекцию */

  glClearColor(fieldColor.redF(), fieldColor.greenF(), fieldColor.blueF(),
               fieldColor.alphaF());

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  projectionDefiner(widthViewport, heightViewport);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glScalef(nSca, nSca, nSca);
  glTranslatef(xTra, yTra, zTra);
  glRotatef(xRot, 1.0f, 0.0f, 0.0f);
  glRotatef(yRot, 0.0f, 1.0f, 0.0f);
  glRotatef(zRot, 0.0f, 0.0f, 1.0f);

  /* Рисуем оси */

  if (gridView == View::SHOW) {
    drawAxis();
  }

  if (axisView == View::SHOW) {
    drawGrid();
  }

  /* Рисуем 3D-объект*/
  if (data3DObject != nullptr) {
    draw3DObject();
  }

  QString text = "Height: " + QString::number(heightViewport) +
                 " Width: " + QString::number(widthViewport) + "   Sc " +
                 QString::number(nSca) + ", Rot(x) " + QString::number(xRot) +
                 ", Rot(y) " + QString::number(yRot);

  statusBar->showMessage(text, 5000);
}

void Viewport::drawAxis() {
  qreal asix = 100.0f / nSca;
  glLineWidth(1.0f);

  /* Ось x */

  glColor4f(asixColorX.redF(), asixColorX.greenF(), asixColorX.blueF(),
            asixColorX.alphaF());
  glBegin(GL_LINES);
  glVertex3f(asix, 0.0f, 0.0f);
  glVertex3f(-asix, 0.0f, 0.0f);
  glEnd();

  /* Ось y */

  glColor4f(asixColorY.redF(), asixColorY.greenF(), asixColorY.blueF(),
            asixColorY.alphaF());

  glBegin(GL_LINES);
  glVertex3f(0.0f, asix, 0.0f);
  glVertex3f(0.0f, -asix, 0.0f);

  /* Ось z */

  glColor4f(asixColorZ.redF(), asixColorZ.greenF(), asixColorZ.blueF(),
            asixColorZ.alphaF());

  glVertex3f(0.0f, 0.0f, asix);
  glVertex3f(0.0f, 0.0f, -asix);
  glEnd();
}

void Viewport::drawGrid() {
  qreal step = 0.5f;
  qreal grid = 100.0f / nSca;
  glLineWidth(0.5f);

  glColor4f(gridColor.redF(), gridColor.greenF(), gridColor.blueF(),
            gridColor.alphaF());

  glBegin(GL_LINES);

  for (qreal i = -grid; i < grid; i += step) {
    glVertex3f(grid, 0.0f, i);
    glVertex3f(-grid, 0.0f, i);
    glVertex3f(i, 0.0f, grid);
    glVertex3f(i, 0.0f, -grid);
  }

  glEnd();
}

void Viewport::draw3DObject() {
  glEnableClientState(GL_VERTEX_ARRAY);
  vertexesDrawing();
  edgesDrawing();
  glDisableClientState(GL_VERTEX_ARRAY);
}

void Viewport::projectionDefiner(int width, int height) {
  qreal vert, horiz;
  qreal koef = (qreal)height / (qreal)width;

  if (width > height) {
    vert = 1.0 / koef;
    horiz = 1.0;
  } else {
    vert = 1.0;
    horiz = 1.0 * koef;
  }

  switch (projection) {
    case Projection::CENTRAL:
      glFrustum(-vert, vert, -horiz, horiz, 1.0, 50.0);  //
      glTranslatef(0.0f, 0.0f, -10.0f);                  //-10.0f
      break;

    case Projection::PARALLEL:
      glOrtho(-vert, vert, -horiz, horiz, -10.0, 10.0);

    default:
      break;
  }
}

void Viewport::vertexesDrawing() {
  glVertexPointer(3, GL_DOUBLE, 0, data3DObject->crds_ARR);

  switch (vertexStyle) {
    case StyleVertex::NONE:
      break;

    case StyleVertex::CIRCLE:
      glEnable(GL_POINT_SMOOTH);
      glPointSize(vertexSize);
      glColor3f(vertexColor.redF(), vertexColor.greenF(), vertexColor.blueF());
      glDrawArrays(GL_POINTS, 0, data3DObject->vrts_COUNT);
      break;

    case StyleVertex::SQUARE:
      glDisable(GL_POINT_SMOOTH);
      glPointSize(vertexSize);
      glColor3f(vertexColor.redF(), vertexColor.greenF(), vertexColor.blueF());
      glDrawArrays(GL_POINTS, 0, data3DObject->vrts_COUNT);
      break;

    default:
      break;
  }
}

void Viewport::edgesDrawing() {
  glLineWidth(facesWidth);

  switch (facesType) {
    case TypeLine::DOTTED:
      glEnable(GL_LINE_STIPPLE);
      glLineStipple(1, 0x00ff);
      glColor3f(facesColor.redF(), facesColor.greenF(), facesColor.blueF());
      glDrawElements(GL_LINES, allVertexesCount, GL_UNSIGNED_INT,
                     allVertexesArray);
      glDisable(GL_LINE_STIPPLE);
      break;

    case TypeLine::STRAIGHT:
      glColor3f(facesColor.redF(), facesColor.greenF(), facesColor.blueF());
      glDrawElements(GL_LINES, allVertexesCount, GL_UNSIGNED_INT,
                     allVertexesArray);
      break;

    default:
      break;
  }
}

void Viewport::move3DObjectX(double value) {
  if (data3DObject != nullptr) {
    kernel_transform_move_x(data3DObject, value);
    update();
  }
}

void Viewport::move3DObjectY(double value) {
  if (data3DObject != nullptr) {
    kernel_transform_move_y(data3DObject, value);
    update();
  }
}

void Viewport::move3DObjectZ(double value) {
  if (data3DObject != nullptr) {
    kernel_transform_move_z(data3DObject, value);
    update();
  }
}

void Viewport::zoom3DObject(double value) {
  if (data3DObject != nullptr) {
    double zoom = 1.0 + value * 0.01f;

    if (zoom < 0.1f) zoom = 0.1f;

    kernel_transform_rescale(data3DObject, zoom);
    update();
  }
}

void Viewport::rotate3DObjectX(double value) {
  if (data3DObject != nullptr) {
    kernel_transform_rotate_x(data3DObject, value / 57.29578);
    update();
  }
}

void Viewport::rotate3DObjectY(double value) {
  if (data3DObject != nullptr) {
    kernel_transform_rotate_y(data3DObject, value / 57.29578);
    update();
  }
}

void Viewport::rotate3DObjectZ(double value) {
  if (data3DObject != nullptr) {
    kernel_transform_rotate_z(data3DObject, value / 57.29578);
    update();
  }
}

void Viewport::setMouseMode(MouseMode mode) { mouseMode = mode; }

void Viewport::putCenter() {
  if (data3DObject != nullptr) {
    kernel_dataset_define_center(data3DObject);
    update();
  }
}

double Viewport::getDistanceMaxY() {
  return (data3DObject->axis_COORD.y_min + data3DObject->axis_COORD.y_max) / 2;
}

void Viewport::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::RightButton)
    typeMouse = TypeButtonMouse::RIGHT;
  else
    typeMouse = TypeButtonMouse::LEFT;

  ptrMousePosition = event->position();
}

void Viewport::setSpiners(double value) {
  switch (mouseMode) {
    case MouseMode::DRAGGINGXZ:
      spinPositionX->setValue(value);
      break;

    case MouseMode::DRAGGINGXY:
      spinPositionY->setValue(value);
      break;

    case MouseMode::DRAGGINGYZ:
      spinPositionZ->setValue(value);
      break;

    case MouseMode::ROTATIONX:
      spinTurnX->setValue(value);
      break;

    case MouseMode::ROTATIONY:
      spinTurnY->setValue(value);
      break;

    case MouseMode::ROTATIONZ:
      spinTurnZ->setValue(value);
      break;

    case MouseMode::SCALE:
      spinScale->setValue(value);
      break;

    default:
      break;
  }
}

double Viewport::rotationTransf(double delta, double value) {
  const auto &selectedFunction = functionsTransf.at((size_t)mouseMode);

  if (data3DObject != nullptr) {
    if ((value > -360) && (value < 360)) {
      if (delta < 0) {
        value += 1;
      } else {
        value -= 1;
      }
      selectedFunction(((value / 361)));
      setSpiners(value);
    } else if (value == -360) {
      value += 1;
    } else {
      value -= 1;
    }
  }

  return value;
}

void Viewport::mouseMoveEvent(QMouseEvent *event) {
  if (typeMouse == TypeButtonMouse::LEFT) {
    double moveVert;
    double moveHoriz;

    double deltaVert = (event->position().x() - ptrMousePosition.x()) / 25;
    double deltaHoriz = (event->position().y() - ptrMousePosition.y()) / 25;

    switch (mouseMode) {
      case MouseMode::DRAGGINGXY:

        valueX += deltaVert;
        valueY += -deltaHoriz;

        moveVert = (valueX - currentPositionX) / 1000;
        moveHoriz = (valueY - currentPositionY) / 1000;

        move3DObjectX(moveVert);
        currentPositionX = valueX;
        spinPositionX->setValue(valueX);

        move3DObjectY(moveHoriz);
        currentPositionY = valueY;
        spinPositionY->setValue(valueY);

        break;

      case MouseMode::DRAGGINGXZ:

        valueX += deltaVert;
        valueZ += deltaHoriz;

        moveVert = (valueX - currentPositionX) / 1000;
        moveHoriz = (valueZ - currentPositionZ) / 1000;

        move3DObjectX(moveVert);
        currentPositionX = valueX;
        spinPositionX->setValue(valueX);

        move3DObjectZ(moveHoriz);
        currentPositionZ = valueZ;
        spinPositionZ->setValue(valueZ);

        break;

      case MouseMode::DRAGGINGYZ:

        valueY += -deltaHoriz;
        valueZ += deltaVert;

        moveVert = (valueY - currentPositionY) / 1000;
        moveHoriz = (valueZ - currentPositionZ) / 1000;

        move3DObjectY(moveVert);
        currentPositionY = valueY;
        spinPositionY->setValue(valueY);

        move3DObjectZ(moveHoriz);
        currentPositionZ = valueZ;
        spinPositionZ->setValue(valueZ);

        break;

      case MouseMode::ROTATIONX:
        currentTurnX = rotationTransf((deltaHoriz - deltaVert), currentTurnX);
        break;

      case MouseMode::ROTATIONY:
        currentTurnY = rotationTransf((deltaHoriz - deltaVert), currentTurnY);
        break;

      case MouseMode::ROTATIONZ:
        currentTurnZ = rotationTransf((deltaHoriz - deltaVert), currentTurnZ);
        break;

      case MouseMode::SCALE:

        zoom += (-deltaHoriz + deltaVert) * 10;
        zoom3DObject(-deltaHoriz + deltaVert);
        currentScale = zoom;
        spinScale->setValue(zoom);

        break;

      default:
        break;
    }
  } else {
    xRot += 180 / nSca * (qreal)(event->position().y() - ptrMousePosition.y()) /
            heightViewport;
    yRot += 180 / nSca * (qreal)(event->position().x() - ptrMousePosition.x()) /
            widthViewport;
  }

  ptrMousePosition = event->position();

  update();
}

void Viewport::wheelEvent(QWheelEvent *event) {
  if ((event->angleDelta().y()) > 0) {
    if (nSca < 100) nSca *= 1.05;
  } else if ((event->angleDelta().y()) < 0) {
    if (nSca > 0.1) nSca /= 1.05;
  }

  update();
}

void Viewport::setProjection(Projection type) {
  projection = type;

  if (type == Projection::PARALLEL)
    nSca = 0.3f;
  else
    nSca = 2.3f;

  update();
}

void Viewport::setProjectionXY() {
  if (projection == Projection::CENTRAL)
    xRot = 15;
  else
    xRot = 0;

  yRot = 0;
  zRot = 0;
  update();
}

void Viewport::setProjectionYZ() {
  if (projection == Projection::CENTRAL)
    xRot = 15;
  else
    xRot = 0;

  yRot = -90;
  zRot = 0;
  update();
}

void Viewport::setProjectionXZ() {
  xRot = 90;
  yRot = 0;
  zRot = 0;
  update();
}

void Viewport::setViewGrid(View view) {
  gridView = view;
  update();
}

void Viewport::setViewAxis(View view) {
  axisView = view;
  update();
}

void Viewport::setColorBackground(QColor color) {
  fieldColor = color;
  update();
}

void Viewport::deleteModel() {
  if (data3DObject != nullptr) {
    kernel_dataset_free(&data3DObject);
    update();
  }
}

void Viewport::setStyleVertex(StyleVertex style) {
  vertexStyle = style;
  update();
}

void Viewport::changedSizeVertex(double size) {
  vertexSize = size;
  update();
}

void Viewport::setColorVertex(QColor color) {
  vertexColor = color;
  update();
}

void Viewport::setTypeLine(TypeLine type) {
  facesType = type;
  update();
}

void Viewport::changedWidthFaces(double width) {
  facesWidth = width;
  update();
}

void Viewport::setColorFaces(QColor color) {
  facesColor = color;
  update();
}
