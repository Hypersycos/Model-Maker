#ifndef VIEWHOLDER_H
#define VIEWHOLDER_H

#include <QWidget>
#include "display.h"
#include <QGraphicsView>
#include <QBoxLayout>
#include <QPushButton>

class ViewHolder : public QWidget
{
    Q_OBJECT
public:
    ViewHolder(QWidget *parent = nullptr);
    void loadDisplay(Display *display);
    void addDisplay(Display *display);
    void removeDisplay(Display *display);
    void reset();
    void editing();
    void running();
    void paused();
    void render();
    void updateButton(Display *display);
    void changeDisplay(bool checked);
    void toggleState(int newtate);

    void buttonPress();

private:
    QGraphicsView *view;
    QVBoxLayout *layout;
    QHBoxLayout *displayButtonLayout;
    QHBoxLayout *buttonLayout;
    QPushButton *pauseButton;

    int selected;
    QList<Display *> displays;
    QList<Display *> tempLoad;
    int state;
};

#endif // VIEWHOLDER_H
