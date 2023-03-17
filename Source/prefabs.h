#ifndef PREFABS_H
#define PREFABS_H

#include <QWidget>
#include <QPushButton>
#include <QScrollArea>
#include "subwindow.h"

class Prefabs : public SubWindow
{
public:
    Prefabs(MainWindow *parent, QAction *toggle);
    void updatePrefabs();

private:
    void createPrefab();
    void removePrefab();
    void savePrefabs();
    void recreateGrid();
    void resizeEvent(QResizeEvent *event) override;

    int width;
    QScrollArea *scrollArea;
    QGridLayout *prefabLayout;
    QList<QPushButton *> prefabs;
};

#endif // PREFABS_H
