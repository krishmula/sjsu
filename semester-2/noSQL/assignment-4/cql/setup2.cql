USE hw4;
TRUNCATE citylist;
TRUNCATE citylist2;
COPY citylist (city, loc, pop, state, zip) FROM '/citylist.csv' WITH HEADER = TRUE;
COPY citylist2 (city, loc, pop, state, zip) FROM '/citylist.csv' WITH HEADER = TRUE;
