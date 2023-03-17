#ifndef SLIDER_H
#define SLIDER_H

#include "screenitem.h"
#include <QSlider>
#include <QLabel>

class Slider : public ScreenItem
{
public:
    Slider();
    QString getProperty(QString property) override;
    bool setProperty(QString name, QVariant value) override;
    static std::pair<bool, Item *> load(QTextStream &in);

private:
    void updated(int value);

    QString textLabel;
    double minimum;
    double maximum;
    double stepping;
    QSlider *slider;
    QLabel *label;
    int textSize;
    bool vertical;
};

#endif // SLIDER_H
