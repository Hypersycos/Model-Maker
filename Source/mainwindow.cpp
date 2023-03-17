#include <QtWidgets>
#include "homewidget.h"
#include "projecttree.h"
#include "mainwindow.h"
#include "properties.h"
#include "prefabs.h"
#include "connections.h"
#include "projecttreemodel.h"
#include <QMessageBox>

MainWindow::MainWindow()
{
    QSettings settings;
    createActions();
    createMenus();

    itemModel = new ProjectTreeModel();
    selectionModel = new QItemSelectionModel(itemModel);

    createSubWindows();

    setWindowTitle(tr("Model Maker"));

    if (!this->restoreGeometry(settings.value("geometry").toByteArray())) //attempt to restore saved geometry from closeEvent
    { //if none, start the program in the centre of the primary screen with the size 640x480
        QScreen *screen = QGuiApplication::primaryScreen();
        QRect screenSize = screen->geometry();
        QPoint defaultPosition = QPoint(screenSize.width()/2-320, screenSize.height()/2-240);

        resize(QSize(640, 480));
        move(defaultPosition);
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    settings.setValue("geometry", saveGeometry());
}

void MainWindow::loadProject(QString file)
{
    ProjectTreeModel *temp = new ProjectTreeModel();
    connect(temp, &ProjectTreeModel::displayAdded, viewHolder, &ViewHolder::loadDisplay);
    QString errorMessage = temp->load(file);
    if (errorMessage != "")
    { //if we fail to open the file
        std::pair<QStringList, QStringList> recents = getRecentNamesAndPaths();
        if (recents.first.contains(file))
        {
            errorMessage += tr("\nDo you want to remove this file from your recent files?");
            QMessageBox *error = new QMessageBox(QMessageBox::Warning, tr("Unable to open file"),
                                                 errorMessage, QMessageBox::Yes | QMessageBox::No);
            if (error->exec() == QMessageBox::Yes)
            {
                modifyRecentProjects(file, 1);
            }
        }
        else
        {
            QMessageBox *error = new QMessageBox(QMessageBox::Warning, tr("Unable to open file"),
                                                 errorMessage, QMessageBox::Ok);
            error->exec();
        }
    }
    else
    {
        connector->reset();
        properties->reset();
        setWindowFilePath(file);
        QFileInfo fileInfo(file);
        setWindowTitle("Model Maker - " +fileInfo.fileName());
        modifyRecentProjects(file, 0);
        saveAct->setEnabled(true);
        saveAsAct->setEnabled(true);
        itemModel->changeRoot(temp->rootItem);
        if (mainSplitter->widget(1) == homeWidget)
            mainSplitter->replaceWidget(1, viewHolder);
    }
    disconnect(temp, &ProjectTreeModel::displayAdded, viewHolder, &ViewHolder::loadDisplay);
}

void MainWindow::modifyRecentProjects(QString file, int mode)
{
    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();

    if (mode == 0) //mode 0 adds an item to the list
    {
        files.removeAll(file);
        files.prepend(file);
        while (files.size() > 5)
        {
            files.removeLast();
        }
    }
    else if (mode == 1)
    {
        files.removeAll(file);
    }
    settings.setValue("recentFileList", files);
    homeWidget->updateRecentDisplay();
}

std::pair<QStringList, QStringList> MainWindow::getRecentNamesAndPaths()
{
    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();
    QStringList names;
    for (int i=0; i<files.count(); i++)
    { //gets the file name (without the extension)
        QFileInfo fileInfo(files.at(i));
        QString name = fileInfo.fileName();
        int index;
        if (fileInfo.suffix().length() > 0)
        { //if it has an extension
            index = name.length() - fileInfo.suffix().length() - 1;
        }
        else
        {
            index = name.length();
        }
        names.append(name.mid(0,index));
    }
    return std::make_pair(files, names); //returns file paths and file names
}

void MainWindow::saveProject(QString path)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox *error = new QMessageBox(QMessageBox::Warning, tr("Unable to save file"),
                                             file.errorString(), QMessageBox::Ok);
        error->exec();
        return;
    }

    QStringList toWrite;
    for (int i = 0; i < itemModel->rowCount(); i++)
    {
        QModelIndex index = itemModel->index(i, 0);
        ProjectTreeItem *item = static_cast<ProjectTreeItem *>(index.internalPointer());
        //save is recursive, returns the strings for every child as well
        toWrite += item->reference->save();
    }

    QTextStream out(&file);
    for (int i = 0; i < toWrite.length(); i++)
    {//adds newlines for splitting when reading back in
        out << toWrite.at(i) << "\n";
    }

    setWindowFilePath(path);
    QFileInfo fileInfo(path);
    setWindowTitle("Model Maker - " +fileInfo.fileName());
}

void MainWindow::newFile()
{
    itemModel->reset();
    connector->reset();
    properties->reset();
    setWindowFilePath("");
    setWindowTitle("Model Maker - Unsaved Project");
    saveAct->setEnabled(true);
    saveAsAct->setEnabled(true);
    mainSplitter->replaceWidget(1, viewHolder);
}

void MainWindow::open()
{
    QString file;
    QObject *sender = QObject::sender();
    QString name = sender->property("text").toString();
    if (sender != openAct && name != tr("Open a Project"))
    { //if the signal was sent by a recent file button
        file = sender->property("text").toString();
        int pathIndex = file.indexOf("-");
        file = file.mid(pathIndex+2); //use that file
    }
    else
    { //otherwise, use file dialog to get file
        file = QFileDialog::getOpenFileName(this,
                                                    tr("Open Project"),
                                                    "",
                                                    tr("Project (*.proj);;All Files (*)"));
        if (file.isEmpty()) return; //if they close the pop-up
    }
    loadProject(file);
}

void MainWindow::save()
{
    if (windowFilePath() != "")
        saveProject(windowFilePath());
    else
        saveAs();
}

void MainWindow::saveAs()
{
    QString file = QFileDialog::getSaveFileName(this,
                                        tr("Save Project"),
                                        "",
                                        tr("Project (*.proj)"));
    if (file.isEmpty())
        return;
    saveProject(file);
}

void MainWindow::toggleProjectTree()
{
    if (projectTree->isHidden())
    {
        projectTree->show();
    }
    else
    {
        projectTree->hide();
    }
}

void MainWindow::toggleProperties()
{
    if (properties->isHidden())
    {
        properties->show();
    }
    else
    {
        properties->hide();
    }
}

void MainWindow::toggleConnector()
{
    if (connector->isHidden())
    {
        connector->show();
    }
    else
    {
        connector->hide();
    }
}

bool MainWindow::swapWidget(SubWindow *widget)
{
    if (widget->parentWidget() == leftSplitter)
    {
        rightSplitter->addWidget(widget);
        return true; //show left button
    }
    else
    {
        leftSplitter->addWidget(widget);
        return false; //show right button
    }
}

bool MainWindow::shiftWidgetUp(SubWindow *widget)
{
    QSplitter *splitter = static_cast<QSplitter *>(widget->parentWidget());
    int i = splitter->indexOf(widget);
    if (i>0) //as long as it's not already at the top
    {
        splitter->insertWidget(i-1, widget);
    }
    return i<2; //return whether its new index is < 1 (i.e. it's at the top)
}

bool MainWindow::shiftWidgetDown(SubWindow *widget)
{
    QSplitter *splitter = static_cast<QSplitter *>(widget->parentWidget());
    int i = splitter->indexOf(widget);
    if (i<splitter->count()-1)
    {
        splitter->insertWidget(i+1, widget);
    }
    return i>splitter->count()-3;
}

void MainWindow::getRecentFiles()
{ //creates the recent files menu in the file dropdown
    recentMenu->clear();

    std::pair<QStringList, QStringList> temp = getRecentNamesAndPaths();
    int length = temp.first.length();
    QStringList paths = temp.first;
    QStringList names = temp.second;

    QString label;
    if (length == 1)
    {
        label = "There is 1 recent file";
    }
    else
    {
        label = "There are " + QString::number(length) + " recent files";
    }
    recentMenu->addSection(label);
    if (length==0)
    { //the section won't show if there's no actions, so this filler action is added
        recentMenu->addAction(tr("Recently opened projects will show here"));
    }

    for (int i=0; i<length; i++)
    {
        QString text;
        if (i<9) //& allows navigation with keyboard. Only useful up to 9 though.
            text = "&";
        text += QString::number(i+1) + ": " + names.at(i) + " - " + paths.at(i);

        QAction *newAction = new QAction(text, this);
        recentMenu->addAction(newAction);
        connect(newAction, &QAction::triggered, this, &MainWindow::open);
    }

    if (length > 0)
    {
        QAction *clearAction = new QAction(tr("Clear recent files"), this);
        recentMenu->addAction(clearAction);
        connect(clearAction, &QAction::triggered, this, &MainWindow::clearRecentFiles);
    }
}

void MainWindow::clearRecentFiles()
{
    QSettings settings;
    settings.setValue("recentFileList", QStringList());
    homeWidget->updateRecentDisplay();
}

void MainWindow::createActions()
{
    newAct = new QAction(tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new project"));
    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);

    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing project"));
    connect(openAct, &QAction::triggered, this, &MainWindow::open);

    //save and saveAs disabled at first, since no file open.
    saveAct = new QAction(tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the project to disk"));
    saveAct->setEnabled(false);
    connect(saveAct, &QAction::triggered, this, &MainWindow::save);

    saveAsAct = new QAction(tr("Save &As"), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the project to disk to a certain location"));
    saveAsAct->setEnabled(false);
    connect(saveAsAct, &QAction::triggered, this, &MainWindow::saveAs);

    projectTreeAct = new QAction(tr("Project &Tree"));
    projectTreeAct->setShortcut(QKeySequence(tr("CTRL+SHIFT+T")));
    projectTreeAct->setStatusTip(tr("Hide the project tree subwindow"));
    projectTreeAct->setCheckable(true);
    projectTreeAct->setChecked(true);
    connect(projectTreeAct, &QAction::toggled, this, &MainWindow::toggleProjectTree);

    propertiesAct = new QAction(tr("&Properties"));
    propertiesAct->setShortcut(QKeySequence(tr("CTRL+SHIFT+P")));
    propertiesAct->setStatusTip(tr("Hide the properties subwindow"));
    propertiesAct->setCheckable(true);
    propertiesAct->setChecked(true);
    connect(propertiesAct, &QAction::toggled, this, &MainWindow::toggleProperties);

    connectorAct = new QAction(tr("&Connections"));
    connectorAct->setShortcut(QKeySequence(tr("CTRL+SHIFT+C")));
    connectorAct->setStatusTip(tr("Hide the connections subwindow"));
    connectorAct->setCheckable(true);
    connectorAct->setChecked(true);
    connect(connectorAct, &QAction::toggled, this, &MainWindow::toggleConnector);
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    recentMenu = fileMenu->addMenu(tr("Open &Recent"));
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->setStyleSheet("QMenu { menu-scrollable: 1; max}");

    connect(recentMenu, &QMenu::aboutToShow, this, &MainWindow::getRecentFiles);

    windowMenu = menuBar()->addMenu(tr("&Window"));
    windowMenu->addAction(projectTreeAct);
    windowMenu->addAction(propertiesAct);
    windowMenu->addAction(connectorAct);
}

void MainWindow::createSubWindows()
{
    projectTree = new ProjectTree(this, itemModel, selectionModel, projectTreeAct);
    homeWidget = new HomeWidget(this);
    connector = new Connections(this, connectorAct);
    properties = new Properties(this, selectionModel, propertiesAct, static_cast<Connections *>(connector));
    viewHolder = new ViewHolder();
    //necessary for viewHolder to keep updated with displays
    connect(itemModel, &ProjectTreeModel::displayAdded, viewHolder, &ViewHolder::addDisplay);
    connect(itemModel, &ProjectTreeModel::displayRemoved, viewHolder, &ViewHolder::removeDisplay);
    connect(itemModel, &ProjectTreeModel::modelAboutToBeReset, viewHolder, &ViewHolder::reset);

    leftSplitter = new QSplitter(this);
    leftSplitter->setOrientation(Qt::Orientation::Vertical);
    leftSplitter->setChildrenCollapsible(false);
    leftSplitter->setHandleWidth(1);

    rightSplitter = new QSplitter(this);
    rightSplitter->setOrientation(Qt::Orientation::Vertical);
    rightSplitter->setChildrenCollapsible(false);
    rightSplitter->setHandleWidth(1);

    mainSplitter = new QSplitter(this);
    mainSplitter->setStyleSheet("QSplitter::handle { "
                                "   border: 1px #777777;"
                                "   background: #999999;"
                                "} ");
    mainSplitter->setChildrenCollapsible(false);

    rightSplitter->addWidget(projectTree);
    rightSplitter->addWidget(properties);
    leftSplitter->addWidget(connector);

    //sorts buttons out, so no extraneous ones are visible
    projectTree->normalise(0, false);
    properties->normalise(2, false);
    connector->normalise(2, true);

    mainSplitter->addWidget(leftSplitter);
    mainSplitter->addWidget(homeWidget);
    mainSplitter->addWidget(rightSplitter);

    setCentralWidget(mainSplitter);
}
