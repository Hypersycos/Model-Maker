#ifndef SUBWINDOW_H
#define SUBWINDOW_H

#include <QWidget>
#include <QVBoxLayout>

class MainWindow;

class SubWindow : public QWidget
{
public:
    SubWindow(MainWindow *creator, QAction *toggle, QString name);
    void normalise(int position, bool isLeft);
    virtual void reset();

public slots:

protected:
    MainWindow *parent;
    QVBoxLayout *layout;
    void swap();
    void shiftUp();
    void shiftDown();
};

#endif // SUBWINDOW_H
