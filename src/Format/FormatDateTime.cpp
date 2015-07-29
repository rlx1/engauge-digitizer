#include "EngaugeAssert.h"
#include "FormatDateTime.h"
#include "Logger.h"

FormatDateTime::FormatDateTime()
{
  // COORD_UNITS_DATE_SKIP and COORD_UNITS_TIME_SKIP allow date/time respectively even when skipped,
  // although there can be ambiguity with between COORD_UNITS_DATE_MONTH_DAY_YEAR and COORD_UNITS_DATE_DAY_MONTH_YEAR

  QStringList skip;

  skip << "";

  QStringList monthDayYear;
  QStringList dayMonthYear;
  QStringList yearMonthDay;

  monthDayYear << "MM/dd/yyyy" << "MM/dd/yy" 
               << "MM-dd-yyyy" << "MM-dd-yy" 
               << "MM/dd"
               << "MMM dd yyyy"
               << "MMM dd"
               << "MMMM dd yyyy"
               << "MMMM dd";
  dayMonthYear << "dd/MM/yyyy" << "dd/MM/yy"
               << "dd-MM-yyyy" << "dd-MM-yy"
               << "dd/MM"
               << "dd/MMM/yyyy" << "dd/MMM/yy"
               << "dd-MMM-yyyy" << "dd-MMM-yy"
               << "dd MMM yyyy" << "dd MMM yy"
               << "dd/MMMM/yyyy" << "dd/MMMM/yy"
               << "dd-MMMM-yyyy" << "dd-MMMM-yy"
               << "dd MMMM yyyy" << "dd MMMM yy";
  yearMonthDay << "yyyy/MM/dd" 
               << "yyyy-MM-dd"
               << "yyyy MM dd"
               << "yyyy/MMM/dd"
               << "yyyy-MMM-dd"
               << "yyyy MMM dd"
               << "yyyy/MMMM/dd"
               << "yyyy-MMMM-dd"
               << "yyyy MMMM dd";

  m_formatsDate [COORD_UNITS_DATE_SKIP] = skip + monthDayYear + yearMonthDay; // Day-month ambiguity gets treated as month/day
  m_formatsDate [COORD_UNITS_DATE_MONTH_DAY_YEAR] = skip + monthDayYear;
  m_formatsDate [COORD_UNITS_DATE_DAY_MONTH_YEAR] = skip + dayMonthYear;
  m_formatsDate [COORD_UNITS_DATE_YEAR_MONTH_DAY] = skip + yearMonthDay;

  ENGAUGE_ASSERT (m_formatsDate.count () == NUM_COORD_UNITS_DATE);

  QStringList hourMinute;
  QStringList hourMinuteSecond;
  QStringList hourMinutePm;
  QStringList hourMinuteSecondPm;

  hourMinute << "hh:mm";
  hourMinuteSecond << "HH:mm:ss";
  hourMinutePm << "hh:mmA"
               << "hh:mm A"
               << "hh:mma"
               << "hh:mm a";
  hourMinuteSecondPm << "hh:mm:ssA"
                     << "hh:mm:ss A"
                     << "hh:mm:ssa"
                     << "hh:mm:ss a";

  m_formatsTime [COORD_UNITS_TIME_SKIP] = skip + hourMinute + hourMinuteSecond + hourMinutePm + hourMinuteSecondPm;
  m_formatsTime [COORD_UNITS_TIME_HOUR_MINUTE] = skip + hourMinute + hourMinutePm;
  m_formatsTime [COORD_UNITS_TIME_HOUR_MINUTE_SECOND] = skip + hourMinuteSecond + hourMinuteSecondPm;

  ENGAUGE_ASSERT (m_formatsTime.count () == NUM_COORD_UNITS_TIME);
}

QDateTime FormatDateTime::parse (CoordUnitsDate coordUnitsDate,
                                 CoordUnitsTime coordUnitsTime,
                                 const QString &string) const
{
  LOG4CPP_INFO_S ((*mainCat)) << "FormatDateTime::parse"
                              << " date=" << coordUnitsDateToString (coordUnitsDate).toLatin1().data()
                              << " time=" << coordUnitsTimeToString (coordUnitsTime).toLatin1().data()
                              << " string=" << string.toLatin1().data();

  ENGAUGE_ASSERT (m_formatsDate.contains (coordUnitsDate));
  ENGAUGE_ASSERT (m_formatsTime.contains (coordUnitsTime));

  QStringList formatsDate = m_formatsDate [coordUnitsDate];
  QStringList formatsTime = m_formatsTime [coordUnitsTime];

  // Loop through the legal date/time combinations
  QStringList::const_iterator itrDate, itrTime;
  for (itrDate = formatsDate.begin(); itrDate != formatsDate.end(); itrDate++) {

    QString formatDate = *itrDate;

    for (itrTime = formatsTime.begin(); itrTime != formatsTime.end(); itrTime++) {

      QString formatTime = *itrTime;

      QString formatDateTime = formatDate + " " + formatTime;
      formatDateTime = formatDateTime.trimmed (); // Remove leading/trailing blanks from skipped date or time

      if (!formatDateTime.isEmpty()) {

        // Try parsing according to the current format
        QDateTime dt = QDateTime::fromString (string,
                                              formatDateTime);

        LOG4CPP_INFO_S ((*mainCat)) << "FormatDateTime::parse"
                                    << " format=" << formatDateTime.toLatin1().data()
                                    << " isValid=" << (dt.isValid() ? "true" : "false");
        
        if (dt.isValid()) {
          return dt;
        }
      }
    }
  }

  return QDateTime(); // Invalid value
}
