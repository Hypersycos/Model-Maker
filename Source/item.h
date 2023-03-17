#ifndef ITEM_H
#define ITEM_H

#include <QString>
#include <QVariant>
#include <tuple>
#include <QModelIndex>
#include <QIcon>
#include <QFileIconProvider>
#include <QTextStream>

class Item : public QObject
{
    Q_OBJECT
public:
    Item();
    virtual ~Item();
    QList<std::pair<QString, QString>> getAllProperties();
    bool setAllProperties(QStringList values);
    //virtual functions to allow polymorphic behaviour
    virtual QString getProperty(QString name);
    virtual bool setProperty(QString name, QVariant value);
    virtual QList<Item *> getChildren();
    virtual QStringList getValidChildren();
    virtual void addChild(Item *newChild, int position = -1);
    virtual void removeChild(Item *child);
    virtual QStringList save();
    static QStringList save(Item *item);
    static std::pair<bool, Item *> load(QTextStream &in);
    static QStringList getPropertiesFromFile(QTextStream &in);
    QIcon icon;

protected:
    QStringList doubles;
    QStringList ints;
    QString name;
    QString type;
    QStringList validProperties;
    QModelIndex index;
};

#endif // ITEM_H
