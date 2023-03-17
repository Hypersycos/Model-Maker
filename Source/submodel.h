#ifndef SUBMODEL_H
#define SUBMODEL_H

#include "model.h"

class SubModel : public Model
{
public:
    SubModel();
    QStringList getValidChildren() override;
    static std::pair<bool, Item *> load(QTextStream &in);

    Vector2 reaction;
    Vector2 friction;
};

#endif // SUBMODEL_H
