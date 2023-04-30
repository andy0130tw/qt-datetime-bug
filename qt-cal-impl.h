#pragma once

#include <QString>
#include <QDateTime>
#include <QLocale>

class myQtCalBE;

class MyQLocale : public QLocale {
    friend myQtCalBE;
public:
    QString toString(QTime time, QStringView format) const;
private:
    auto getD();
};


// from QCalendar::dateTimeToString, then QCalendarBackend in qLocale
class myQtCalBE {
public:
    QString myDateTimeToString(QStringView format, const QDateTime &datetime,
                                    QDate dateOnly, QTime timeOnly,
                                    const MyQLocale &locale);
};
