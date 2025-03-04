#include "window.h"

#include "./ui_window.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "../submodules/gif/gif.h"

#ifdef __cplusplus
}
#endif

Window::Window(QWidget *parent) : QMainWindow(parent), ui(new Ui::Window) {
  ui->setupUi(this);

  setWindowTitle("3DViewer-v1.0");

  fontRegular = QFontDatabase::applicationFontFamilies(0).at(0);

  statusBar = new QStatusBar();
  statusBar->setStyleSheet("background-color: #e0e0e0;");

  viewport = new Viewport(nullptr, statusBar, ui->draggingXSpinBox,
                          ui->draggingYSpinBox, ui->draggingZSpinBox,
                          ui->rotationXSpinBox, ui->rotationYSpinBox,
                          ui->rotationZSpinBox, ui->zoomSpinBox);

  labelInfoFile = new QLabel();
  ui->statusbar->addPermanentWidget(labelInfoFile);

  dirList = new QFileInfoList();
  model = new Explorer();

  currentFolder = new QDir();
  currentFolder->setPath(QDir::currentPath());
  currentFolder->cdUp();
  currentFolder->cd("src");

  folderRenderImages = new QDir();
  folderRenderImages->mkdir(QDir::homePath() + "/Images-3DViewer");
  folderRenderImages->cd(QDir::homePath() + "/Images-3DViewer");

  folderDataSamples = new QDir();
  folderDataSamples->setPath(QDir::currentPath());
  folderDataSamples->cdUp();
  folderDataSamples->cd("src/data-samples");

  model->getFolderList(folderDataSamples->absolutePath(), dirList);
  ui->listView->setModel(model);
  ui->pathEdit->setText(folderDataSamples->absolutePath());
  ui->listView->setWordWrap(true);

#ifdef Q_OS_MAC
  settings = new QSettings("grenoullelogique.fr", "Squash", this);
#else
  settings = new QSettings("Grenoulle Logique", "Squash", this);
#endif

  QVBoxLayout *panelLayout = new QVBoxLayout();

  QHBoxLayout *controlLayout = new QHBoxLayout();
  controlLayout->setSpacing(0);

  QPushButton *leftMouse = new QPushButton(
      QIcon(":/new/images/resources/left-mouse.png"), ": Трансформация ");
  leftMouse->setEnabled(false);
  QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  leftMouse->setSizePolicy(sizePolicy);

  controlLayout->insertWidget(0, leftMouse);

  QPushButton *wheelMouse = new QPushButton(
      QIcon(":/new/images/resources/wheel-mouse.png"), ": Масштаб сцены ");
  wheelMouse->setEnabled(false);
  wheelMouse->setSizePolicy(sizePolicy);

  controlLayout->insertWidget(1, wheelMouse);

  QPushButton *rightMouse = new QPushButton(
      QIcon(":/new/images/resources/right-mouse.png"), ": Поворот сцены ");
  rightMouse->setEnabled(false);
  rightMouse->setSizePolicy(sizePolicy);

  controlLayout->insertWidget(2, rightMouse);
  controlLayout->insertWidget(3, new QLabel());

  viewport->setStyleSheet(
      "QPushButton {"
      "     color: #000000; "
      "     background-color: #20000000; "
      "     border: 0px;"
      "     border-radius: 3px;"
      "     width: 30; height: 30;"
      "} "
      "QPushButton:disabled {"
      "     color: #3b3b3b; "
      "     text-align: center;"
      "     background-color: #20000000; "
      "     border: 0px;"
      "     border-radius: 3px;"
      "     width: 150; height: 20; margin: 2px"
      "} "
      "QPushButton:hover {"

      "     background-color: #90014A82; "

      "} "
      "QPushButton:checked {"

      "     background-color: #60002b4d; "

      "} "
      "QPushButton:pressed {"

      "     background-color: #80014A82; "

      "}");

  panelLayout->addLayout(controlLayout);

  QGridLayout *gridButtons = new QGridLayout();
  //   gridButtons->setHorizontalSpacing(2);

  draggingXZButton =
      new QPushButton(QIcon(":/new/images/resources/dxz.png"), "");
  draggingXYButton =
      new QPushButton(QIcon(":/new/images/resources/dxy.png"), "");
  draggingYZButton =
      new QPushButton(QIcon(":/new/images/resources/dyz.png"), "");
  rotationXButton = new QPushButton(QIcon(":/new/images/resources/rx.png"), "");
  rotationYButton = new QPushButton(QIcon(":/new/images/resources/ry.png"), "");
  rotationZButton = new QPushButton(QIcon(":/new/images/resources/rz.png"), "");
  scaleButton = new QPushButton(QIcon(":/new/images/resources/s.png"), "");
  centerButton = new QPushButton(
      QIcon(":/new/images/resources/center-mouse-icon.png"), "");

  draggingXZButton->setCheckable(true);
  draggingXYButton->setCheckable(true);
  draggingYZButton->setCheckable(true);
  rotationXButton->setCheckable(true);
  rotationYButton->setCheckable(true);
  rotationZButton->setCheckable(true);
  scaleButton->setCheckable(true);

  gridButtons->addWidget(draggingXZButton, 1, 1);
  gridButtons->addWidget(draggingXYButton, 1, 2);
  gridButtons->addWidget(draggingYZButton, 1, 3);
  gridButtons->addWidget(rotationXButton, 1, 4);
  gridButtons->addWidget(rotationYButton, 1, 5);
  gridButtons->addWidget(rotationZButton, 1, 6);
  gridButtons->addWidget(scaleButton, 1, 7);
  gridButtons->addWidget(centerButton, 1, 8);
  gridButtons->setRowStretch(0, 1);
  gridButtons->setColumnStretch(0, 1);

  panelLayout->addLayout(gridButtons);
  viewport->setLayout(panelLayout);

  ui->viewportLayout->insertWidget(0, viewport);
  ui->viewportLayout->insertWidget(1, statusBar);

  ui->tabWidget->setTabIcon(
      0, QIcon(":/new/images/resources/transform-control-icon.png"));
  ui->tabWidget->setTabIcon(
      1, QIcon(":/new/images/resources/projection-control-icon.png"));
  ui->tabWidget->setTabIcon(
      2, QIcon(":/new/images/resources/vertexes-control-icon.png"));
  ui->tabWidget->setTabIcon(
      3, QIcon(":/new/images/resources/line-control-icon.png"));

  connect(draggingXZButton, SIGNAL(clicked()), this,
          SLOT(clickedDraggingXZMode()));
  connect(draggingXYButton, SIGNAL(clicked()), this,
          SLOT(clickedDraggingXYMode()));
  connect(draggingYZButton, SIGNAL(clicked()), this,
          SLOT(clickedDraggingYZMode()));
  connect(rotationXButton, SIGNAL(clicked()), this,
          SLOT(clickedRotationXMode()));
  connect(rotationYButton, SIGNAL(clicked()), this,
          SLOT(clickedRotationYMode()));
  connect(rotationZButton, SIGNAL(clicked()), this,
          SLOT(clickedRotationZMode()));
  connect(scaleButton, SIGNAL(clicked()), this, SLOT(clickedScaleMode()));
  connect(centerButton, SIGNAL(clicked()), this, SLOT(clickedPutCenter()));

  connect(this, &Window::emitFileTo3DObject, viewport,
          &Viewport::fileTo3DObject);

  connect(this, &Window::emitRotate3DObjectX, viewport,
          &Viewport::rotate3DObjectX);
  connect(this, &Window::emitRotate3DObjectY, viewport,
          &Viewport::rotate3DObjectY);
  connect(this, &Window::emitRotate3DObjectZ, viewport,
          &Viewport::rotate3DObjectZ);
  connect(this, &Window::emitZoom3DObject, viewport, &Viewport::zoom3DObject);
  connect(this, &Window::emitMove3DObjectX, viewport, &Viewport::move3DObjectX);
  connect(this, &Window::emitMove3DObjectY, viewport, &Viewport::move3DObjectY);
  connect(this, &Window::emitMove3DObjectZ, viewport, &Viewport::move3DObjectZ);

  connect(this, &Window::emitSetMouseMode, viewport, &Viewport::setMouseMode);
  connect(this, &Window::emitPutCenter, viewport, &Viewport::putCenter);

  connect(this, &Window::emitViewGrid, viewport, &Viewport::setViewGrid);
  connect(this, &Window::emitViewAxis, viewport, &Viewport::setViewAxis);
  connect(this, &Window::emitSetColorBackground, viewport,
          &Viewport::setColorBackground);
  connect(this, &Window::emitDeleteModel, viewport, &Viewport::deleteModel);

  connect(this, &Window::emitSetProjection, viewport, &Viewport::setProjection);
  connect(this, &Window::emitSetProjectionYZ, viewport,
          &Viewport::setProjectionYZ);
  connect(this, &Window::emitSetProjectionXY, viewport,
          &Viewport::setProjectionXY);
  connect(this, &Window::emitSetProjectionXZ, viewport,
          &Viewport::setProjectionXZ);

  connect(this, &Window::emitSetStyleVertex, viewport,
          &Viewport::setStyleVertex);
  connect(this, &Window::emitChangedSizeVertex, viewport,
          &Viewport::changedSizeVertex);
  connect(this, &Window::emitSetColorVertex, viewport,
          &Viewport::setColorVertex);

  connect(this, &Window::emitSetTypeFaces, viewport, &Viewport::setTypeLine);
  connect(this, &Window::emitChangedSizeFaces, viewport,
          &Viewport::changedWidthFaces);
  connect(this, &Window::emitSetColorFaces, viewport, &Viewport::setColorFaces);

  getSettings();
}

Window::~Window() {
  setSettings();
  delete ui;
  delete viewport;
}

/* Мои слоты */

void Window::clickedDraggingXZMode() {
  emitSetMouseMode(MouseMode::DRAGGINGXZ);
  draggingXZButton->setChecked(true);
  draggingXYButton->setChecked(false);
  draggingYZButton->setChecked(false);
  rotationXButton->setChecked(false);
  rotationYButton->setChecked(false);
  rotationZButton->setChecked(false);
  scaleButton->setChecked(false);
  ui->tabWidget->setCurrentIndex(0);
}

void Window::clickedDraggingXYMode() {
  emitSetMouseMode(MouseMode::DRAGGINGXY);
  draggingXZButton->setChecked(false);
  draggingXYButton->setChecked(true);
  draggingYZButton->setChecked(false);
  rotationXButton->setChecked(false);
  rotationYButton->setChecked(false);
  rotationZButton->setChecked(false);
  scaleButton->setChecked(false);
  ui->tabWidget->setCurrentIndex(0);
}

void Window::clickedDraggingYZMode() {
  emitSetMouseMode(MouseMode::DRAGGINGYZ);
  draggingXZButton->setChecked(false);
  draggingXYButton->setChecked(false);
  draggingYZButton->setChecked(true);
  rotationXButton->setChecked(false);
  rotationYButton->setChecked(false);
  rotationZButton->setChecked(false);
  scaleButton->setChecked(false);
  ui->tabWidget->setCurrentIndex(0);
}

void Window::clickedRotationXMode() {
  emitSetMouseMode(MouseMode::ROTATIONX);
  draggingXZButton->setChecked(false);
  draggingXYButton->setChecked(false);
  draggingYZButton->setChecked(false);
  rotationXButton->setChecked(true);
  rotationYButton->setChecked(false);
  rotationZButton->setChecked(false);
  scaleButton->setChecked(false);
  ui->tabWidget->setCurrentIndex(0);
}

void Window::clickedRotationYMode() {
  emitSetMouseMode(MouseMode::ROTATIONY);
  draggingXZButton->setChecked(false);
  draggingXYButton->setChecked(false);
  draggingYZButton->setChecked(false);
  rotationXButton->setChecked(false);
  rotationYButton->setChecked(true);
  rotationZButton->setChecked(false);
  scaleButton->setChecked(false);
  ui->tabWidget->setCurrentIndex(0);
}

void Window::clickedRotationZMode() {
  emitSetMouseMode(MouseMode::ROTATIONZ);
  draggingXZButton->setChecked(false);
  draggingXYButton->setChecked(false);
  draggingYZButton->setChecked(false);
  rotationXButton->setChecked(false);
  rotationYButton->setChecked(false);
  rotationZButton->setChecked(true);
  scaleButton->setChecked(false);
  ui->tabWidget->setCurrentIndex(0);
}

void Window::clickedScaleMode() {
  emitSetMouseMode(MouseMode::SCALE);
  draggingXZButton->setChecked(false);
  draggingXYButton->setChecked(false);
  draggingYZButton->setChecked(false);
  rotationXButton->setChecked(false);
  rotationYButton->setChecked(false);
  rotationZButton->setChecked(false);
  scaleButton->setChecked(true);
  ui->tabWidget->setCurrentIndex(0);
}

void Window::clickedPutCenter() {
  emit emitPutCenter();
  emit emitZoom3DObject(0);
  emit emitRotate3DObjectX(0);
  emit emitRotate3DObjectY(0);
  emit emitRotate3DObjectZ(0);

  ui->draggingXSpinBox->setValue(0);
  ui->draggingYSpinBox->setValue(0);
  ui->draggingZSpinBox->setValue(0);
  ui->rotationXSpinBox->setValue(0);
  ui->rotationYSpinBox->setValue(0);
  ui->rotationXSpinBox->setValue(0);
  ui->zoomSpinBox->setValue(0);
  ui->tabWidget->setCurrentIndex(0);
}

/* Слоты меню */

void Window::on_openFileMenu_triggered() {
  QString filePath = QFileDialog::getOpenFileName(
      this, "Выбрать файл", folderDataSamples->absolutePath(), ".obj (*.obj)");

  if (filePath.isEmpty()) {
    QMessageBox::information(nullptr, "Ошибка",
                             "Файл не соответсвует типу .obj");
  } else
    openFileAndInitModel(filePath);
}

void Window::setInfoObject(QString filePath) {
  QFileInfo fileInfo(filePath);

  QString info = "Vert " + QString::number(viewport->vertexesCount, 10) +
                 ", Fac " + QString::number(viewport->facesCount, 10) + " (" +
                 fileInfo.fileName() + ")";
  labelInfoFile->setText(info);
}

void Window::openFileAndInitModel(QString filePath) {
  emit emitFileTo3DObject(filePath);
  emit emitZoom3DObject(0);
  emit emitRotate3DObjectX(180);

  viewport->update();
  setInfoObject(filePath);
}

void Window::on_JPGAnimationMenu_triggered() {
  QDateTime dateTime = dateTime.currentDateTime();
  QString currentDateTime = dateTime.toString("yyyy_MM_dd_HHmmss_zzz");
  QString file =
      folderRenderImages->absolutePath() + "/" + currentDateTime + ".jpeg";

  QString filePath = QFileDialog::getSaveFileName(this, tr("Сохранить как "),
                                                  file, tr("*.jpeg"));

  if (filePath.isEmpty()) {
    QMessageBox::information(nullptr, "Ошибка", "Не удалось сохранить файл");
  } else {
    viewport->grab().save(filePath);
  }

  QString text = "Создан файл: \"" + filePath + "\"";

  ui->statusbar->showMessage(text, 15000);

  updateViewBrowser();
}

void Window::on_PNGAnimationMenu_triggered() {
  QDateTime dateTime = dateTime.currentDateTime();
  QString currentDateTime = dateTime.toString("yyyy_MM_dd_HHmmss_zzz");

  QString file =
      folderRenderImages->absolutePath() + "/" + currentDateTime + ".bmp";

  QString filePath = QFileDialog::getSaveFileName(this, tr("Сохранить как "),
                                                  file, tr("*.bmp"));

  if (filePath.isEmpty()) {
    QMessageBox::information(nullptr, "Ошибка", "Не удалось сохранить файл");
  } else {
    viewport->grab().save(filePath);
  }

  QString text = "Создан файл: \"" + filePath + "\"";

  ui->statusbar->showMessage(text, 15000);

  updateViewBrowser();
}

void Window::createGif(QString filePath, int widthGif, int heightGif) {
  QDir pathFile(filePath);

  QDateTime dateTime = dateTime.currentDateTime();
  QString currentDateTime = dateTime.toString("yyyy_MM_dd_HHmmss_zzz");

  gifName = filePath + "/" + currentDateTime + ".gif";
  QByteArray ga = gifName.toLocal8Bit();

  GifWriter writer = {};
  int err = 0;

  if (GifBegin(&writer, ga.data(), widthGif, heightGif, 10, 8, false)) {
    for (int i = 1; i <= 50; i++) {
      if (err == 1) {
        break;
      }

      QImage img(filePath + tr("/gif-tmp/") + QString::number(i) + tr(".bmp"));

      if (!img.isNull()) {
        if (GifWriteFrame(&writer,
                          img.convertToFormat(QImage::Format_Indexed8)
                              .convertToFormat(QImage::Format_RGBA8888)
                              .constBits(),
                          widthGif, heightGif, 10, 8, false)) {
        } else {
          QMessageBox::critical(0, "Error-1", "Не возможно создать gif-файл.");
          err = 1;
        }
      } else {
        QMessageBox::critical(0, "Error-2", "Не возможно создать gif-файл.");
        err = 1;
      }
    }

    if (err == 0) {
      GifEnd(&writer);
    }
  } else {
    err = 1;
    QMessageBox::critical(0, "Error-3", "Не возможно создать gif-файл.");
  }

  if (err == 1) {
    if (QFile::exists(gifName)) {
      QFile::remove(gifName);
    }
  }

  pathFile.setPath(filePath + tr("/gif-tmp/"));
  pathFile.removeRecursively();
}

void Window::frameGif() {
  QString text;

  if (startTime == tmpTime) {
    viewport->grab()
        .scaled(widthGif, heightGif, Qt::IgnoreAspectRatio)
        .save(gifPath + tr("/gif-tmp/") + QString::number(counter) +
              tr(".bmp"));
    counter++;
    tmpTime += 1000 / gifFps;
    text = "Запись GIF-анимации  " + QString::number(counter / 10) + " c";
  }

  if (startTime == 1000 * gifLength) {
    createGif(gifPath, widthGif, heightGif);
    timer->stop();
    counter = 1;
    text = "Запись завершена! Файл: \"" + gifName + "\"";
  }

  startTime += 1000 / gifFps;
  ui->statusbar->showMessage(text, 15000);
}

void Window::on_gifAnimationMenu_triggered() {
  widthGif = viewport->geometry().size().width();
  heightGif = viewport->geometry().size().height();

  gifPath = QFileDialog::getExistingDirectory(
      this, tr("Выбрать место для сохранения"),
      folderRenderImages->absolutePath());

  if (gifPath.isEmpty()) {
    QMessageBox::information(nullptr, "Ошибка",
                             "Не удалось открыть выбранное местоположение");
  } else {
    QDir *pathDir = new QDir();
    pathDir->mkdir(gifPath + "/gif-tmp");

    startTime = 0, tmpTime = 1000 / gifFps;
    timer = new QTimer(this);

    connect(timer, SIGNAL(timeout()), this, SLOT(frameGif()));

    timer->start(1000 / gifFps);
  }
}

void Window::on_exitFileMenu_triggered() { QApplication::quit(); }

void Window::on_deleteFileMenu_triggered() {
  emit emitDeleteModel();
  labelInfoFile->setText(tr(" "));
}

void Window::on_gridWindowMenu_triggered(bool checked) {
  if (checked == false) {
    emit emitViewGrid(View::SHOW);
    ui->gridWindowMenu->setText(tr("Скрыть сетку"));
  } else {
    emit emitViewGrid(View::HIDE);
    ui->gridWindowMenu->setText(tr("Показать сетку"));
  }
}

void Window::on_axisWindowMenu_triggered(bool checked) {
  if (checked == false) {
    emit emitViewAxis(View::SHOW);
    ui->axisWindowMenu->setText(tr("Скрыть оси"));
  } else {
    emit emitViewAxis(View::HIDE);
    ui->axisWindowMenu->setText(tr("Показать оси"));
  }
}

void Window::on_colorWindowMenu_triggered() {
  currentBackgrColor = QColorDialog::getColor(Qt::gray, this, "Цвет фона");

  if (currentBackgrColor.isValid())
    emit emitSetColorBackground(currentBackgrColor);
}

/* Слоты вкладки с афинными преобразованиями */

void Window::on_draggingXSpinBox_valueChanged(double value) {
  double move = (value - currentPositionX) / 10;
  emitMove3DObjectX(move);
  currentPositionX = value;
  viewport->valueX = value;
}

void Window::on_draggingYSpinBox_valueChanged(double value) {
  double move = (value - currentPositionY) / 10;
  emitMove3DObjectY(move);
  currentPositionY = value;
  viewport->valueY = value;
}

void Window::on_draggingZSpinBox_valueChanged(double value) {
  double move = (value - currentPositionZ) / 10;
  emitMove3DObjectZ(move);
  currentPositionZ = value;
  viewport->valueZ = value;
}

void Window::on_zoomSpinBox_valueChanged(double value) {
  emitZoom3DObject(currentScale);
  currentScale = value;
  viewport->zoom = value;
}

void Window::on_rotationXSpinBox_valueChanged(double value) {
  emitRotate3DObjectX(value - currentTurnX);
  currentTurnX = value;
  viewport->currentTurnX = value;
}

void Window::on_rotationYSpinBox_valueChanged(double value) {
  emitRotate3DObjectY(value - currentTurnY);
  currentTurnY = value;
  viewport->currentTurnY = value;
}

void Window::on_rotationZSpinBox_valueChanged(double value) {
  emitRotate3DObjectZ(value - currentTurnZ);
  currentTurnZ = value;
  viewport->currentTurnZ = value;
}

/* Слоты в с вкладкой проекции */

void Window::on_projectionComboBox_currentIndexChanged(int index) {
  switch (index) {
    case 0:
      emit emitSetProjection(Projection::PARALLEL);
      break;

    case 1:
      emit emitSetProjection(Projection::CENTRAL);
      break;

    default:
      break;
  }
}

void Window::on_viewPlaneYZButton_clicked() { emit emitSetProjectionYZ(); }

void Window::on_viewPlaneXYButton_clicked() { emit emitSetProjectionXY(); }

void Window::on_viewPlaneXZButton_clicked() { emit emitSetProjectionXZ(); }

/* Слоты в с вкладкой параметров вершин */

void Window::on_vertexComboBox_currentIndexChanged(int index) {
  switch (index) {
    case 0:
      emit emitSetStyleVertex(StyleVertex::NONE);
      break;

    case 1:
      emit emitSetStyleVertex(StyleVertex::CIRCLE);
      break;

    case 2:
      emit emitSetStyleVertex(StyleVertex::SQUARE);
      break;

    default:
      break;
  }
}

void Window::on_vertexSpinBox_valueChanged(double value) {
  emit emitChangedSizeVertex(currentSizeVertex);
  currentSizeVertex = value;
}

void Window::on_vertexColor_clicked() {
  QColor color = QColorDialog::getColor(Qt::black, this, "Цвет вершин");

  if (color.isValid()) {
    emit emitSetColorVertex(color);
    ui->vertexColor->setStyleSheet("background-color:" + color.name());
  }
}

/* Слоты в с вкладкой параметров граней */

void Window::on_facesComboBox_currentIndexChanged(int index) {
  switch (index) {
    case 0:
      emit emitSetTypeFaces(TypeLine::STRAIGHT);
      break;

    case 1:
      emit emitSetTypeFaces(TypeLine::DOTTED);
      break;

    default:
      break;
  }
}

void Window::on_facesSpinBox_valueChanged(double value) {
  emit emitChangedSizeFaces(currentWidthFaces);
  currentWidthFaces = value;
}

void Window::on_facesColor_clicked() {
  QColor color = QColorDialog::getColor(Qt::black, this, "Цвет граней");

  if (color.isValid()) {
    emit emitSetColorFaces(color);
    ui->facesColor->setStyleSheet("background-color:" + color.name());
  }
}

void Window::setSettings() {
  QColor color;

  settings->beginGroup("Application");
  settings->setValue("BackgrColorR", currentBackgrColor.red());
  settings->setValue("BackgrColorG", currentBackgrColor.green());
  settings->setValue("BackgrColorB", currentBackgrColor.blue());
  settings->setValue("BackgrColorA", currentBackgrColor.alpha());
  settings->endGroup();

  settings->beginGroup("Faces");
  settings->setValue("FacesType", ui->facesComboBox->currentIndex());
  settings->setValue("FacesWidth", ui->facesSpinBox->value());
  color = ui->vertexColor->palette().color(QPalette::Window);
  settings->setValue("FacesColorR", color.red());
  settings->setValue("FacesColorG", color.green());
  settings->setValue("FacesColorB", color.blue());
  settings->endGroup();

  settings->beginGroup("Vertexes");
  settings->setValue("VertexType", ui->vertexComboBox->currentIndex());
  settings->setValue("VertexSize", ui->vertexSpinBox->value());
  color = ui->facesColor->palette().color(QPalette::Window);
  settings->setValue("VertexesColorR", color.red());
  settings->setValue("VertexesColorG", color.green());
  settings->setValue("VertexesColorB", color.blue());
  settings->endGroup();

  settings->beginGroup("Projection");
  settings->setValue("ProjectionType", ui->projectionComboBox->currentIndex());
  settings->endGroup();
}

void Window::getSettings() {
  QColor color;

  settings->beginGroup("Application");
  color = QColor(settings->value("BackgrColorR", 180.0).toInt(),
                 settings->value("BackgrColorG", 180.0).toInt(),
                 settings->value("BackgrColorB", 180.0).toInt(),
                 settings->value("BackgrColorA", 180.0).toInt());
  emit emitSetColorBackground(color);
  settings->endGroup();

  settings->beginGroup("Projection");
  ui->projectionComboBox->setCurrentIndex(
      settings->value("ProjectionType", 0).toInt());
  emit emitSetProjection(
      (Projection)settings->value("ProjectionType", 0).toInt());
  settings->endGroup();

  settings->beginGroup("Vertexes");
  ui->vertexComboBox->setCurrentIndex(settings->value("VertexType", 0).toInt());
  emit emitSetStyleVertex(
      (StyleVertex)settings->value("VertexType", 0).toInt());
  ui->vertexSpinBox->setValue(settings->value("VertexSize", 0).toDouble());
  emit emitChangedSizeVertex(settings->value("VertexSize", 0).toDouble());
  color = QColor(settings->value("VertexesColorR", 0.0).toInt(),
                 settings->value("VertexesColorG", 0.0).toInt(),
                 settings->value("VertexesColorB", 0.0).toInt(), 0);
  ui->vertexColor->setStyleSheet("background-color:" + color.name());
  emit emitSetColorVertex(color);
  settings->endGroup();

  settings->beginGroup("Faces");
  ui->facesComboBox->setCurrentIndex(settings->value("FacesType", 0).toInt());
  emit emitSetTypeFaces((TypeLine)settings->value("FacesType", 0).toInt());
  ui->facesSpinBox->setValue(settings->value("FacesWidth", 0).toDouble());
  emit emitChangedSizeFaces(settings->value("FacesWidth", 0).toDouble());
  color = QColor(settings->value("FacesColorR", 0.0).toInt(),
                 settings->value("FacesColorG", 0.0).toInt(),
                 settings->value("FacesColorB", 0.0).toInt(), 0);
  ui->facesColor->setStyleSheet("background-color:" + color.name());
  emit emitSetColorFaces(color);
  settings->endGroup();
}

void Window::updateViewBrowser() {
  ui->listView->setViewMode(QListView::IconMode);
  ui->listView->setViewMode(QListView::ListMode);
}

void Window::on_folderUpButton_clicked() {
  currentFolder->cdUp();

  ui->pathEdit->setText(currentFolder->absolutePath());
  model->getFolderList(currentFolder->absolutePath(), dirList);

  updateViewBrowser();
}

void Window::on_listView_doubleClicked(const QModelIndex &index) {
  QString tmp = dirList->at(index.row()).absoluteFilePath();

  if (dirList->at(index.row()).isDir()) {
    model->getFolderList(tmp, dirList);
    // int ct = model->rowCount(index);
    ui->pathEdit->setText(tmp);

  } else if (dirList->at(index.row()).completeSuffix() == "obj") {
    openFileAndInitModel(tmp);

  } else {
    QDesktopServices::openUrl(QUrl(tmp));
  }

  updateViewBrowser();
}
