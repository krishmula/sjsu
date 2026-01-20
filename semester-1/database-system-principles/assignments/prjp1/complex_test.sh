#!/bin/bash

echo ""
echo "Test 55: Complex query with >=, <=, <>, AND, ORDER BY DESC"
rm -f employees55.tab
./db "CREATE TABLE employees55 (emp_id int, name char(25), department char(20), salary int, years int)" > /dev/null
./db "INSERT INTO employees55 VALUES (201, 'Sarah Williams', 'Engineering', 125000, 8)" > /dev/null
./db "INSERT INTO employees55 VALUES (202, 'Mike Anderson', 'Sales', 95000, 5)" > /dev/null
./db "INSERT INTO employees55 VALUES (203, 'Lisa Martinez', 'Engineering', 135000, 12)" > /dev/null
./db "INSERT INTO employees55 VALUES (204, 'Tom Jackson', 'Support', 72000, 3)" > /dev/null
./db "INSERT INTO employees55 VALUES (205, 'Emma Thompson', 'Sales', 88000, 6)" > /dev/null
./db "INSERT INTO employees55 VALUES (206, 'David Lee', 'Engineering', 115000, 9)" > /dev/null
./db "INSERT INTO employees55 VALUES (207, 'Nina Patel', 'Support', 68000, 2)" > /dev/null
./db "INSERT INTO employees55 VALUES (208, 'Chris Brown', 'Sales', 92000, 4)" > /dev/null

./db "SELECT name, department, salary FROM employees55 WHERE salary >= 90000 AND salary <= 130000 AND department <> 'Support' ORDER BY salary DESC" 2>&1 | grep -A 100 "SELECT statement" > test55.out
cat > test55.exp << 'EXPECTED'
SELECT statement
name                      department           salary 
------------------------- -------------------- ------ 
Sarah Williams            Engineering          125000 
David Lee                 Engineering          115000 
Mike Anderson             Sales                 95000 
Chris Brown               Sales                 92000 


 4 record(s) selected.
EXPECTED

if diff -w test55.out test55.exp > /dev/null; then
    echo "Test 55 passed"
    ((PASSED++))
    rm -f test55.out test55.exp employees55.tab
else
    echo "Test 55 FAILED"
    ((FAILED++))
    echo "Expected:"
    cat test55.exp
    echo "Got:"
    cat test55.out
fi

echo ""
echo "Test 56: Aggregates with complex WHERE using all new operators"
rm -f sales.tab
./db "CREATE TABLE sales (product_id int, product_name char(20), price int, quantity int, region char(10))" > /dev/null
./db "INSERT INTO sales VALUES (301, 'Laptop', 1200, 15, 'North')" > /dev/null
./db "INSERT INTO sales VALUES (302, 'Mouse', 25, 150, 'South')" > /dev/null
./db "INSERT INTO sales VALUES (303, 'Keyboard', 80, 85, 'East')" > /dev/null
./db "INSERT INTO sales VALUES (304, 'Monitor', 350, 45, 'West')" > /dev/null
./db "INSERT INTO sales VALUES (305, 'Headset', 120, 95, 'North')" > /dev/null
./db "INSERT INTO sales VALUES (306, 'Webcam', 90, 72, 'South')" > /dev/null
./db "INSERT INTO sales VALUES (307, 'Tablet', 450, 38, 'East')" > /dev/null

./db "SELECT SUM(price), AVG(quantity), COUNT(*) FROM sales WHERE price >= 80 AND price <= 500 AND region <> 'South'" 2>&1 | grep -A 100 "SELECT statement" > test56.out
cat > test56.exp << 'EXPECTED'
SELECT statement
SUM        AVG        COUNT     
---------- ---------- ----------
      1000         65          4
EXPECTED

if diff -w test56.out test56.exp > /dev/null; then
    echo "Test 56 passed"
    ((PASSED++))
    rm -f test56.out test56.exp sales.tab
else
    echo "Test 56 FAILED"
    ((FAILED++))
    echo "Expected:"
    cat test56.exp
    echo "Got:"
    cat test56.out
fi
