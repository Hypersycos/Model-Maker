#include "properties.h"
#include <QLineEdit>
#include <QStandardItemModel>
#include <QTreeView>
#include <QHeaderView>
#include <QComboBox>
#include <QApplication>
#include <QMenu>
#include <QClipboard>
#include <QMimeData>
#include <QPushButton>
#include <QDialog>
#include <QSortFilterProxyModel>

Properties::Properties(MainWindow *parent, QItemSelectionModel *selectionmodel, QAction *toggle, Connections *connector) : SubWindow(parent, toggle, tr("Properties"))
{
    selectionModel = selectionmodel;

    treeView = new QTreeView();
    treeView->setHeaderHidden(false);
    treeView->resizeColumnToContents(2);
    treeView->resizeColumnToContents(3);
    treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(treeView, &QTreeView::customContextMenuRequested, this, &Properties::contextMenu);
    layout->addWidget(treeView);

    bools << "Anchored" << "Gravity Enabled" << "Render Forces" << "Vertical" << "Force Clockwise Order";
    dropdowns << "Material";
    selectors << "World" << "Property" << "Property2";
    strings << "Name" << "Type" << "Material";
    vectors << "Position" << "Scale" << "Size" << "Velocity" << "Camera Position";
    noInput << "Time" << "Acceleration" << "Acceleration.X" << "Acceleration.Y" << "Type" << "Gravitational Potential Energy"
            << "Kinetic Energy";
    noOutput << "Scale" << "Scale.X" << "Scale.Y" << "Mass" << "Elasticity" << "Friction Coefficient" << "Anchored" << "Centre of Mass"
             << "Centre of Mass.X" << "Centre of Mass.Y" << "Time Multiplier" << "Size" << "Size.X" << "Size.Y";
    IOLessItems << "Display" << "Slider" << "Graph";

    this->connector = connector;

    connect(selectionModel, &QItemSelectionModel::selectionChanged, this, &Properties::updateProperties);
}

void Properties::contextMenu(QPoint pos)
{
    QModelIndex index = treeView->indexAt(pos);
    if (!index.isValid())
        return;

    QMenu *menu = new QMenu(this);

    QAction *copyAct = new QAction(tr("&Copy"));
    copyAct->setData(index);
    connect(copyAct, &QAction::triggered, this, &Properties::copy);
    menu->addAction(copyAct);

    QAction *pasteAct = new QAction(tr("&Paste"));
    pasteAct->setData(index);
    if (QApplication::clipboard()->text() == "")
        pasteAct->setEnabled(false);
    else
        connect(pasteAct, &QAction::triggered, this, &Properties::paste);
    menu->addAction(pasteAct);

    menu->popup(treeView->viewport()->mapToGlobal(pos));
}

void Properties::copy()
{
    QObject *sender = QObject::sender();
    QAction *act = static_cast<QAction *>(sender);
    QModelIndex index = act->data().value<QModelIndex>();
    //we only care about the value of the property, so always take column 1
    QStandardItem *item = model->itemFromIndex(index.siblingAtColumn(1));

    QString text = item->text();

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(text);
}

void Properties::paste()
{
    QObject *sender = QObject::sender();
    QAction *act = static_cast<QAction *>(sender);
    QModelIndex index = act->data().value<QModelIndex>();
    QStandardItem *item = model->itemFromIndex(index.siblingAtColumn(1));

    QClipboard *clipboard = QApplication::clipboard();
    QString text = clipboard->text();

    item->setText(text);
}

void Properties::reset()
{
    updateProperties(QItemSelection(), QItemSelection());
}

void Properties::updateProperties(QItemSelection selected, QItemSelection deselected)
{
    if (selected.indexes().length() == 0 || !selected.indexes().at(0).isValid())
    {
        treeView->setModel(nullptr);
    }
    else
    {
        item = static_cast<ProjectTreeItem *>(selected.indexes().at(0).internalPointer());
        QList<std::pair<QString, QString>> data = item->reference->getAllProperties();
        model = new QStandardItemModel();
        //start building a new model for the view

        propertyOrder = QStringList(); //used for items that disable/enable other items, since using the model's findItems would be inconvenient
        QList<std::pair<QString, QStandardItem *>> toDropdown; //(otherwise would have to find the property name, get its index to find another item, and then enable/disable that)
        QList<std::pair<QString, QStandardItem *>> toSelector;

        QStringList inputs = connector->getInputsForItem(item); //get all current inputs for this item
        QStringList outputs = connector->getOutputsForItem(item);

        QStandardItem *last;

        for (int i = 0; i<data.length(); i++)
        {
            QString property = data.at(i).first;
            QString value = data.at(i).second;
            QList<QStandardItem *> row;

            QStandardItem *temp = new QStandardItem();
            if (property.contains(".")) //if a multi-dimensional property (e.g. a vector)
            { //get rid of everything before the .
                temp->setText(property.mid(property.indexOf(".")+1));
                //don't include in the property order, as that's only top-level properties
            }
            else
            {
                propertyOrder.append(property);
                temp->setText(property);
                last = temp;
            }
            //property name shouldn't be editable
            temp->setEditable(false);
            row.append(temp);



            temp = new QStandardItem();
            if (bools.contains(property))
            {
                //use checkbox for bools
                temp->setCheckable(true);
                if (value == "true")
                    temp->setCheckState(Qt::CheckState::Checked);
                temp->setEditable(false);
                //no text for bools
            }
            else
            {
                if (dropdowns.contains(property))
                    toDropdown.append(std::pair<QString, QStandardItem *>(property, temp)); //append property and item for later
                else if (selectors.contains(property))
                    toSelector.append(std::pair<QString, QStandardItem *>(property, temp));
                else if (noInput.contains(property))
                    temp->setEnabled(false);
                //some properties aren't editable, e.g. time
                temp->setText(value);
            }

            temp->setData(QVariant::fromValue<QString>(property)); //used when data is changed to identify the property
            temp->setData(QVariant::fromValue<int>(1), Qt::UserRole + 2); //identifies the item as a property value
            row.append(temp);


            if (!dropdowns.contains(property) && !strings.contains(property) && !selectors.contains(property))
            {//neither strings, selectors nor dropdowns can be used for inputs or outputs
                temp = new QStandardItem();
                temp->setEditable(false);
                if (!noInput.contains(property) && !IOLessItems.contains(item->getType()))
                {
                    temp->setCheckable(true);
                    if (inputs.contains(property)) //if already an input
                        temp->setCheckState(Qt::CheckState::Checked);

                    temp->setData(QVariant::fromValue<QString>(property));
                    temp->setData(QVariant::fromValue<int>(2), Qt::UserRole + 2);
                }
                row.append(temp);


                temp = new QStandardItem();
                temp->setEditable(false);

                if (!noOutput.contains(property) && !IOLessItems.contains(item->getType()))
                {
                    temp->setCheckable(true);
                    if (outputs.contains(property)) //if already an output
                        temp->setCheckState(Qt::CheckState::Checked);

                    temp->setData(QVariant::fromValue<QString>(property));
                    temp->setData(QVariant::fromValue<int>(3), Qt::UserRole + 2);
                }
                row.append(temp);
            }

            //if multidimensional, add to parent instead of to root
            if (property.contains("."))
                last->appendRow(row);
            else
                model->appendRow(row);
        }

        QStringList headers;
        headers << "Property" << "Value" << "Input" << "Output";
        model->setHorizontalHeaderLabels(headers);

        treeView->setModel(model);
        treeView->resizeColumnToContents(0);
        treeView->resizeColumnToContents(2);
        treeView->resizeColumnToContents(3);

        for (int i=0; i<toDropdown.length(); i++)
        {
            QStringList possibleValues;
            if (toDropdown.at(i).first == "Material")
            {
                //get materials. These are placeholders
                possibleValues << "<Custom>" << "Iron" << "Rubber" << "Wood";
            }
            else
            {
                possibleValues = dropdownValues.at(dropdowns.indexOf(toDropdown.at(i).first)-1);
            }

            QModelIndex index = model->indexFromItem(toDropdown.at(i).second);
            QComboBox *dropdown = new QComboBox();
            dropdown->addItems(possibleValues);
            dropdown->setCurrentText(toDropdown.at(i).second->text()); //set dropdown to current item
            dropdown->setObjectName(toDropdown.at(i).first); //used to identify the property when updated

            connect(dropdown, &QComboBox::currentTextChanged, this, &Properties::dropdownChanged);

            treeView->setIndexWidget(index, dropdown); //replace textbox with dropdown
        }

        for (int i = 0; i < toSelector.length(); i++)
        {
            QLineEdit *name = new QLineEdit(toSelector.at(i).second->text());
            name->setReadOnly(true);
            name->setMinimumSize(QSize(0, 0));
            name->setFrame(false);
            name->setMaximumHeight(int(name->sizeHint().height() * 0.9));
            //displays property

            QPushButton *button = new QPushButton("..");
            button->setObjectName(toSelector.at(i).first);
            button->setContentsMargins(0, 0, 0, 0);
            button->setMaximumHeight(int(button->sizeHint().height()*0.9));
            button->setMaximumWidth(int(button->sizeHint().width()*0.3));
            //opens pop-up to change property

            connect(button, &QPushButton::clicked, this, &Properties::selectorClicked);

            QWidget *container = new QWidget();
            QHBoxLayout *layout = new QHBoxLayout();
            layout->addWidget(name);
            layout->addWidget(button);
            layout->setContentsMargins(0, 0, 0, 0);
            layout->setSpacing(0);
            container->setLayout(layout);

            QModelIndex index = model->indexFromItem(toSelector.at(i).second);
            treeView->setIndexWidget(index, container);
        }

        connect(model, &QStandardItemModel::itemChanged, this, &Properties::updateData);
    }
}

void Properties::selectorClicked()
{
    QObject *sender = QObject::sender();
    QString name = sender->objectName();
    QDialog *dialog = new QDialog();

    QVBoxLayout *layout = new QVBoxLayout();
    dialog->setLayout(layout);

    //filter used to only display valid items
    QSortFilterProxyModel *filter = new QSortFilterProxyModel();
    filter->setFilterRole(Qt::UserRole + 1);
    QItemSelectionModel *selection = new QItemSelectionModel(filter);
    QTreeView *view = new QTreeView();
    view->setModel(filter);
    view->setSelectionModel(selection);
    layout->addWidget(view);


    if (name == "World")
    {
        filter->setSourceModel(selectionModel->model());
        filter->setFilterFixedString("World");
    }
    else if (name == "Property" || name == "Property2")
    {
        //properties can have types, e.g. a slider would use a float, a checkbox would use a bool
        QString type = item->reference->getProperty("TypeString");
        if (type.at(0) == "I")
            filter->setSourceModel(connector->inputs);
        else
            filter->setSourceModel(connector->outputs);
        filter->setRecursiveFilteringEnabled(true);
        if (type.at(1) != "A")
            filter->setFilterFixedString(type.at(1));
    }

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *select = new QPushButton(tr("Select"));
    QPushButton *cancel = new QPushButton(tr("Cancel"));
    buttonLayout->addWidget(select);
    buttonLayout->addWidget(cancel);
    connect(select, &QPushButton::clicked, dialog, &QDialog::accept);
    connect(cancel, &QPushButton::clicked, dialog, &QDialog::reject);
    layout->addLayout(buttonLayout);

    dialog->exec();
    //exec() blocks until the dialog has finished
    if (dialog->result() == 1 && !selection->selection().empty())
    {
        QModelIndex index = filter->mapToSource(selection->currentIndex());
        QString newText;
        if (name == "World")
        {
            item->reference->setProperty("World", index);
            ProjectTreeItem *item = static_cast<ProjectTreeItem *>(index.internalPointer());
            newText = item->data().toString();
        }
        else if (name == "Property" || name == "Property2")
        {
            ConnectionsItem *connectorItem = static_cast<ConnectionsItem *>(index.internalPointer());
            ProjectTreeItem *treeItem = connectorItem->reference;
            if (item == nullptr)
                return;
            if (name == "Property")
            {
                item->reference->setProperty("Item", QVariant::fromValue(treeItem));
                item->reference->setProperty("PropertyString", QVariant::fromValue(connectorItem->property));
                newText = item->reference->getProperty("Property");
            }
            else
            {
                item->reference->setProperty("Item2", QVariant::fromValue(treeItem));
                item->reference->setProperty("PropertyString2", QVariant::fromValue(connectorItem->property));
                newText = item->reference->getProperty("Property2");
            }
        }
        sender->parent()->findChild<QLineEdit *>()->setText(newText);
    }
}

void Properties::dropdownChanged(const QString &text)
{
    QObject *sender = QObject::sender();
    QString name = sender->objectName();
    bool worked = item->reference->setProperty(name, text);
    if (!worked)
    {
        QComboBox *box = static_cast<QComboBox *>(sender);
        box->setCurrentText(item->reference->getProperty(name));
    }
    else if (name == "Material")
    { //disable/enable elasticity and friction coefficient if change successful
        int row1 = propertyOrder.indexOf("Elasticity");
        int row2 = propertyOrder.indexOf("Friction Coefficient");

        QStandardItem *item1 = model->item(row1, 1);
        QStandardItem *item2 = model->item(row2, 1);
        if (text == "<Custom>")
        {
            item1->setEnabled(true);
            item2->setEnabled(true);
        }
        else
        {
            item1->setEnabled(false);
            item2->setEnabled(false);
        }
    }
}

void Properties::updateData(QStandardItem *changed)
{
    int type = changed->data(Qt::UserRole + 2).toInt();
    QString property = changed->data().toString();
    if (property.contains("{") || property.contains("}"))
        return;

    QString typeChar;
    if (bools.contains(property))
        typeChar = "B";
    else if (vectors.contains(property))
        typeChar = "V";
    else
        typeChar = "F";

    if (type == 1) //if property value
    {
        if (changed->isEditable())
        { //if not a bool
            bool worked = item->reference->setProperty(property, changed->text());
            if (!worked) //may have failed validation, e.g. e < 0
                changed->setText(item->reference->getProperty(property));
            else if (property.contains("."))
            {//if we changed a.y, then update a
                QString parentProperty = property.left(property.indexOf("."));
                int parentIndex = propertyOrder.indexOf(parentProperty);
                model->item(parentIndex, 1)->setText(item->reference->getProperty(parentProperty));
            }
            else
            {//if we updated a, then update a.y
                QStandardItem *parent = model->item(changed->row());
                if (parent->hasChildren())
                {
                    for (int i = 0; i < parent->rowCount(); i++)
                    {
                        QString childProperty = parent->child(i, 1)->data().toString();
                        QString temp = parent->child(i, 1)->text();
                        parent->child(i, 1)->setText(item->reference->getProperty(childProperty));
                    }
                }
            }
        }
        else
        {
            bool newValue = changed->checkState() == Qt::CheckState::Checked;
            item->reference->setProperty(property, newValue);
        }
    }
    else if (type == 2) //if input checkbox
    {
        if (changed->checkState() == Qt::CheckState::Checked)
            connector->addInput(item, property, typeChar);
        else
            connector->removeInput(item, property);
    }
    else //must be output checkbox
    {
        if (changed->checkState() == Qt::CheckState::Checked)
            connector->addOutput(item, property, typeChar);
        else
            connector->removeOutput(item, property);
    }
}
