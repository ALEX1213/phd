#include "util/me/life_cycle/life_cycle_lib.h"

#include "util/me/me.pb.h"

#include "phd/string.h"
#include "phd/test.h"

namespace me {
namespace {

// The number of milliseconds in a day.
constexpr int64_t MILLISECONDS_IN_DAY =
    /*second=*/1000 * /*hour=*/3600 * /*day=*/24;

TEST(ParseLifeCycleDatetimeOrDie, UnixEpoch) {
  EXPECT_EQ(ParseLifeCycleDatetimeOrDie("1970-01-01 00:00:00"),
               absl::UnixEpoch());
}

TEST(ParseLifeCycleDatetimeOrDie, InvalidDate) {
  ASSERT_DEATH(ParseLifeCycleDatetimeOrDie("This will crash"),
               "Failed to parse 'This will crash'");
}

TEST(ToMillisecondsSinceUnixEpoch, UnixEpoch) {
  EXPECT_EQ(ToMillisecondsSinceUnixEpoch(absl::UnixEpoch()), 0);
}

TEST(ParseDateToMilliseconds, UnixEpoch) {
  EXPECT_EQ(ToMillisecondsSinceUnixEpoch(
      ParseLifeCycleDatetimeOrDie("1970-01-01 00:00:00")), 0);
}

TEST(ParseDateToMilliseconds, DateOnly) {
  // Testing with a known date.
  EXPECT_EQ(ToMillisecondsSinceUnixEpoch(
      ParseLifeCycleDatetimeOrDie("2017-01-01 00:00:00")), 1483228800000);
}

TEST(ParseDateToMilliseconds, DateAndHour) {
  // Testing with a known date.
  EXPECT_EQ(ToMillisecondsSinceUnixEpoch(
      ParseLifeCycleDatetimeOrDie("2017-01-01 01:00:00")), 1483232400000);
}

TEST(ParseDateToMilliseconds, DateAndHourAndMinute) {
  // Testing with a known date.
  EXPECT_EQ(ToMillisecondsSinceUnixEpoch(
      ParseLifeCycleDatetimeOrDie("2017-01-01 01:01:00")), 1483232460000);
}

TEST(ParseDateToMilliseconds, DateAndHourAndMinuteAndSecond) {
  // Testing with a known date.
  EXPECT_EQ(ToMillisecondsSinceUnixEpoch(
      ParseLifeCycleDatetimeOrDie("2017-01-01 01:01:01")), 1483232461000);
}

TEST(AddMeasurementsFromDurationOrDie, NegativeTime) {
  Series series;
  AddMeasurementsFromDurationOrDie(0, -1000, "location", &series);
  EXPECT_EQ(series.measurement_size(), 0);
}

TEST(AddMeasurementsFromDurationOrDie, GroupField) {
  Series series;
  AddMeasurementsFromDurationOrDie(0, 1000, "location", &series);
  ASSERT_EQ(series.measurement_size(), 1);
  EXPECT_EQ(series.measurement(0).group(), "location");
}

TEST(AddMeasurementsFromDurationOrDie, SourceField) {
  Series series;
  AddMeasurementsFromDurationOrDie(0, 1000, "location", &series);
  ASSERT_EQ(series.measurement_size(), 1);
  EXPECT_EQ(series.measurement(0).source(), "LifeCycle");
}

TEST(AddMeasurementsFromDurationOrDie, ValueField) {
  Series series;
  AddMeasurementsFromDurationOrDie(0, 1000, "location", &series);
  ASSERT_EQ(series.measurement_size(), 1);
  EXPECT_EQ(series.measurement(0).value(), 1000);
}

TEST(AddMeasurementsFromDurationOrDie, MillisecondsInDayMinusOne) {
  Series series;
  AddMeasurementsFromDurationOrDie(
      0, MILLISECONDS_IN_DAY - 1, "location", &series);
  ASSERT_EQ(series.measurement_size(), 1);
  EXPECT_EQ(series.measurement(0).value(), MILLISECONDS_IN_DAY - 1);
}

TEST(AddMeasurementsFromDurationOrDie, MillisecondsInDay) {
  Series series;
  AddMeasurementsFromDurationOrDie(
      0, MILLISECONDS_IN_DAY, "location", &series);
  ASSERT_EQ(series.measurement_size(), 1);
  EXPECT_EQ(series.measurement(0).value(), MILLISECONDS_IN_DAY);
}

TEST(AddMeasurementsFromDurationOrDie, MillisecondsInDayPlusOne) {
  Series series;
  AddMeasurementsFromDurationOrDie(
      0, MILLISECONDS_IN_DAY + 1, "location", &series);
  ASSERT_EQ(series.measurement_size(), 2);
  EXPECT_EQ(series.measurement(0).value(), MILLISECONDS_IN_DAY);
  EXPECT_EQ(series.measurement(1).value(), 1);
}

TEST(LocationToGroup, StringReturned) {
  EXPECT_EQ(LocationToGroup("Location"), "Location");
}

TEST(LocationToGroup, LeftWhitespaceTrimmed) {
  EXPECT_EQ(LocationToGroup(" Location"), "Location");
}

TEST(LocationToGroup, ConvertToCamelCase) {
  EXPECT_EQ(LocationToGroup("location"), "Location");
}

TEST(LocationToGroup, JoinSpaces) {
  EXPECT_EQ(LocationToGroup("Hello World"), "HelloWorld");
}

TEST(LocationToGroup, JoinSpacesLowercase) {
  EXPECT_EQ(LocationToGroup("hello world"), "HelloWorld");
}

TEST(LocationToGroup, DefaultValue) {
  EXPECT_EQ(LocationToGroup(""), "default");
}

TEST(LocationToGroup, DefaultValueWhitespace) {
  EXPECT_EQ(LocationToGroup("  "), "default");
}

TEST(ProcessLineOrDie, MapSize) {
  string line = "1970-01-01 00:00:00, 1970-01-01 00:00:01, "
                "unused_start_time, unused_end_time, unused_duration, "
                "series, location, unused_note";
  SeriesCollection series_collection;
  absl::flat_hash_map<string, Series*> name_to_series_map;
  ProcessLineOrDie(line, /*line_num=*/0, boost::filesystem::path("."),
                   &series_collection, &name_to_series_map);

  ASSERT_EQ(name_to_series_map.size(), 1);
}

TEST(ProcessLineOrDie, SeriesCollectionSize) {
  string line = "1970-01-01 00:00:00, 1970-01-01 00:00:01, "
                "unused_start_time, unused_end_time, unused_duration, "
                "series, location, unused_note";
  SeriesCollection series_collection;
  absl::flat_hash_map<string, Series*> name_to_series_map;
  ProcessLineOrDie(line, /*line_num=*/0, boost::filesystem::path("."),
                   &series_collection, &name_to_series_map);

  ASSERT_EQ(series_collection.series_size(), 1);
}

TEST(ProcessLineOrDie, SeriesField) {
  string line = "1970-01-01 00:00:00, 1970-01-01 00:00:01, "
                "unused_start_time, unused_end_time, unused_duration, "
                "series, location, unused_note";
  SeriesCollection series_collection;
  absl::flat_hash_map<string, Series*> name_to_series_map;
  ProcessLineOrDie(line, /*line_num=*/0, boost::filesystem::path("."),
                   &series_collection, &name_to_series_map);

  ASSERT_EQ(series_collection.series_size(), 1);
  EXPECT_EQ(series_collection.series(0).name(), "SeriesTime");
}

TEST(ProcessLineOrDie, UnitName) {
  string line = "1970-01-01 00:00:00, 1970-01-01 00:00:01, "
                "unused_start_time, unused_end_time, unused_duration, "
                "series, location, unused_note";
  SeriesCollection series_collection;
  absl::flat_hash_map<string, Series*> name_to_series_map;
  ProcessLineOrDie(line, /*line_num=*/0, boost::filesystem::path("."),
                   &series_collection, &name_to_series_map);

  ASSERT_EQ(series_collection.series_size(), 1);
  EXPECT_EQ(series_collection.series(0).unit(), "milliseconds");
}

TEST(ProcessLineOrDie, MeasurementsCount) {
  string line = "1970-01-01 00:00:00, 1970-01-01 00:00:01, "
                "unused_start_time, unused_end_time, unused_duration, "
                "series, location, unused_note";
  SeriesCollection series_collection;
  absl::flat_hash_map<string, Series*> name_to_series_map;
  ProcessLineOrDie(line, /*line_num=*/0, boost::filesystem::path("."),
                   &series_collection, &name_to_series_map);

  ASSERT_EQ(series_collection.series_size(), 1);
  EXPECT_EQ(series_collection.series(0).measurement_size(), 1);
}

TEST(RegressionTest, TroublesomeA) {
  // This line was found to have an incorrectly parsed start date.
  string line = "2017-01-20 01:55:01, 2017-01-22 12:00:45, "
                "2017-01-20 01:55:39 GMT, 2017-01-22 12:00:45 GMT,"
                "209106, Alpha, ,";
  SeriesCollection series_collection;
  absl::flat_hash_map<string, Series*> name_to_series_map;
  ProcessLineOrDie(line, /*line_num=*/0, boost::filesystem::path("."),
                   &series_collection, &name_to_series_map);

  ASSERT_EQ(series_collection.series_size(), 1);
  ASSERT_EQ(series_collection.series(0).measurement_size(), 3);
  Measurement measurement = series_collection.series(0).measurement(0);

  absl::Time time = (absl::UnixEpoch() +
                     absl::Milliseconds(measurement.ms_since_unix_epoch()));

  EXPECT_EQ(measurement.ms_since_unix_epoch(), 1484877301000);
}

}  // namespace
}  // namespace me

TEST_MAIN();