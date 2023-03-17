#include "world.h"
#include "submodel.h"

World::World() : Item()
{
    validProperties << "Gravity Enabled"
                    << "g"
                    << "Size"
                    << "Size.X"
                    << "Size.Y"
                    << "Time Multiplier"
                    << "Time";

    doubles << "g"
            << "Size.X"
            << "Size.Y"
            << "Time Multiplier"
            << "Time";

    g = 9.81;
    doGravity = true;
    speed = 1;
    time = 0;

    QFileIconProvider *temp = new QFileIconProvider();
    icon = temp->icon(temp->Network);

    type = "World";
    name = "World";
}

QString World::getProperty(QString name)
{
    if (name == "Gravity Enabled")
    {
        if (doGravity)
            return "true";
        else
            return "false";
    }
    else if (name == "g")
    {
        return QString::number(g);
    }
    else if (name == "Size")
    {
        return "(" + QString::number(hSize) + ", " + QString::number(vSize) + ")";
    }
    else if (name == "Size.X")
    {
        return QString::number(hSize);
    }
    else if (name == "Size.Y")
    {
        return QString::number(vSize);
    }
    else if (name == "Time Multiplier")
    {
        return QString::number(speed);
    }
    else if (name == "Time")
    {
        return QString::number(time);
    }
    else if (name == "Name")
    {
        return this->name;
    }
    else if (name == "Type")
    {
        return "World";
    }
    else
    {
        return "Invalid";
    }
}


bool World::setProperty(QString name, QVariant value)
{
    bool validation;
    if (doubles.contains(name))
    {
        value.toDouble(&validation);
        if (!validation)
            return false;
    }

    if (name == "Gravity Enabled")
    {
        doGravity = value.toBool();
    }
    else if (name == "g")
    {
        g = value.toDouble();
    }
    else if (name == "Size")
    {
        QString temp = value.toString();

        if (!temp.contains(","))
            return false;
        temp = temp.remove("(");
        temp = temp.remove(")");
        temp = temp.remove(" ");
        QStringList tempList = temp.split(",");

        bool valid1;
        bool valid2;

        double first = tempList.first().toDouble(&valid1);
        double second = tempList.last().toDouble(&valid2);

        if (valid1 && valid2)
        {
            hSize = first;
            vSize = second;
        }
        else
            return false;
    }
    else if (name == "Size.X")
    {
        vSize = value.toDouble();
    }
    else if (name == "Size.Y")
    {
        hSize = value.toDouble();
    }
    else if (name == "Time Multiplier")
    {
        speed = value.toDouble();
    }
    else if (name == "Name")
    {
        this->name = value.toString();
    }
    else
    {
        return false;
    }
    return true;
}

void World::addChild(Item *newChild, int position)
{
    if (position == -1)
        position = models.length();
    models.insert(position, static_cast<Model *>(newChild));
}

void World::removeChild(Item *child)
{
    models.removeOne(static_cast<Model *>(child));
}

QList<Item *> World::getChildren()
{
    QList<Item *> toReturn;
    for (int i = 0; i < models.length(); i++)
    {
        toReturn.append(models.at(i));
    }
    return toReturn;
}

QStringList World::getValidChildren()
{
    return QStringList("Model");
}

std::pair<bool, Item *> World::load(QTextStream &in)
{
    World *newWorld = new World();
    if (!newWorld->setAllProperties(getPropertiesFromFile(in)))
        return std::pair<bool, Item *>(false, nullptr);
    QString temp = in.readLine();
    while (temp.contains("{"))
    {
        std::pair<bool, Item *> child = Model::load(in);
        if (!child.first)
            return std::pair<bool, Item *>(false, nullptr);
        newWorld->addChild(child.second);
        temp = in.readLine();
    }
    if (temp != "}")
        return std::pair<bool, Item *>(false, nullptr);
    return std::pair<bool, Item *>(true, newWorld);
}

QStringList World::save()
{
    QStringList toReturn = Item::save(this);
    for (int i = 0; i < models.length(); i++)
    {
        QStringList child = models.at(i)->save();
        toReturn += child;
    }
    toReturn << "}";
    return toReturn;
}

void World::startRun()
{
    //save all relevant properties for resetting when going back to edit mode
    time = 0;
    for (int i = 0; i < models.length(); i++)
    {
        Model *model = models.at(i);
        storedValues << model->getProperty("Position")
                     << model->getProperty("Rotation")
                     << model->getProperty("Velocity")
                     << model->getProperty("Angular Velocity");
        for (int j = 0; j < model->subModels.length(); j++)
        {
            SubModel *subModel = model->subModels.at(j);
            storedValues << subModel->getProperty("Position")
                         << subModel->getProperty("Velocity");
        }
    }
}

void World::stopRun()
{
    //reset properties
    QStringList properties = {"Position", "Rotation", "Velocity", "Angular Velocity"};
    int index = 0;
    for (int i = 0; i < models.length(); i++)
    {
        Model *model = models.at(i);
        for (int j = 0; j < 4; j++)
        {
            model->setProperty(properties.at(j), QVariant::fromValue(storedValues.at(index)));
            index++;
        }
        for (int j = 0; j < model->subModels.length(); j++)
        {
            SubModel *subModel = model->subModels.at(j);
            for (int k = 0; k < 3; k = k + 2)
            {
                subModel->setProperty(properties.at(k), QVariant::fromValue(storedValues.at(index)));
                index++;
            }
        }
    }
    storedValues = QStringList();
}

void World::unpause()
{
    lastUpdate = steady_clock::now();
}

void World::physicsUpdate()
{
    time_point<steady_clock> now = steady_clock::now();
    duration<double> difference = now - lastUpdate;
    double delta_t = difference.count() * speed;
    time += delta_t;
    for (int i = 0; i < models.length(); i++)
    {
        Model *model = models.at(i);
        if (!model->anchored && model->mass != 0.0)
        {
            //motion in the y direction
            double a = 0;
            if (doGravity)
            { //gravity always accelerates at g
                a -= g;
                for (int j = 0; j < model->subModels.length(); j++)
                { //apply submodel's weights
                    SubModel *subModel = model->subModels.at(j);
                    a -= g * (subModel->mass / model->mass);
                }
            }
            for (int j = 0; j < model->forces.length(); j++)
            {
                a += model->forces.at(j)->force.rotate(model->rotation).y / model->mass;
            }
            //s = ut + 1/2at^2
            double distance = model->velocity.y * delta_t + 0.5*a*(pow(delta_t, 2));
            //v = u + at
            double velocity = model->velocity.y + a * delta_t;
            model->velocity.y = velocity;
            model->position.y = model->position.y + distance;
            model->acceleration.y = a;

            //motion in the x direction
            a = 0;
            for (int j = 0; j < model->forces.length(); j++)
            {
                a += model->forces.at(j)->force.rotate(model->rotation).x / model->mass;
            }
            distance = model->velocity.x * delta_t + 0.5*a*(pow(delta_t, 2));
            velocity = model->velocity.x + a * delta_t;
            model->velocity.x = velocity;
            model->position.x = model->position.x + distance;
            model->acceleration.x = a;

            model->GPE = model->mass * g * model->position.y;
            model->KE = 0.5 * model->mass * model->velocity.magnitudeSquared();

            double moments = 0;
            for (int j = 0; j < model->forces.length(); j++)
            { // moment = F*d
                Force *force = model->forces.at(j);
                moments += force->force.y * (force->position.x - model->CoM.x) * model->scale.x;
                moments -= force->force.x * (force->position.y - model->CoM.y) * model->scale.y;
            }

            if (doGravity)
            {
                for (int j = 0; j < model->subModels.length(); j++)
                { //submodel's weights can also affect moments
                    SubModel *subModel = model->subModels.at(j);
                    moments -= g * subModel->mass * ((subModel->position.x + subModel->CoM.x) * subModel->scale.x - model->CoM.x) * model->scale.x;
                }
            }
            moments /= model->mass;

            model->rotation += model->angularVelocity*delta_t + 0.5*moments*pow(delta_t,2);
            model->angularVelocity += moments*delta_t;
        }
        else
        { //we assume they're always in contact and parallel
            for (int j = 0; j < model->subModels.length(); j++)
            {
                SubModel *subModel = model->subModels.at(j);

                Vector2 resultantForce = Vector2();

                //x and y is parallel and perpendicular to the parent model, rather than compared to absolute x and y.
                for (int k = 0; k < subModel->forces.length(); k++)
                {
                    resultantForce.x += subModel->forces.at(k)->force.x;
                    resultantForce.y += subModel->forces.at(k)->force.y;
                }

                if (doGravity)
                {
                    Vector2 gravity = Vector2(0, -subModel->mass * g).rotate(-model->rotation);
                    resultantForce.y += gravity.y;
                    resultantForce.x += gravity.x;
                }

                //reaction will oppose all downward force.
                double reaction = 0;
                if (resultantForce.y < 0)
                {
                    reaction = -resultantForce.y;
                    resultantForce.y = 0;
                }

                subModel->reaction = Vector2(0, reaction);
                double fMax = reaction * subModel->frictionCoefficient;
                bool checkForRest = false;

                if (subModel->velocity.x == 0.0)
                {
                    if (resultantForce.x < fMax)
                    {
                        if (resultantForce.x > -fMax)
                        { //if resultant.x is < |fMax|, then there is no motion.
                            subModel->friction = Vector2(-resultantForce.x, 0);
                            resultantForce.x = 0;
                        }
                        else
                        { //otherwise friction is fMax and opposes motion
                            subModel->friction = Vector2(fMax, 0);
                            resultantForce.x += fMax;
                        }
                    }
                    else
                    {
                        subModel->friction = Vector2(-fMax, 0);
                        resultantForce.x -= fMax;
                    }
                }
                else if (subModel->velocity.x > 0)
                { //friction is -fmax if travelling in +x
                    checkForRest = true;
                    subModel->friction = Vector2(-fMax, 0);
                    resultantForce.x -= fMax;
                }
                else
                { //friction is fmax if travelling in -x
                    checkForRest = true;
                    subModel->friction = Vector2(fMax, 0);
                    resultantForce.x += fMax;
                }

                double a = resultantForce.x / subModel->mass;
                double distance;
                double velocity;

                // v = u + at
                // 0 = velocity.x + a * t
                // t = -velocity.x / a
                double time;
                if (a == 0.0)
                    time = -1;
                else
                    time = -subModel->velocity.x / a;
                if (time < delta_t && time > 0 && checkForRest)
                { //if friction is overpowering motion, then check to see if the object comes to rest. If it does, then only move the object as far as it should.
                    distance = subModel->velocity.x * time + 0.5*a*(pow(time, 2));
                    velocity = 0;
                }
                else
                {
                    distance = subModel->velocity.x * delta_t + 0.5*a*(pow(delta_t, 2));
                    velocity = subModel->velocity.x + a * delta_t;
                }
                subModel->velocity.x = velocity;
                subModel->position.x = subModel->position.x + distance;
                subModel->acceleration.x = a;

                a = resultantForce.y / subModel->mass;
                distance = subModel->velocity.y * delta_t + 0.5*a*(pow(delta_t, 2));
                velocity = subModel->velocity.y + a * delta_t;
                subModel->velocity.y = velocity;
                subModel->position.y = subModel->position.y + distance;
                subModel->acceleration.y = a;

                subModel->GPE = subModel->mass * g * subModel->position.rotate(model->rotation).y;
                subModel->KE = 0.5 * subModel->mass * subModel->velocity.magnitudeSquared();
            }
        }

    }
    lastUpdate = now;
}
