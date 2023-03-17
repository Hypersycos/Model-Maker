#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QSplitter>
//#include <QStandardItemModel>
#include <QItemSelectionModel>
#include <QPushButton>
#include "projecttreemodel.h"
#include <QGraphicsView>
#include "viewholder.h"

class SubWindow;
class HomeWidget;
class Display;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    std::pair<QStringList, QStringList> getRecentNamesAndPaths();
    bool swapWidget(SubWindow *widget);
    bool shiftWidgetUp(SubWindow *widget);
    bool shiftWidgetDown(SubWindow *widget);

public slots:
    void newFile();
    void open();
    void getRecentFiles();
    void save();
    void saveAs();
    void toggleProjectTree();
    void toggleProperties();
    void toggleConnector();
    void clearRecentFiles();

private:
    void createActions();
    void createMenus();
    void createSubWindows();
    void loadProject(QString file);
    void saveProject(QString path);
    void modifyRecentProjects(QString file, int mode);
    void closeEvent(QCloseEvent *event) override;

    void displayHomeMenu();
    void displayEditMenu();
    void displayRunMenu();

    QSplitter *leftSplitter;
    QSplitter *rightSplitter;
    QSplitter *mainSplitter;

    ProjectTreeModel *itemModel;
    QItemSelectionModel *selectionModel;

    //subwindows rather than specific classes used for easier use of swap/shift widget methods
    SubWindow *projectTree;
    SubWindow *properties;
    SubWindow *connector;
    ViewHolder *viewHolder;
    HomeWidget *homeWidget;

    QMenu *fileMenu;
    QMenu *windowMenu;
    QMenu *recentMenu;

    QAction *newAct;
    QAction *openAct;
    QAction *getRecentFilesAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *projectTreeAct;
    QAction *propertiesAct;
    QAction *connectorAct;
};

#endif
