#!/bin/bash
rm -f dbfile.bin *.tab
./db "CREATE TABLE class(Student_Name char(20) NOT NULL, Gender char(1), Exams int, Quiz_Total int, Total int NOT NULL)"
./db "INSERT INTO class VALUES ('Alice', 'F', 85, 380, 465)"
./db "INSERT INTO class VALUES ('Bob', 'M', 78, 350, 428)"
echo "--- SELECT * FROM class ---"
./db "SELECT * FROM class"
echo "--- SELECT * FROM class WHERE Total > 450 ---"
./db "SELECT * FROM class WHERE Total > 450"
echo "--- SELECT * FROM class ORDER BY Total DESC ---"
./db "SELECT * FROM class ORDER BY Total DESC"

./db "CREATE TABLE grades(Student_Name char(20) NOT NULL, Final_Grade char(2), GPA int)"
./db "INSERT INTO grades VALUES ('Alice', 'A', 4)"
./db "INSERT INTO grades VALUES ('Bob', 'B', 3)"

echo "--- SELECT * FROM class NATURAL JOIN grades ---"
./db "SELECT * FROM class NATURAL JOIN grades"

echo "--- SELECT Student_Name, Total, GPA FROM class NATURAL JOIN grades ---"
./db "SELECT Student_Name, Total, GPA FROM class NATURAL JOIN grades"

echo "--- Test 41: SELECT * FROM class WHERE ---"
./db "SELECT * FROM class WHERE"
echo "Exit code: $?"

echo "--- Test 47: SELECT MAX(Exams) FROM class ---"
./db "SELECT MAX(Exams) FROM class"
echo "Exit code: $?"


