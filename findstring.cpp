#include "findstring.h"

FindString::FindString(QObject *parent) : QObject(parent)
{

}

bool FindString::matchPrefix(const QString &fullString, const QString &pattern)
{
//    return fullString.startsWith(pattern);
    return fullString.contains(pattern);
}
