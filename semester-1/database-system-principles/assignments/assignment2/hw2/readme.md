### HW2 - CS 257 : Krishna Mula

The folder hw2/ contains two files: the hw2.sqc file which is to be executed, and the readme.md file, which contains instructions on how to execute.

Connect to the DB2 docker container.

Follow the steps to connect to the sample database, and execute our program.

1. db2 connect to sample
2. cd hw2
3. db2 prep hw2.sqc
4. gcc -I ../sqllib/include -c hw2.c
5. gcc -o hw2 hw2.o -L ../sqllib/lib -ldb2
6. ./hw2 sample
