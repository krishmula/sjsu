CREATE KEYSPACE IF NOT EXISTS hw4 WITH REPLICATION = { 'class' : 'SimpleStrategy', 'replication_factor' : 1 };
USE hw4;

DROP TABLE IF EXISTS citylist;
CREATE TABLE citylist (
  city varchar,
  loc list<float>,
  pop int,
  state varchar,
  zip varchar,
  PRIMARY KEY (state, city, zip)
);

COPY citylist (state, city, zip, loc, pop) FROM '/citylist.csv' WITH HEADER = TRUE;

DROP TABLE IF EXISTS citylist2;
CREATE TABLE citylist2 (
  city varchar,
  loc list<float>,
  pop int,
  state varchar,
  zip varchar,
  PRIMARY KEY (state, city, zip)
) WITH CLUSTERING ORDER BY (city ASC, zip DESC);

COPY citylist2 (state, city, zip, loc, pop) FROM '/citylist.csv' WITH HEADER = TRUE;
