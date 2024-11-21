DROP TABLE emp;
CREATE TABLE emp(name text, salary integer);
INSERT INTO emp 
VALUES ('name1', 1000),('name2', 2000);
CREATE EXTENSION template;
SELECT hello_SQL();
SELECT hello_c();
SELECT add_modulo(4, 2, 10);
SELECT add_modulo(4, 2, 5);
SELECT equation('x', '2', '0');
SELECT complex_add('(1,2)'::complex_number, '(3,4)'::complex_number);
SELECT name, check_salary(emp, 1500) FROM emp;
SELECT generate_record('name', 1500);
SELECT generate_table('name1', 1, 'name2', 2);
