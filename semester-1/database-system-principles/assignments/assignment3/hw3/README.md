### HW3 - CS 257 : Krishna Mula

The folder hw3/ contains two files: the create.sql file which contains the table, and trigger logic, and the drop.sql file, which contains sql to drop the tables to start fresh.

Follow these steps to execute.

1. connect to the db2 container
1. go to the hw3/ directory, using cd.
1. execute this command: db2 -td"^" -f create.sql ## "^" becausee, that's our delimiter
1. run the test using: db2 -tvf test.sql > test.out
1. clean up everything using: db2 -td"^" -f drop.sql
1. compare the output in test.out to the test.out.final
