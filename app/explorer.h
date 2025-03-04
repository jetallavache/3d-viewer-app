#ifndef SRC_3DVIEWER_APP_EXPLORER_H_
#define SRC_3DVIEWER_APP_EXPLORER_H_

#pragma once

#include <QDir>
#include <QModelIndex>
#include <QPixmap>

class Explorer : public QAbstractListModel {
 public:
  Explorer(QObject *parent = nullptr);
  int rowCount(const QModelIndex &) const;
  QVariant data(const QModelIndex &index, int role) const;

  void getFolderList(QString folderPath, QFileInfoList *dirList);

 private:
  QFileInfoList *aDirList;
};

#endif /* SRC_3DVIEWER_APP_EXPLORER_H_ */