#include "prefabs.h"
#include "mainwindow.h"
#include <QPushButton>
#include <math.h>
#include <QScrollArea>
#include <QResizeEvent>

Prefabs::Prefabs(MainWindow *parent, QAction *toggle) : SubWindow(parent, toggle, tr("Prefabs"))
{
    scrollArea = new QScrollArea(this);
    layout->addWidget(scrollArea);
    width = 3;
    updatePrefabs();
}

void Prefabs::updatePrefabs()
{
    prefabs = parent->loadPrefabs();
    recreateGrid();
}

void Prefabs::resizeEvent(QResizeEvent *event)
{
    int newWidth = event->size().width();
    width = int(floor(newWidth/100));
    recreateGrid();
}

void Prefabs::recreateGrid()
{ //ensures prefab buttons fit horizontally
    QWidget *container = new QWidget();
    prefabLayout = new QGridLayout();
    container->setLayout(prefabLayout);

    int row = 0;
    int col = 0;
    for (int i = 0; i < prefabs.length(); i++)
    {
        prefabLayout->addWidget(prefabs.at(i), row, col);
        if (col == width)
        {
            row++;
            col = 0;
        }
        else
        {
            col++;
        }
    }
    scrollArea->setWidget(container);
}
