/*******************************************************
**
**  Zadanie 2
**
*******************************************************/

CREATE FUNCTION hello_sql() RETURNS void
AS 
$BODY$
    BEGIN
        RAISE NOTICE 'Hello, SQL!!!';
    END
$BODY$ 
LANGUAGE 'plpgsql';

CREATE FUNCTION hello_c() 
RETURNS void
AS 'MODULE_PATHNAME'
LANGUAGE C;
/*******************************************************
**
**  Zadanie 3
**
*******************************************************/

CREATE FUNCTION add_modulo(integer, integer, integer)
RETURNS integer
AS 'MODULE_PATHNAME'
LANGUAGE C;

/*******************************************************
**
**  Zadanie 4
**
*******************************************************/

CREATE FUNCTION equation(text, text, text)
RETURNS text
AS 'MODULE_PATHNAME'
LANGUAGE C;

/*******************************************************
**
**  Zadanie 5
**
*******************************************************/

CREATE TYPE complex_number;


CREATE OR REPLACE FUNCTION complex_number_in(cstring)
RETURNS complex_number
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;


CREATE OR REPLACE FUNCTION complex_number_out(complex_number)
RETURNS cstring
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;


CREATE TYPE complex_number
(
    internallength = 16,
    input = complex_number_in,
    output = complex_number_out
);

CREATE OR REPLACE FUNCTION complex_add(complex_number, complex_number)
RETURNS complex_number
AS 'MODULE_PATHNAME'
LANGUAGE C;

/*******************************************************
**
**  Zadanie 6
**
*******************************************************/

CREATE FUNCTION check_salary(emp, integer) RETURNS boolean
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT; 

CREATE FUNCTION generate_record(text, int) RETURNS emp
AS 'MODULE_PATHNAME'
LANGUAGE C;

/*******************************************************
**
**  Zadanie 7
**
*******************************************************/

CREATE OR REPLACE FUNCTION generate_table(text, integer, text, integer)
RETURNS SETOF emp
AS 'MODULE_PATHNAME'
LANGUAGE C;

/*******************************************************
**
**  Zadanie 9
**
*******************************************************/

CREATE FUNCTION get_shered_message() 
RETURNS void
AS 'MODULE_PATHNAME'
LANGUAGE C;

CREATE FUNCTION set_shered_message(text) 
RETURNS void
AS 'MODULE_PATHNAME'
LANGUAGE C;

/*******************************************************
**
**  Zadanie 10
**
*******************************************************/

CREATE OR REPLACE FUNCTION wait_signal() 
RETURNS text
AS 'MODULE_PATHNAME'
LANGUAGE C;

CREATE OR REPLACE FUNCTION give_signal() 
RETURNS void
AS 'MODULE_PATHNAME'
LANGUAGE C;
