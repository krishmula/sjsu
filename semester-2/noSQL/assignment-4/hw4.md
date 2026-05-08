---
pdf_options:
  format: a4
  margin: 10mm
  landscape: true
css: |-
  pre {
    font-size: 8px !important;
  }
---

# Assignment 4 (Cassandra)

## 3. Describe your data wrangling method.

To get the original data ready for Cassandra, I had to account for a few structural differences. The raw `zips.json` stores the zipcode under an `_id` field and provides the location as a basic JSON array. However, our `citylist` schema explicitly needs a `zip` column and requires the `loc` field to be formatted as a `list<float>`. 

To handle this, I wrote a Python parser (`wrangle.py`) that steps through the JSON file line line-by-line. For each record, it grabs the `_id` to act as the `zip`, extracts the coordinates, and converts them into a string that perfectly matches CQL list syntax. Finally, the script dumps this cleaned data into a new comma-separated file (`zips_wrangled.csv`), organizing the columns so they can be seamlessly ingested using Cassandra's built-in `COPY` command.

## 4. Screenshot of results

```sql
SELECT * FROM citylist LIMIT 15;
```

**Result:**

```text
 state | city        | zip   | loc                   | pop
-------+-------------+-------+-----------------------+------
    IA |      ACKLEY | 50601 | [-93.06084, 42.55272] | 2316
    IA |    ACKWORTH | 50001 | [-93.37672, 41.37372] |  491
    IA |       ADAIR | 50002 | [-94.64436, 41.51369] | 1748
    IA |        ADEL | 50003 | [-94.03796, 41.62214] | 4884
    IA |       AFTON | 50830 | [-94.19405, 41.04015] | 1450
    IA |      AGENCY | 52530 | [-92.31623, 40.99569] | 1233
    IA |   AINSWORTH | 52201 | [-91.54717, 41.32022] | 1265
    IA |       AKRON | 51001 | [-96.52245, 42.83539] | 2068
    IA | ALBERT CITY | 50510 |  [-94.98238, 42.7784] | 1293
    IA |       ALBIA | 52531 | [-92.79462, 41.02872] | 6397
    IA |      ALBION | 50005 | [-92.98821, 42.11427] |  773
    IA |   ALBURNETT | 52202 | [-91.63924, 42.15832] | 1038
    IA |       ALDEN | 50006 | [-93.38412, 42.51789] | 1374
    IA |   ALEXANDER | 50420 |   [-93.445, 42.81139] |  664
    IA |      ALGONA | 50511 | [-94.23064, 43.06598] | 7829

(15 rows)
```

## 5. 

### Method 1: Allow Filtering

**A CQL query to get the result:**
```sql
SELECT * FROM citylist WHERE city = 'NEW YORK' LIMIT 15 ALLOW FILTERING;
```

**Result & Execution Time:**

```text
 state | city     | zip   | loc                   | pop
-------+----------+-------+-----------------------+-------
    NY | NEW YORK | 10001 |  [-73.9967, 40.74838] | 18913
    NY | NEW YORK | 10002 | [-73.98768, 40.71523] | 84143
    NY | NEW YORK | 10003 | [-73.98922, 40.73125] | 51224
    NY | NEW YORK | 10005 | [-74.00835, 40.70565] |   202
    NY | NEW YORK | 10006 | [-74.01347, 40.70845] |   119
    NY | NEW YORK | 10007 | [-74.00702, 40.71391] |  3374
    NY | NEW YORK | 10009 | [-73.97959, 40.72619] | 57426
    NY | NEW YORK | 10010 | [-73.98133, 40.73748] | 24907
    NY | NEW YORK | 10011 | [-73.99963, 40.74023] | 46560
    NY | NEW YORK | 10012 | [-73.99828, 40.72553] | 26365
    NY | NEW YORK | 10013 | [-74.00253, 40.71851] | 21860
    NY | NEW YORK | 10014 | [-74.00542, 40.73393] | 31147
    NY | NEW YORK | 10016 | [-73.97813, 40.74428] | 51561
    NY | NEW YORK | 10017 | [-73.97066, 40.75172] | 12465
    NY | NEW YORK | 10018 |  [-73.9925, 40.75471] |  4834

(15 rows)

Tracing session: 43618d60-4521-11f1-9cae-57de2c5e9a02

 activity                                                                                                                   | timestamp                  | source     | source_elapsed | client
----------------------------------------------------------------------------------------------------------------------------+----------------------------+------------+----------------+-----------
                                                                                                         Execute CQL3 query | 2026-05-01 05:47:39.190000 | 172.17.0.2 |              0 | 127.0.0.1
             Parsing SELECT * FROM citylist WHERE city = 'NEW YORK' LIMIT 15 ALLOW FILTERING; [Native-Transport-Requests-8] | 2026-05-01 05:47:39.190001 | 172.17.0.2 |            454 | 127.0.0.1
                                                                          Preparing statement [Native-Transport-Requests-8] | 2026-05-01 05:47:39.191000 | 172.17.0.2 |            685 | 127.0.0.1
                                                                    Computing ranges to query [Native-Transport-Requests-8] | 2026-05-01 05:47:39.191001 | 172.17.0.2 |            819 | 127.0.0.1
 Submitting range requests on 17 ranges with a concurrency of 1 (0.0 rows per range expected) [Native-Transport-Requests-8] | 2026-05-01 05:47:39.191002 | 172.17.0.2 |            913 | 127.0.0.1
                                                        Submitted 1 concurrent range requests [Native-Transport-Requests-8] | 2026-05-01 05:47:39.191003 | 172.17.0.2 |            991 | 127.0.0.1
              Executing seq scan across 0 sstables for (min(-9223372036854775808), min(-9223372036854775808)] [ReadStage-7] | 2026-05-01 05:47:39.191004 | 172.17.0.2 |           1081 | 127.0.0.1
                                                                      Read 15 live rows and 0 tombstone cells [ReadStage-7] | 2026-05-01 05:47:39.193000 | 172.17.0.2 |           2648 | 127.0.0.1
                                                                                                           Request complete | 2026-05-01 05:47:39.192890 | 172.17.0.2 |           2890 | 127.0.0.1
```

### Method 2: Secondary Index

**CQL command to create an index and a CQL query to get the result:**
```sql
CREATE INDEX city_idx ON citylist (city);
SELECT * FROM citylist WHERE city = 'NEW YORK' LIMIT 15;
```

**Result & Execution Time:**

```text
 state | city     | zip   | loc                   | pop
-------+----------+-------+-----------------------+-------
    NY | NEW YORK | 10001 |  [-73.9967, 40.74838] | 18913
    NY | NEW YORK | 10002 | [-73.98768, 40.71523] | 84143
    NY | NEW YORK | 10003 | [-73.98922, 40.73125] | 51224
    NY | NEW YORK | 10005 | [-74.00835, 40.70565] |   202
    NY | NEW YORK | 10006 | [-74.01347, 40.70845] |   119
    NY | NEW YORK | 10007 | [-74.00702, 40.71391] |  3374
    NY | NEW YORK | 10009 | [-73.97959, 40.72619] | 57426
    NY | NEW YORK | 10010 | [-73.98133, 40.73748] | 24907
    NY | NEW YORK | 10011 | [-73.99963, 40.74023] | 46560
    NY | NEW YORK | 10012 | [-73.99828, 40.72553] | 26365
    NY | NEW YORK | 10013 | [-74.00253, 40.71851] | 21860
    NY | NEW YORK | 10014 | [-74.00542, 40.73393] | 31147
    NY | NEW YORK | 10016 | [-73.97813, 40.74428] | 51561
    NY | NEW YORK | 10017 | [-73.97066, 40.75172] | 12465
    NY | NEW YORK | 10018 |  [-73.9925, 40.75471] |  4834

(15 rows)

Tracing session: 53cf06f0-4521-11f1-9cae-57de2c5e9a02

 activity                                                                                                                        | timestamp                  | source     | source_elapsed | client
---------------------------------------------------------------------------------------------------------------------------------+----------------------------+------------+----------------+-----------
                                                                                                              Execute CQL3 query | 2026-05-01 05:48:06.751000 | 172.17.0.2 |              0 | 127.0.0.1
                                  Parsing SELECT * FROM citylist WHERE city = 'NEW YORK' LIMIT 15; [Native-Transport-Requests-6] | 2026-05-01 05:48:06.751001 | 172.17.0.2 |            118 | 127.0.0.1
                                                                               Preparing statement [Native-Transport-Requests-6] | 2026-05-01 05:48:06.751002 | 172.17.0.2 |            230 | 127.0.0.1
                                  Index mean cardinalities are city_idx:1. Scanning with city_idx. [Native-Transport-Requests-6] | 2026-05-01 05:48:06.751003 | 172.17.0.2 |            483 | 127.0.0.1
                                                                         Computing ranges to query [Native-Transport-Requests-6] | 2026-05-01 05:48:06.751004 | 172.17.0.2 |            500 | 127.0.0.1
 Submitting range requests on 17 ranges with a concurrency of 17 (0.05625 rows per range expected) [Native-Transport-Requests-6] | 2026-05-01 05:48:06.751005 | 172.17.0.2 |            536 | 127.0.0.1
                                                             Submitted 1 concurrent range requests [Native-Transport-Requests-6] | 2026-05-01 05:48:06.752000 | 172.17.0.2 |           1004 | 127.0.0.1
                                                               Executing read on hw4.citylist using index city_idx [ReadStage-9] | 2026-05-01 05:48:06.752001 | 172.17.0.2 |           1114 | 127.0.0.1
                                                             Executing single-partition query on citylist.city_idx [ReadStage-9] | 2026-05-01 05:48:06.752002 | 172.17.0.2 |           1331 | 127.0.0.1
                                                                                      Acquiring sstable references [ReadStage-9] | 2026-05-01 05:48:06.752003 | 172.17.0.2 |           1341 | 127.0.0.1
                                         Skipped 0/1 non-slice-intersecting sstables, included 0 due to tombstones [ReadStage-9] | 2026-05-01 05:48:06.752004 | 172.17.0.2 |           1354 | 127.0.0.1
                                                                     Partition index found for sstable 1, size = 0 [ReadStage-9] | 2026-05-01 05:48:06.752005 | 172.17.0.2 |           1446 | 127.0.0.1
                                                                      Executing single-partition query on citylist [ReadStage-9] | 2026-05-01 05:48:06.752006 | 172.17.0.2 |           1630 | 127.0.0.1
                                                                                      Acquiring sstable references [ReadStage-9] | 2026-05-01 05:48:06.752007 | 172.17.0.2 |           1650 | 127.0.0.1
                                         Skipped 0/1 non-slice-intersecting sstables, included 0 due to tombstones [ReadStage-9] | 2026-05-01 05:48:06.753000 | 172.17.0.2 |           2014 | 127.0.0.1
                                                                             Key cache hit for sstable 1, size = 2 [ReadStage-9] | 2026-05-01 05:48:06.753001 | 172.17.0.2 |           2048 | 127.0.0.1
                                                                         Merged data from memtables and 1 sstables [ReadStage-9] | 2026-05-01 05:48:06.753002 | 172.17.0.2 |           2247 | 127.0.0.1
                                                                           Read 15 live rows and 0 tombstone cells [ReadStage-9] | 2026-05-01 05:48:06.753003 | 172.17.0.2 |           2285 | 127.0.0.1
                                                                         Merged data from memtables and 1 sstables [ReadStage-9] | 2026-05-01 05:48:06.753004 | 172.17.0.2 |           2295 | 127.0.0.1
                                                                                                                Request complete | 2026-05-01 05:48:06.753740 | 172.17.0.2 |           2740 | 127.0.0.1
```

### Method 3: Materialized View

**CQL command to create a materialized view and a CQL query to get the result:**
```sql
CREATE MATERIALIZED VIEW citylist_mv AS
  SELECT * FROM citylist
  WHERE city IS NOT NULL AND state IS NOT NULL AND zip IS NOT NULL
  PRIMARY KEY (city, state, zip);
  
SELECT * FROM citylist_mv WHERE city = 'NEW YORK' LIMIT 15;
```

**Result & Execution Time:**

```text
 city     | state | zip   | loc                   | pop
----------+-------+-------+-----------------------+-------
 NEW YORK |    NY | 10001 |  [-73.9967, 40.74838] | 18913
 NEW YORK |    NY | 10002 | [-73.98768, 40.71523] | 84143
 NEW YORK |    NY | 10003 | [-73.98922, 40.73125] | 51224
 NEW YORK |    NY | 10005 | [-74.00835, 40.70565] |   202
 NEW YORK |    NY | 10006 | [-74.01347, 40.70845] |   119
 NEW YORK |    NY | 10007 | [-74.00702, 40.71391] |  3374
 NEW YORK |    NY | 10009 | [-73.97959, 40.72619] | 57426
 NEW YORK |    NY | 10010 | [-73.98133, 40.73748] | 24907
 NEW YORK |    NY | 10011 | [-73.99963, 40.74023] | 46560
 NEW YORK |    NY | 10012 | [-73.99828, 40.72553] | 26365
 NEW YORK |    NY | 10013 | [-74.00253, 40.71851] | 21860
 NEW YORK |    NY | 10014 | [-74.00542, 40.73393] | 31147
 NEW YORK |    NY | 10016 | [-73.97813, 40.74428] | 51561
 NEW YORK |    NY | 10017 | [-73.97066, 40.75172] | 12465
 NEW YORK |    NY | 10018 |  [-73.9925, 40.75471] |  4834

(15 rows)

Tracing session: 5cd709f0-4521-11f1-9cae-57de2c5e9a02

 activity                                                                                          | timestamp                  | source     | source_elapsed | client
---------------------------------------------------------------------------------------------------+----------------------------+------------+----------------+-----------
                                                                                Execute CQL3 query | 2026-05-01 05:48:21.903000 | 172.17.0.2 |              0 | 127.0.0.1
 Parsing SELECT * FROM citylist_mv WHERE city = 'NEW YORK' LIMIT 15; [Native-Transport-Requests-6] | 2026-05-01 05:48:21.903001 | 172.17.0.2 |            257 | 127.0.0.1
                                                 Preparing statement [Native-Transport-Requests-6] | 2026-05-01 05:48:21.903002 | 172.17.0.2 |            413 | 127.0.0.1
                                     Executing single-partition query on citylist_mv [ReadStage-7] | 2026-05-01 05:48:21.904000 | 172.17.0.2 |           1036 | 127.0.0.1
                                                        Acquiring sstable references [ReadStage-7] | 2026-05-01 05:48:21.904001 | 172.17.0.2 |           1072 | 127.0.0.1
           Skipped 0/0 non-slice-intersecting sstables, included 0 due to tombstones [ReadStage-7] | 2026-05-01 05:48:21.904002 | 172.17.0.2 |           1247 | 127.0.0.1
                                           Merged data from memtables and 0 sstables [ReadStage-7] | 2026-05-01 05:48:21.904003 | 172.17.0.2 |           1329 | 127.0.0.1
                                             Read 15 live rows and 0 tombstone cells [ReadStage-7] | 2026-05-01 05:48:21.904004 | 172.17.0.2 |           1363 | 127.0.0.1
                                                                                  Request complete | 2026-05-01 05:48:21.904505 | 172.17.0.2 |           1505 | 127.0.0.1
```

**Your analysis goes here:**
- **Method 1 (Time: 2.89 ms):** Relying solely on `ALLOW FILTERING` proved to be the slowest. Without an index or optimized partition key, Cassandra is forced to brute-force a full scan across all partitions just to evaluate the city condition. 
- **Method 2 (Time: 2.74 ms):** The Secondary Index approach is quicker than a full scan. Cassandra creates a hidden lookup table for the index, queries it for the matching primary keys, and then fetches the corresponding rows. It's an improvement, but finding and fetching non-sequential records still introduces notable overhead.
- **Method 3 (Time: 1.51 ms):** The Materialized View is easily the fastest option. Because a view actively restructures the data so that `city` becomes the partition key, filtering by 'NEW YORK' becomes an immediate, exact partition match with practically zero wasted effort.

## 6. Which method(s) work for this question?

You can successfully execute this query using either the Materialized View (Method 3) or the Secondary Index (Method 2) as long as you append `ALLOW FILTERING` to force the population condition to be evaluated. However, querying the Materialized View is undeniably the better choice here since your target data is already efficiently partitioned by the requested `city`.

**CQL query to get the result:**
```sql
SELECT zip FROM citylist_mv WHERE city = 'NEW YORK' AND pop > 20000 AND pop < 30000 LIMIT 15 ALLOW FILTERING;
```


**Result:**

```text
 zip
-------
 10010
 10012
 10013
 10026
 10030
 10035
 10039

(7 rows)
```

## 7. Schema declaration

```sql
CREATE TABLE citylist2 (
  city varchar,
  loc list<float>,
  pop int,
  state varchar,
  zip varchar,
  PRIMARY KEY (state, city, zip)
) WITH CLUSTERING ORDER BY (city ASC, zip DESC);
```

## 8. Show the table content.

```sql
SELECT * FROM citylist2 LIMIT 15;
```
**Result:**


**Result:**

```text
 state | city        | zip   | loc                   | pop
-------+-------------+-------+-----------------------+------
    IA |      ACKLEY | 50601 | [-93.06084, 42.55272] | 2316
    IA |    ACKWORTH | 50001 | [-93.37672, 41.37372] |  491
    IA |       ADAIR | 50002 | [-94.64436, 41.51369] | 1748
    IA |        ADEL | 50003 | [-94.03796, 41.62214] | 4884
    IA |       AFTON | 50830 | [-94.19405, 41.04015] | 1450
    IA |      AGENCY | 52530 | [-92.31623, 40.99569] | 1233
    IA |   AINSWORTH | 52201 | [-91.54717, 41.32022] | 1265
    IA |       AKRON | 51001 | [-96.52245, 42.83539] | 2068
    IA | ALBERT CITY | 50510 |  [-94.98238, 42.7784] | 1293
    IA |       ALBIA | 52531 | [-92.79462, 41.02872] | 6397
    IA |      ALBION | 50005 | [-92.98821, 42.11427] |  773
    IA |   ALBURNETT | 52202 | [-91.63924, 42.15832] | 1038
    IA |       ALDEN | 50006 | [-93.38412, 42.51789] | 1374
    IA |   ALEXANDER | 50420 |   [-93.445, 42.81139] |  664
    IA |      ALGONA | 50511 | [-94.23064, 43.06598] | 7829

(15 rows)
```

## 9. CQL query to get the result

```sql
SELECT zip FROM citylist2 WHERE state = 'CA' LIMIT 15;
**Result:**
```


**Result:**

```text
 zip
-------
 95220
 93510
 93446
 92301
 96006
 91350
 92536
 93601
 94507
 94706
 95410
 91803
 91801
 92656
 95910

(15 rows)
```

## 10. CQL query to get the result

```sql
**Result:**
SELECT zip FROM citylist2 WHERE state = 'CA' AND city = 'SAN JOSE' AND pop > 5000 AND pop < 10000 LIMIT 15 ALLOW FILTERING;
```

**Result:**


**Result:**

```text
 zip
-------
 95139
 95138
 95135
 95119

(4 rows)
```
