#include <QTextStream>
#include <QDateTime>
#include <QLocale>
#include <QCalendar>
// #include "qt-cal-impl.h"

QTextStream qStdout(stdout);

/* auto MyQLocale::toString(QTime time, QStringView format) const -> QString {
    return myQtCalBE().myDateTimeToString(format, QDateTime(), QDate(), time, *this);
} */

int main() {
    const auto now = QTime::currentTime();
    const auto myloc = QLocale(QLocale::Chinese, QLocale::TraditionalChineseScript);
    const auto tf = myloc.timeFormat(QLocale::ShortFormat);
    // const auto cusloc = MyQLocale();

    qStdout << "--- Faulty impl for " << myloc.bcp47Name() << Qt::endl
            << "Time format: " << tf << Qt::endl
            << "Result: " << myloc.toString(now, tf) << Qt::endl;

    return 0;
}
