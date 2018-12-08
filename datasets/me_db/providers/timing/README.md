# Timing.app

[Timing](https://timingapp.com/) is a macOS app for passive time tracking based
on a phone's geolocation. By 
sampling your phone's location periodically and grouping data into a series
of durations spent in different areas, it let's you label the areas with 
categories, obtaining pretty minimal effort passive time tracking. 

## Exported Data

Aggregating data from Timing can be performed automatically by reading the 
application's files. :

```
SELECT
  Task.title as series,
  CAST(ROUND(AppActivity.startDate * 1000.0) AS int) as date,
  CAST(ROUND((AppActivity.endDate - AppActivity.startDate) * 1000.0) AS int) as value,
  Application.title as `group`
FROM
  AppActivity
LEFT JOIN 
  Application
  ON AppActivity.applicationID=AppActivity.id
LEFT JOIN
  Task ON AppActivity.taskID=Task.id
LIMIT 10;
```


## Imported Data

The `SeriesCollection` proto generated by my `ProcessSeriesCollectionOrDie()`
functions contains one series for each `NAME`. The following schema is used:

```
Series.name                      ${Task.title}
Series.family                    "ScreenTime"
Series.unit                      "milliseconds"
Measurement.group                ${Application.title}
Measurement.ms_since_unix_epoch  ${AppActivity.startDate} in milliseconds (see below).
Measurement.value                (end - start) in milliseconds (see below).
Measurement.source               "Timing.app"
```

If the entry has a start date different from the end date, it is split into
multiple `Measurement`s, one per day. This is to ensure that summing the
measurements for a day is always <= 24 hours. For example, an entry has a start
date of `2017-01-01 18:00` and an end date of `2017-01-03 12:00` will produce
three measurements:

1.  `2017-01-01 18:00` - `2017-01-02 00:00`
2.  `2017-01-02 00:00` - `2017-01-03 00:00`
3.  `2017-01-03 00:00` - `2017-01-03 12:00`

The `NOTE` field is not exported.
