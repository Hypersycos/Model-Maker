#include <homewidget.h>
#include <projecttree.h>
#include <mainwindow.h>
#include <sstream>
#include <QtWidgets>

HomeWidget::HomeWidget(MainWindow *creator) : QWidget(creator)
{
    parent = creator;
    QVBoxLayout *create = new QVBoxLayout();
    //split into two vertical layouts, for create button and load buttons

    QPushButton *newButton = new QPushButton("New Blank Project");
    connect(newButton, &QPushButton::clicked, parent, &MainWindow::newFile);
    create->addWidget(newButton);

    load = new QVBoxLayout();
    QPushButton *openButton = new QPushButton(tr("Open a Project"));
    connect(openButton, &QPushButton::clicked, parent, &MainWindow::open);
    load->addWidget(openButton);

    QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

    QHBoxLayout *options = new QHBoxLayout();
    options->addSpacerItem(spacer);
    options->addLayout(create);
    options->addSpacerItem(spacer);
    options->addLayout(load);
    options->addSpacerItem(spacer);
    options->setAlignment(load, Qt::AlignTop);
    options->setAlignment(create, Qt::AlignTop);

    //adds recent project buttons to load
    updateRecentDisplay();

    QLabel *tip = new QLabel(tr("<i>Open a project with <b>CTRL+O</b>"
                                " or create a new project with <b>CTRL+N</b></i> "));
    tip->setAlignment(Qt::AlignCenter);
    tip->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QSpacerItem *vspacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
    //spacer takes up excess space

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(5);
    layout->addSpacerItem(vspacer);
    layout->addWidget(tip);
    layout->addLayout(options);
    layout->addSpacerItem(vspacer);
    layout->addSpacerItem(vspacer);
    layout->addSpacerItem(vspacer);
    layout->addSpacerItem(vspacer);
    //adding multiple times gives a nicer looking ratio

    this->setLayout(layout);
}

void HomeWidget::updateRecentDisplay()
{
    std::pair<QStringList, QStringList> recentsData = parent->getRecentNamesAndPaths();
    int length = recentsData.first.length();
    QStringList paths = recentsData.first;
    QStringList names = recentsData.second;
    int i = 0;
    //declare i outside so we can use it after

    for (i=0; i<length; i++)
    {
        QPushButton *button = this->findChild<QPushButton *>(QString(i));
        //try to rewrite existing buttons if possible, rather than clearing children

        std::stringstream text;
        text << i+1 << ": " << names.at(i).toStdString() << " - " << paths.at(i).toStdString();
        QString buttonText = text.str().c_str();

        if (button == nullptr)
        {
            QPushButton *newButton = new QPushButton(buttonText);
            newButton->setFlat(true);
            newButton->setStyleSheet("Text-align:left");
            newButton->setObjectName(QString(i));
            connect(newButton, &QPushButton::clicked, parent, &MainWindow::open);
            load->addWidget(newButton);
        }
        else
        {
            button->setText(buttonText);
        }
    }


    while (this->findChild<QPushButton *>(QString(i)) != nullptr)
    { //delete any excess buttons
        delete this->findChild<QPushButton *>(QString(i));
        i++;
    }
}
