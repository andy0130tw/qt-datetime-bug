#include <QString>
#include <QDateTime>
#include <QLocale>
#include "qt-cal-impl.h"

auto MyQLocale::getD() {
    return this->d;
}

// from QCalendar::dateTimeToString, then QCalendarBackend in qLocale
QString myQtCalBE::myDateTimeToString(QStringView format, const QDateTime &datetime,
                                      QDate dateOnly, QTime timeOnly,
                                      const MyQLocale &locale) {
    QDate date;
    QTime time;
    bool formatDate = false;
    bool formatTime = false;
    if (datetime.isValid()) {
        date = datetime.date();
        time = datetime.time();
        formatDate = true;
        formatTime = true;
    } else if (dateOnly.isValid()) {
        date = dateOnly;
        formatDate = true;
    } else if (timeOnly.isValid()) {
        time = timeOnly;
        formatTime = true;
    } else {
        return QString();
    }

    QString result;
    int year = 0, month = 0, day = 0;
    if (formatDate) {
        const auto parts = julianDayToDate(date.toJulianDay());
        if (!parts.isValid())
            return QString();
        year = parts.year;
        month = parts.month;
        day = parts.day;
    }

    qsizetype i = 0;
    while (i < format.size()) {
        if (format.at(i).unicode() == '\'') {
            result.append(qt_readEscapedFormatString(format, &i));
            continue;
        }

        const QChar c = format.at(i);
        qsizetype repeat = qt_repeatCount(format.mid(i));
        bool used = false;
        if (formatDate) {
            switch (c.unicode()) {
            case 'y':
                used = true;
                if (repeat >= 4)
                    repeat = 4;
                else if (repeat >= 2)
                    repeat = 2;

                switch (repeat) {
                case 4: {
                    const int len = (year < 0) ? 5 : 4;
                    result.append(locale.getD()->m_data->longLongToString(year, -1, 10, len,
                                                                     QLocaleData::ZeroPadded));
                    break;
                }
                case 2:
                    result.append(locale.getD()->m_data->longLongToString(year % 100, -1, 10, 2,
                                                                     QLocaleData::ZeroPadded));
                    break;
                default:
                    repeat = 1;
                    result.append(c);
                    break;
                }
                break;

            case 'M':
                used = true;
                repeat = qMin(repeat, 4);
                switch (repeat) {
                case 1:
                    result.append(locale.getD()->m_data->longLongToString(month));
                    break;
                case 2:
                    result.append(locale.getD()->m_data->longLongToString(month, -1, 10, 2,
                                                                     QLocaleData::ZeroPadded));
                    break;
                case 3:
                    result.append(monthName(locale, month, year, QLocale::ShortFormat));
                    break;
                case 4:
                    result.append(monthName(locale, month, year, QLocale::LongFormat));
                    break;
                }
                break;

            case 'd':
                used = true;
                repeat = qMin(repeat, 4);
                switch (repeat) {
                case 1:
                    result.append(locale.getD()->m_data->longLongToString(day));
                    break;
                case 2:
                    result.append(locale.getD()->m_data->longLongToString(day, -1, 10, 2,
                                                                     QLocaleData::ZeroPadded));
                    break;
                case 3:
                    result.append(locale.dayName(
                                      dayOfWeek(date.toJulianDay()), QLocale::ShortFormat));
                    break;
                case 4:
                    result.append(locale.dayName(
                                      dayOfWeek(date.toJulianDay()), QLocale::LongFormat));
                    break;
                }
                break;

            default:
                break;
            }
        }
        if (!used && formatTime) {
            switch (c.unicode()) {
            case 'h': {
                used = true;
                repeat = qMin(repeat, 2);
                int hour = time.hour();
                if (timeFormatContainsAP(format)) {
                    if (hour > 12)
                        hour -= 12;
                    else if (hour == 0)
                        hour = 12;
                }

                switch (repeat) {
                case 1:
                    result.append(locale.getD()->m_data->longLongToString(hour));
                    break;
                case 2:
                    result.append(locale.getD()->m_data->longLongToString(hour, -1, 10, 2,
                                                                     QLocaleData::ZeroPadded));
                    break;
                }
                break;
            }
            case 'H':
                used = true;
                repeat = qMin(repeat, 2);
                switch (repeat) {
                case 1:
                    result.append(locale.getD()->m_data->longLongToString(time.hour()));
                    break;
                case 2:
                    result.append(locale.getD()->m_data->longLongToString(time.hour(), -1, 10, 2,
                                                                     QLocaleData::ZeroPadded));
                    break;
                }
                break;

            case 'm':
                used = true;
                repeat = qMin(repeat, 2);
                switch (repeat) {
                case 1:
                    result.append(locale.getD()->m_data->longLongToString(time.minute()));
                    break;
                case 2:
                    result.append(locale.getD()->m_data->longLongToString(time.minute(), -1, 10, 2,
                                                                     QLocaleData::ZeroPadded));
                    break;
                }
                break;

            case 's':
                used = true;
                repeat = qMin(repeat, 2);
                switch (repeat) {
                case 1:
                    result.append(locale.getD()->m_data->longLongToString(time.second()));
                    break;
                case 2:
                    result.append(locale.getD()->m_data->longLongToString(time.second(), -1, 10, 2,
                                                                     QLocaleData::ZeroPadded));
                    break;
                }
                break;

            case 'A':
            case 'a': {
                QString text = time.hour() < 12 ? locale.amText() : locale.pmText();
                used = true;
                repeat = 1;
                if (format.mid(i + 1).startsWith(u'p', Qt::CaseInsensitive))
                    ++repeat;
                if (c.unicode() == 'A' && (repeat == 1 || format.at(i + 1).unicode() == 'P'))
                    text = std::move(text).toUpper();
                else if (c.unicode() == 'a' && (repeat == 1 || format.at(i + 1).unicode() == 'p'))
                    text = std::move(text).toLower();
                // else 'Ap' or 'aP' => use CLDR text verbatim, preserving case
                result.append(text);
                break;
            }

            case 'z':
                used = true;
                repeat = (repeat >= 3) ? 3 : 1;

                // note: the millisecond component is treated like the decimal part of the seconds
                // so ms == 2 is always printed as "002", but ms == 200 can be either "2" or "200"
                result.append(locale.getD()->m_data->longLongToString(time.msec(), -1, 10, 3,
                                                                 QLocaleData::ZeroPadded));
                if (repeat == 1) {
                    if (result.endsWith(locale.zeroDigit()))
                        result.chop(1);
                    if (result.endsWith(locale.zeroDigit()))
                        result.chop(1);
                }
                break;

            case 't':
                used = true;
                repeat = 1;
                // If we have a QDateTime use the time spec otherwise use the current system tzname
                result.append(formatDate ? datetime.timeZoneAbbreviation()
                                         : QDateTime::currentDateTime().timeZoneAbbreviation());
                break;

            default:
                break;
            }
        }
        if (!used)
            result.resize(result.size() + repeat, c);
        i += repeat;
    }

    return result;
}
