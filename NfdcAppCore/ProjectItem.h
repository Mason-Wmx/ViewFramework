#pragma once


namespace SIM
{
    // Structure to hold cache for projects
    struct ProjectItem
    {
        QString name;
        QString id;
        QDateTime createTime;
        QDateTime modifiedTime;
        // REMEMBER TO ADD MEMBER IN operator<< and operator>> !!!
        // QDataStream &operator<<(QDataStream &out, const ProjectItem &myObj)
        // QDataStream &operator>>(QDataStream &in, ProjectItem &myObj)


        //operator== needed for QList comparision
        bool operator==(const ProjectItem & other) const
        {
            if (!(id == other.id))
                return false;
            if (!(name == other.name))
                return false;
            if (!(createTime == other.createTime))
                return false;
            if (!(modifiedTime == other.modifiedTime))
                return false;

            return true;
        }
    };
}

QDataStream &operator<<(QDataStream &out, const SIM::ProjectItem &myObj);
QDataStream &operator >> (QDataStream &in, SIM::ProjectItem &myObj);

QDataStream &operator<<(QDataStream &out, const std::string &myObj);
QDataStream &operator >> (QDataStream &in, std::string &myObj);
