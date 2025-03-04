#include "explorer.h"

#include <QDesktopServices>
#include <QUrl>

Explorer::Explorer(QObject *parent) : QAbstractListModel(parent) {}

void Explorer::getFolderList(QString folderPath, QFileInfoList *dirList) {
  QDir dir = QDir(folderPath);

  *dirList = dir.entryInfoList(QDir::NoFilter, QDir::DirsFirst);

  this->aDirList = dirList;
}

int Explorer::rowCount(const QModelIndex &) const {
  return this->aDirList->count();
}

QVariant Explorer::data(const QModelIndex &index, int role) const {
  QVariant value;

  switch (role) {
    case Qt::DisplayRole: {
      value = this->aDirList->at(index.row()).fileName();
    } break;

    case Qt::DecorationRole: {
      if (this->aDirList->at(index.row()).isDir()) {
        QPixmap icon = QPixmap(":/resources/fileicon/folder.png");
        QPixmap tmp = icon.scaled(30, 30, Qt::KeepAspectRatio);
        value = tmp;
        break;
      }

      if (this->aDirList->at(index.row()).isFile()) {
        QString fileExt = this->aDirList->at(index.row()).completeSuffix();

        if ((fileExt == "png")) {
          QPixmap icon = QPixmap(":/resources/fileicon/png.png");
          QPixmap tmp = icon.scaled(30, 30, Qt::KeepAspectRatio);
          value = tmp;
          break;
        }

        if ((fileExt == "jpg")) {
          QPixmap icon = QPixmap(":/resources/fileicon/jpg.png");
          QPixmap tmp = icon.scaled(30, 30, Qt::KeepAspectRatio);
          value = tmp;
          break;
        }

        if ((fileExt == "gif")) {
          QPixmap icon = QPixmap(":/resources/fileicon/gif.png");
          QPixmap tmp = icon.scaled(30, 30, Qt::KeepAspectRatio);
          value = tmp;
          break;
        }

        if ((fileExt == "obj")) {
          QPixmap icon = QPixmap(":/resources/fileicon/obj.png");
          QPixmap tmp = icon.scaled(30, 30, Qt::KeepAspectRatio);
          value = tmp;
          break;
        }

        if ((fileExt == "txt")) {
          QPixmap icon = QPixmap(":/resources/fileicon/txt.png");
          QPixmap tmp = icon.scaled(30, 30, Qt::KeepAspectRatio);
          value = tmp;
          break;
        }

        QPixmap icon = QPixmap(":/resources/fileicon/file.png");
        QPixmap tmp = icon.scaled(30, 30, Qt::KeepAspectRatio);
        value = tmp;
        break;
      }

      value = this->aDirList->at(index.row()).fileName();
    } break;

    case Qt::UserRole: {
      value = this->aDirList->at(index.row()).fileName();
    } break;

    default:
      break;
  }

  return value;
}