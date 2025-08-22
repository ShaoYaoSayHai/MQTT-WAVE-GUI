#ifndef FINDSTRING_H
#define FINDSTRING_H

#include <QObject>

class FindString : public QObject
{
    Q_OBJECT
public:
    explicit FindString(QObject *parent = nullptr);

    bool matchPrefix(const QString& fullString, const QString& pattern) ;

signals:

};

#endif // FINDSTRING_H
