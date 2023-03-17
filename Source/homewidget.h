#ifndef HOMEWIDGET_H
#define HOMEWIDGET_H
#include <QWidget>
#include <QVBoxLayout>

class MainWindow;

class HomeWidget : public QWidget
{

public:
    HomeWidget(MainWindow *creator);
    void updateRecentDisplay();

private:
    MainWindow *parent;
    QVBoxLayout *load;
};

#endif // HOMEWIDGET_H
