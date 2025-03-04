#ifndef SRC_3DVIEWER_APP_WINDOW_H_
#define SRC_3DVIEWER_APP_WINDOW_H_

#pragma once

#include <QColorDialog>
#include <QFileDialog>
#include <QFileInfo>
#include <QFrame>
#include <QHBoxLayout>
#include <QImage>
#include <QLayout>
#include <QMainWindow>
#include <QPalette>
#include <QSettings>
#include <QSize>
#include <QSizePolicy>
#include <QStatusBar>
#include <QVBoxLayout>

#include "explorer.h"
#include "viewport.h"

QT_BEGIN_NAMESPACE

namespace Ui {
class Window;
}

QT_END_NAMESPACE

class Window : public QMainWindow {
  Q_OBJECT

  /* Поля класса Window */

 public:
 private:
  Ui::Window *ui;
  QString fontRegular;
  Viewport *viewport;
  QSettings *settings;

  QFileInfoList *dirList;
  Explorer *model;

  QStatusBar *statusBar;

  QPushButton *draggingXZButton;
  QPushButton *draggingXYButton;
  QPushButton *draggingYZButton;
  QPushButton *rotationXButton;
  QPushButton *rotationYButton;
  QPushButton *rotationZButton;
  QPushButton *scaleButton;
  QPushButton *centerButton;

  QDir *currentFolder;
  QDir *folderDataSamples;
  QDir *folderRenderImages;
  QString gifPath;
  QString gifName;

  int widthGif, heightGif;
  QLabel *labelInfoFile;

  double currentPositionX = 0;
  double currentPositionY = 0;
  double currentPositionZ = 0;
  int currentTurnX = 0;
  int currentTurnY = 0;
  int currentTurnZ = 0;
  int currentScale = 0;

  double currentSizeVertex = 1;
  double currentWidthFaces = 1;
  QColor currentBackgrColor = QColor(180, 180, 180, 0);

  const int gifFps = 10, gifLength = 5;
  QTimer *timer;
  int startTime, tmpTime;
  int counter = 1;

  /* Конструкторы и деструктор класса Window */
 public:
  Window(QWidget *parent = nullptr);
  ~Window();

  /* Методы класса Window */
 private:
  void setInfoObject(QString filePath);
  void openFileAndInitModel(QString filePath);
  void initSettings();
  void setSettings();
  void getSettings();

  void createGif(QString filePath, int widthGif, int heightGif);
  void updateViewBrowser();

 signals:
  void emitFileTo3DObject(QString fileName);

  void emitRotate3DObjectX(int value);
  void emitRotate3DObjectY(int value);
  void emitRotate3DObjectZ(int value);
  void emitZoom3DObject(int value);
  void emitMove3DObjectX(double value);
  void emitMove3DObjectY(double value);
  void emitMove3DObjectZ(double value);

  void emitSetMouseMode(MouseMode mode);
  void emitPutCenter();

  void emitViewGrid(View view);
  void emitViewAxis(View view);
  void emitSetColorBackground(QColor color);
  void emitDeleteModel();

  void emitSetProjection(Projection type);
  void emitSetProjectionXY();
  void emitSetProjectionYZ();
  void emitSetProjectionXZ();

  void emitSetStyleVertex(StyleVertex style);
  void emitChangedSizeVertex(double value);
  void emitSetColorVertex(QColor color);

  void emitSetTypeFaces(TypeLine type);
  void emitChangedSizeFaces(double width);
  void emitSetColorFaces(QColor color);

  /* Слоты панели управления */
 private slots:
  /* Это свои */
  void clickedDraggingXZMode();
  void clickedDraggingXYMode();
  void clickedDraggingYZMode();

  void clickedRotationXMode();
  void clickedRotationYMode();
  void clickedRotationZMode();
  void clickedScaleMode();
  void clickedPutCenter();
  void frameGif();

  /* Афинные преобразования */
  void on_draggingZSpinBox_valueChanged(double value);
  void on_draggingYSpinBox_valueChanged(double value);
  void on_draggingXSpinBox_valueChanged(double value);
  void on_zoomSpinBox_valueChanged(double value);
  void on_rotationZSpinBox_valueChanged(double value);
  void on_rotationYSpinBox_valueChanged(double value);
  void on_rotationXSpinBox_valueChanged(double value);

  /* Проекция */
  void on_projectionComboBox_currentIndexChanged(int index);
  void on_viewPlaneYZButton_clicked();
  void on_viewPlaneXYButton_clicked();
  void on_viewPlaneXZButton_clicked();

  /* Параметры граней */
  void on_facesComboBox_currentIndexChanged(int index);
  void on_facesSpinBox_valueChanged(double arg1);
  void on_facesColor_clicked();

  /* Параметры вершин*/
  void on_vertexComboBox_currentIndexChanged(int index);
  void on_vertexSpinBox_valueChanged(double value);
  void on_vertexColor_clicked();

  /* Слоты меню */

  void on_openFileMenu_triggered();
  void on_JPGAnimationMenu_triggered();
  void on_PNGAnimationMenu_triggered();
  void on_gifAnimationMenu_triggered();
  void on_colorWindowMenu_triggered();
  void on_gridWindowMenu_triggered(bool checked);
  void on_axisWindowMenu_triggered(bool checked);
  void on_deleteFileMenu_triggered();
  void on_exitFileMenu_triggered();

  /* Файловый менеджер */
  void on_folderUpButton_clicked();
  void on_listView_doubleClicked(const QModelIndex &index);
};

#endif /* SRC_3DVIEWER_APP_WINDOW_H_ */
