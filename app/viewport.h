#ifndef SRC_3DVIEWER_APP_VIEWPORT_H_
#define SRC_3DVIEWER_APP_VIEWPORT_H_

#pragma once

#define GL_SILENCE_DEPRECATION

#include <QDoubleSpinBox>
#include <QMessageBox>
#include <QMouseEvent>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>
#include <QStatusBar>
#include <QString>
#include <QTimer>
#include <QWheelEvent>
#include <QWidget>
#include <QtOpenGL>

#if defined(linux) || defined(_WIN32)
#include <GL/glut.h>
#else
#include <GLUT/GLUT.h>
#endif

#include <functional>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

#include "../kernel/dataset.h"
#include "../kernel/transform.h"
#include "../kernel/utils.h"

#ifdef __cplusplus
}
#endif

enum class StyleVertex { NONE, CIRCLE, SQUARE };

enum class Projection { PARALLEL, CENTRAL };

enum class TypeLine { STRAIGHT, DOTTED };

enum class MouseMode {
  DRAGGINGXZ,
  DRAGGINGXY,
  DRAGGINGYZ,
  ROTATIONX,
  ROTATIONY,
  ROTATIONZ,
  SCALE
};

enum class TypeButtonMouse { LEFT, RIGHT };

enum class View { SHOW, HIDE };

class Viewport : public QOpenGLWidget {
  Q_OBJECT

  /* Поля класса Viewport */

 public:
  int vertexesCount = 0;
  int facesCount = 0;

 private:
  QStatusBar *statusBar;
  QDoubleSpinBox *spinPositionX;
  QDoubleSpinBox *spinPositionY;
  QDoubleSpinBox *spinPositionZ;
  QDoubleSpinBox *spinTurnX;
  QDoubleSpinBox *spinTurnY;
  QDoubleSpinBox *spinTurnZ;
  QDoubleSpinBox *spinScale;

  int widthViewport = 0;
  int heightViewport = 0;

  WireframeModel *data3DObject = nullptr;
  int allVertexesCount = 0;
  int *allVertexesArray = nullptr;

  Projection projection = Projection::CENTRAL;

  QColor fieldColor = QColor(180, 180, 180, 0);

  QColor vertexColor = QColor(0, 0, 0);
  double vertexSize = 1.0;
  StyleVertex vertexStyle = StyleVertex::CIRCLE;

  QColor facesColor = QColor(0, 0, 0);
  double facesWidth = 1.0;
  TypeLine facesType = TypeLine::STRAIGHT;

  MouseMode mouseMode = MouseMode::DRAGGINGXZ;

  std::vector<std::function<void(int)>> functionsTransf;

  TypeButtonMouse typeMouse = TypeButtonMouse::LEFT;

  View axisView = View::SHOW;
  QColor asixColorX = QColor(112, 0, 0, 0);
  QColor asixColorY = QColor(0, 112, 0, 0);
  QColor asixColorZ = QColor(0, 0, 112, 0);

  View gridView = View::SHOW;
  QColor gridColor = QColor(150, 150, 150, 0);

 public:
  double currentPositionX = 0;
  double currentPositionY = 0;
  double currentPositionZ = 0;
  double currentTurnX = 0;
  double currentTurnY = 0;
  double currentTurnZ = 0;
  double currentScale = 0;

  double valueX = 0;
  double valueY = 0;
  double valueZ = 0;
  double rotatX = 0;
  double rotatY = 0;
  double rotatZ = 0;
  double zoom = 0;

  /* new */
 private:
  qreal xRot = 22.0f;
  qreal yRot = 22.0f;
  qreal zRot = 0.0f;
  qreal xTra = 0.0f;
  qreal yTra = 0.0f;
  qreal zTra = 0.0f;
  qreal nSca = 0.5f;

  QPointF ptrMousePosition;

  /* Конструкторы и деструктор класса Viewport */

 public:
  Viewport(QWidget *parent, QStatusBar *widget, QDoubleSpinBox *dx,
           QDoubleSpinBox *dy, QDoubleSpinBox *dz, QDoubleSpinBox *rx,
           QDoubleSpinBox *ry, QDoubleSpinBox *rz, QDoubleSpinBox *s);
  ~Viewport();
  void setMouseMode(MouseMode mode);
  void putCenter();
  double getDistanceMaxY();

  /* Методы класса Viewport */

 public slots:
  void fileTo3DObject(QString fileName);
  void rotate3DObjectX(double value);
  void rotate3DObjectY(double value);
  void rotate3DObjectZ(double value);
  void zoom3DObject(double value);
  void move3DObjectX(double value);
  void move3DObjectY(double value);
  void move3DObjectZ(double value);

  void setViewGrid(View view);
  void setViewAxis(View view);
  void setColorBackground(QColor color);

  void setProjection(Projection type);
  void setProjectionXY();
  void setProjectionYZ();
  void setProjectionXZ();

  void setStyleVertex(StyleVertex style);
  void changedSizeVertex(double size);
  void setColorVertex(QColor color);
  void deleteModel();

  void setTypeLine(TypeLine type);
  void changedWidthFaces(double width);
  void setColorFaces(QColor color);

 protected:
  void initializeGL() override;
  void resizeGL(int w, int h) override;
  void paintGL() override;

  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void wheelEvent(QWheelEvent *event) override;

 private:
  void drawAxis();
  void drawGrid();
  void draw3DObject();
  void initDataset3DObject();

  void setSpiners(double value);
  double rotationTransf(double delta, double value);

  void projectionDefiner(int w, int h);
  void vertexesDrawing();
  void edgesDrawing();
};

#endif /* SRC_3DVIEWER_APP_VIEWPORT_H_ */
