#include "stdafx.h"
#include "ProjectItem.h"




//Operators needed to serialize _projectsCache
QDataStream &operator<<(QDataStream &out, const SIM::ProjectItem &myObj)
{
    out << myObj.name;
    out << myObj.id;
    out << myObj.createTime;
    out << myObj.modifiedTime;
    return out;
}
QDataStream &operator >> (QDataStream &in, SIM::ProjectItem &myObj)
{
    in >> myObj.name;
    in >> myObj.id;
    in >> myObj.createTime;
    in >> myObj.modifiedTime;
    return in;
}

QDataStream &operator<<(QDataStream &out, const std::string &myObj)
{
    out << QString::fromStdString(myObj);
    return out;
}
QDataStream &operator >> (QDataStream &in, std::string &myObj)
{
    QString tmp;
    in >> tmp;
    myObj = tmp.toStdString();
    return in;
}

