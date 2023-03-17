#include "subwindow.h"
#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QAction>
#include <QLabel>
#include <QApplication>
#include <QStyle>

SubWindow::SubWindow(MainWindow *creator, QAction *toggle, QString name) : QWidget(creator)
{
    parent = creator;
    layout = new QVBoxLayout();

    QWidget *titleBar = new QWidget();
    titleBar->setStyleSheet("background: #AAAAAA");

    QLabel *title = new QLabel(name);
    QStyle *style = QApplication::style();

    QPushButton *closeButton = new QPushButton();
    closeButton->setStyleSheet("padding: 0px;"
                               "text-align: center");

    closeButton->setFlat(true);
    closeButton->setContentsMargins(0,0,0,0);
    closeButton->setMinimumSize(QSize(0,0));
    closeButton->setIcon(style->standardIcon(QStyle::SP_TitleBarCloseButton));
    connect(closeButton, &QPushButton::clicked, toggle, &QAction::toggle);
    //if close button pressed, simulate clicking one of the buttons in the Window menu. Ensures checkbox is consistent

    QPushButton *upButton = new QPushButton();
    upButton->setFlat(true);
    upButton->setIcon(style->standardIcon(QStyle::SP_ArrowUp));
    upButton->setObjectName("UpButton");
    connect(upButton, &QPushButton::clicked, this, &SubWindow::shiftUp);

    QPushButton *downButton = new QPushButton();
    downButton->setFlat(true);
    downButton->setIcon(style->standardIcon(QStyle::SP_ArrowDown));
    downButton->setObjectName("DownButton");
    connect(downButton, &QPushButton::clicked, this, &SubWindow::shiftDown);

    QPushButton *shiftButton = new QPushButton();
    shiftButton->setFlat(true);
    shiftButton->setIcon(style->standardIcon(QStyle::SP_ArrowLeft));
    shiftButton->setObjectName("ShiftButton");
    connect(shiftButton, &QPushButton::clicked, this, &SubWindow::swap);

    QHBoxLayout *titleLayout = new QHBoxLayout();
    titleLayout->addWidget(title);
    titleLayout->setAlignment(title, Qt::AlignLeft);

    titleLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

    titleLayout->addWidget(shiftButton);
    titleLayout->setAlignment(shiftButton, Qt::AlignRight);
    titleLayout->addWidget(upButton);
    titleLayout->setAlignment(upButton, Qt::AlignRight);
    titleLayout->addWidget(downButton);
    titleLayout->setAlignment(downButton, Qt::AlignRight);
    titleLayout->addWidget(closeButton);
    titleLayout->setAlignment(closeButton, Qt::AlignRight);


    titleLayout->setSpacing(0);
    titleLayout->setContentsMargins(11,0,0,0);

    titleBar->setLayout(titleLayout);

    layout->addWidget(titleBar);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    this->setLayout(layout);
}

void SubWindow::normalise(int position, bool isLeft)
{ //position: 0 = top, 1 = somewhere in the middle, 2 = bottom

    QPushButton *upButton = this->findChild<QPushButton *>("UpButton");
    QPushButton *downButton = this->findChild<QPushButton *>("DownButton");

    upButton->show();
    downButton->show();

    if (position == 0)
        upButton->hide();
    else if (position == 2)
        downButton->hide();

    QPushButton *shiftButton = this->findChild<QPushButton *>("ShiftButton");
    QStyle *style = QApplication::style();

    if (isLeft)
        shiftButton->setIcon(style->standardIcon(QStyle::SP_ArrowRight));
    else
        shiftButton->setIcon(style->standardIcon(QStyle::SP_ArrowLeft));
}

void SubWindow::swap()
{
    QPushButton *shiftButton = this->findChild<QPushButton *>("ShiftButton");
    QStyle *style = QApplication::style();
    if (parent->swapWidget(this))
    {
        shiftButton->setIcon(style->standardIcon(QStyle::SP_ArrowLeft));
    }
    else
    {
        shiftButton->setIcon(style->standardIcon(QStyle::SP_ArrowRight));
    }
}

void SubWindow::shiftUp()
{
    if (parent->shiftWidgetUp(this))
    {//if at top
        this->findChild<QPushButton *>("UpButton")->hide();
    }
    this->findChild<QPushButton *>("DownButton")->show();
}

void SubWindow::shiftDown()
{
    if (parent->shiftWidgetDown(this))
    {//if at bottom
        this->findChild<QPushButton *>("DownButton")->hide();
    }
    this->findChild<QPushButton *>("UpButton")->show();
}

void SubWindow::reset()
{

}
