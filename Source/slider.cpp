#include "slider.h"
#include <QLayout>

Slider::Slider() : ScreenItem()
{
    validProperties << "Minimum"
                    << "Maximum"
                    << "Vertical"
                    << "Interval"
                    << "Label";

    doubles << "Minimum"
            << "Maximum"
            << "Interval";

    name = "Slider";
    type = "Slider";

    //QSliders only support integers
    minimum = 0;
    maximum = 1;
    textSize = 11;
    stepping = 0.01;
    vertical = false;

    textLabel = "";

    widget = new QWidget();

    slider = new QSlider(Qt::Orientation::Horizontal);
    slider->setValue(0);
    slider->setMinimum(0);
    slider->setMaximum(100);
    label = new QLabel("0");

    connect(slider, &QSlider::valueChanged, this, &Slider::updated);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(label);
    layout->addWidget(slider);
    widget->setLayout(layout);
}

void Slider::updated(int value)
{
    double realValue = value*stepping;
    //converts integer on slider to float value
    label->setText(textLabel + QString::number(realValue));

    if (connected)
        connected->reference->setProperty(property, QVariant::fromValue(QString::number(realValue)));
}

QString Slider::getProperty(QString property)
{
    QString toReturn = ScreenItem::getProperty(property);
    if (toReturn != "Invalid")
        return toReturn;
    else if (property == "Minimum")
        return QString::number(minimum);
    else if (property == "Maximum")
        return QString::number(maximum);
    else if (property == "Vertical")
    {
        if (vertical)
            return "true";
        else
            return "false";
    }
    else if (property == "Interval")
        return QString::number(stepping);
    else if (property == "Label")
        return textLabel;
    else
        return "Invalid";
}

bool Slider::setProperty(QString property, QVariant value)
{
    bool toReturn = ScreenItem::setProperty(property, value);
    if (toReturn)
        return true;
    //ScreenItem deals with inherited properties

    bool validation;
    if (doubles.contains(name))
    {
        value.toDouble(&validation);
        if (!validation)
            return false;
    }

    if (property == "Minimum")
    {
        minimum = value.toDouble();
        slider->setMinimum(int(minimum/stepping));
    }
    else if (property == "Maximum")
    {
        maximum = value.toDouble();
        slider->setMaximum(int(maximum/stepping));
    }
    else if (property == "Vertical")
    {
        if (value.toBool())
        {
            slider->setOrientation(Qt::Orientation::Vertical);
            QVBoxLayout *layout = new QVBoxLayout();
            layout->addWidget(label);
            layout->addWidget(slider);
            vertical = true;
            //have to delete current layout before re-setting
            delete widget->layout();
            widget->setLayout(layout);
        }
        else
        {
            slider->setOrientation(Qt::Orientation::Horizontal);
            QHBoxLayout *layout = new QHBoxLayout();
            layout->addWidget(label);
            layout->addWidget(slider);
            vertical = false;
            delete widget->layout();
            widget->setLayout(layout);
        }
    }
    else if (property == "Interval")
    {
        double old = slider->value()*stepping;
        stepping = value.toDouble();
        slider->setMinimum(int(minimum/stepping));
        slider->setMaximum(int(maximum/stepping));
        slider->setValue(int(old/stepping));
    }
    else if (property == "Label")
    {
        textLabel = value.toString();
        updated(slider->value());
    }
    else
        return false;
    return true;
}

std::pair<bool, Item *> Slider::load(QTextStream &in)
{
    Slider *newSlider = new Slider();
    if (!newSlider->setAllProperties(getPropertiesFromFile(in))
            || in.readLine() != "}")
        return std::pair<bool, Item *>(false, nullptr);
    return std::pair<bool, Item *>(true, newSlider);
}
