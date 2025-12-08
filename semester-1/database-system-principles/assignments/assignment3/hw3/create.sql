connect to CS257^

-- Drop existing objects
drop TRIGGER hw3.classcheck^
drop table hw3.schedule^
drop table hw3.class_prereq^
drop table hw3.class^
drop table hw3.student^

-- TASK 1: Create STUDENT table with PRIMARY KEY
CREATE TABLE hw3.student (
    student_id CHAR(6) NOT NULL PRIMARY KEY,
    first VARCHAR(15) NOT NULL,
    last VARCHAR(15) NOT NULL,
    gender CHAR(1) NOT NULL,
    CONSTRAINT student_gender_check CHECK (gender IN ('M', 'F', 'O'))
)^

-- TASK 1: Create CLASS table with PRIMARY KEY
CREATE TABLE hw3.class (
    class_id CHAR(6) NOT NULL PRIMARY KEY,
    name VARCHAR(20) NOT NULL,
    desc VARCHAR(20) NOT NULL
)^

-- TASK 2 & 3: Create CLASS_PREREQ with FOREIGN KEYS and CASCADE
CREATE TABLE hw3.class_prereq (
    class_id CHAR(6) NOT NULL,
    prereq_id CHAR(6) NOT NULL,
    req_grade CHAR(1) NOT NULL,
    CONSTRAINT class_idfk FOREIGN KEY (class_id) 
        REFERENCES hw3.class(class_id) ON DELETE CASCADE,
    CONSTRAINT prereq_idfk FOREIGN KEY (prereq_id) 
        REFERENCES hw3.class(class_id) ON DELETE CASCADE,
    CONSTRAINT no_self_prereq CHECK (class_id <> prereq_id),
    CONSTRAINT valid_req_grade CHECK (req_grade IN ('A','B','C','D'))
)^

-- TASK 3: Create SCHEDULE with FOREIGN KEYS
CREATE TABLE hw3.schedule (
    student_id CHAR(6) NOT NULL,
    class_id CHAR(6) NOT NULL,
    semester INT NOT NULL,
    year INT NOT NULL,
    grade CHAR(1),
    CONSTRAINT studentidfk FOREIGN KEY (student_id) 
        REFERENCES hw3.student(student_id) ON DELETE CASCADE,
    CONSTRAINT classidfk FOREIGN KEY (class_id) 
        REFERENCES hw3.class(class_id) ON DELETE CASCADE,
    CONSTRAINT valid_semester CHECK (semester IN (1, 2, 3)),
    CONSTRAINT valid_grade CHECK (grade IN ('A','B','C','D','F','I','W'))
)^

-- TASK 4: Create TRIGGER to check prerequisites
CREATE TRIGGER hw3.classcheck
NO CASCADE BEFORE INSERT ON hw3.schedule
REFERENCING NEW AS newrow  
FOR EACH ROW MODE DB2SQL
WHEN (0 < (SELECT COUNT(*)
            FROM hw3.class_prereq 
            WHERE hw3.class_prereq.class_id = newrow.class_id)) 
BEGIN ATOMIC
    DECLARE num_prereq INT;
    DECLARE prereq_satisfied INT;

    -- Count total prerequisites for this class
    SET num_prereq = (SELECT COUNT(*)
                      FROM hw3.class_prereq 
                      WHERE hw3.class_prereq.class_id = newrow.class_id);

    -- Count how many prerequisites the student has satisfied
    SET prereq_satisfied = (
        SELECT COUNT(DISTINCT cp.prereq_id)
        FROM hw3.class_prereq cp
        WHERE cp.class_id = newrow.class_id
        AND EXISTS (
            SELECT 1
            FROM hw3.schedule s
            WHERE s.student_id = newrow.student_id
            AND s.class_id = cp.prereq_id
            AND s.grade IS NOT NULL
            AND (
                (cp.req_grade = 'A' AND s.grade = 'A') OR
                (cp.req_grade = 'B' AND s.grade IN ('A','B')) OR
                (cp.req_grade = 'C' AND s.grade IN ('A','B','C')) OR
                (cp.req_grade = 'D' AND s.grade IN ('A','B','C','D'))
            )
        )
    );

    -- If not all prerequisites are satisfied, reject the insert
    IF (prereq_satisfied < num_prereq) THEN
        SIGNAL SQLSTATE '88888' ('Missing Pre-req');
    END IF;
END^
