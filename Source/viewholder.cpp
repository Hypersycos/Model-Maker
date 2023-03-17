#include "viewholder.h"
#include <QApplication>
#include <QRadioButton>
#include <QTimer>
#include <QStyle>

ViewHolder::ViewHolder(QWidget *parent) : QWidget(parent)
{
    layout = new QVBoxLayout();
    buttonLayout = new QHBoxLayout();
    displayButtonLayout = new QHBoxLayout();
    view = new QGraphicsView();
    view->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);
    layout->addLayout(buttonLayout);
    layout->addWidget(view);
    layout->addLayout(displayButtonLayout);

    QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    buttonLayout->addSpacerItem(spacer);

    QStyle *style = QApplication::style();

    QPushButton *playButton = new QPushButton();
    playButton->setIcon(style->standardIcon(QStyle::SP_MediaPlay));
    playButton->setObjectName("play");
    connect(playButton, &QPushButton::clicked, this, &ViewHolder::buttonPress);
    buttonLayout->addWidget(playButton);

    pauseButton = new QPushButton();
    pauseButton->setIcon(style->standardIcon(QStyle::SP_MediaPause));
    pauseButton->setObjectName("pause");
    pauseButton->setEnabled(false);
    connect(pauseButton, &QPushButton::clicked, this, &ViewHolder::buttonPress);
    buttonLayout->addWidget(pauseButton);

    QPushButton *stopButton = new QPushButton();
    stopButton->setIcon(style->standardIcon(QStyle::SP_MediaStop));
    stopButton->setObjectName("stop");
    connect(stopButton, &QPushButton::clicked, this, &ViewHolder::buttonPress);
    buttonLayout->addWidget(stopButton);

    buttonLayout->addSpacerItem(spacer);

    setLayout(layout);
    selected = -1;
    editing();
}

void ViewHolder::addDisplay(Display *display)
{
    displays.append(display);
    QRadioButton *newButton = new QRadioButton(display->getProperty("Name"));
    newButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    newButton->setObjectName(QString::number(displays.length()-1));
    connect(display, &Display::nameChanged, this, &ViewHolder::updateButton);
    displayButtonLayout->addWidget(newButton);
    connect(newButton, &QRadioButton::toggled, this, &ViewHolder::changeDisplay);
}

void ViewHolder::loadDisplay(Display *display)
{
    //used when loading from a file
    tempLoad.append(display);
}

void ViewHolder::updateButton(Display *display)
{ //updates display selectors with display's new name
    QString index = QString::number(displays.indexOf(display));
    QRadioButton *button = findChild<QRadioButton *>(index);
    button->setText(display->getProperty("Name"));
}

void ViewHolder::changeDisplay(bool checked)
{
    if (!checked)
        return;
    QObject *sender = QObject::sender();
    selected = sender->objectName().toInt();
    view->setScene(displays.at(selected)->scene);
}

void ViewHolder::removeDisplay(Display *display)
{
    int index = displays.indexOf(display);
    if (selected >= index)
        selected--;
    for (int i = index + 1; i < displays.length(); i++)
    { //renames all buttons after, so their positions are correct
        QRadioButton *button = buttonLayout->findChild<QRadioButton *>(QString::number(i));
        button->setObjectName(QString::number(i-1));
    }
    QString name = QString::number(index);
    delete this->findChild<QRadioButton *>(name);
    displays.removeAll(display);
}

void ViewHolder::reset()
{ //swaps displays and tempLoad
    for (int i = 0; i < displays.length(); i++)
        removeDisplay(displays.at(i));
    view->setScene(nullptr);
    for (int i = 0; i < tempLoad.length(); i++)
        addDisplay(tempLoad.at(i));
    tempLoad = QList<Display *>();
    toggleState(0);
}

void ViewHolder::editing()
{
    if (state != 0)
        return;
    render();
    //this triggers after the Qt event loop has occured once, so the program remains responsive
    QTimer::singleShot(0, this, &ViewHolder::editing);
}

void ViewHolder::running()
{
    if (state != 1)
        return;
    if (selected == -1 || displays.at(selected)->world == nullptr)
        return toggleState(0);
    displays.at(selected)->world->physicsUpdate();
    render();
    QTimer::singleShot(0, this, &ViewHolder::running);
}

void ViewHolder::paused()
{
    if (state != 2)
        return;
    if (selected != -1)
    render();
    QTimer::singleShot(0, this, &ViewHolder::paused);
}

void ViewHolder::render()
{
    if (selected != -1)
    {
        Display *display = displays.at(selected);
        display->update(state);
        view->update();
    }
}

void ViewHolder::buttonPress()
{
    QObject *sender = QObject::sender();
    QString name = sender->objectName();
    if (name == "play")
        toggleState(1);
    else if (name == "pause")
        toggleState(2);
    else if (name == "stop")
        toggleState(0);
}

void ViewHolder::toggleState(int newState)
{
    if (state == newState)
        return;
    if (selected == -1)
    {
        state = 0;
        editing();
        return;
    }

    if (newState == 1 && state == 0)
    {//if going from editing to running
        displays.at(selected)->world->startRun();
        displays.at(selected)->start();
    }

    state = newState;

    if (state == 0)
    {
        displays.at(selected)->world->stopRun();
        pauseButton->setEnabled(false);
        editing();
    }
    else if (state == 1)
    {
        displays.at(selected)->world->unpause();
        pauseButton->setEnabled(true);
        running();
    }
    else
    {
        paused();
    }
}
