#include <QApplication>
#include <QLocale>

#include <clocale>

#include "window.h"

int main(int argc, char *argv[]) {


  QApplication app(argc, argv);
  QLocale::setDefault(QLocale::C);
  std::setlocale(LC_ALL, "C");

  QFontDatabase::addApplicationFont(":/resources/ttf/Hack-Regular.ttf");

  QFont font = QFont(QFontDatabase::applicationFontFamilies(0).at(0));
  font.setPointSize(9);

  QApplication::setFont(font);

  Window win;
  win.show();

  return app.exec();
}
