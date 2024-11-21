#include "postgres.h"

#include <string.h>
#include <stdio.h>

#include "executor/executor.h"
#include "fmgr.h"
#include "funcapi.h"
#include "utils/builtins.h"
#include "utils/geo_decls.h"
#include "utils/memutils.h"
#include <miscadmin.h>
#include <storage/ipc.h>
#include <storage/shmem.h>
#include <storage/lwlock.h>
#include <storage/latch.h>
#include "utils/wait_event.h"


PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(hello_c);
PG_FUNCTION_INFO_V1(add_modulo);
PG_FUNCTION_INFO_V1(equation);
PG_FUNCTION_INFO_V1(complex_number_in);
PG_FUNCTION_INFO_V1(complex_number_out);
PG_FUNCTION_INFO_V1(complex_add);
PG_FUNCTION_INFO_V1(check_salary);
PG_FUNCTION_INFO_V1(generate_record);
PG_FUNCTION_INFO_V1(generate_table);
PG_FUNCTION_INFO_V1(get_shered_message);
PG_FUNCTION_INFO_V1(set_shered_message);
PG_FUNCTION_INFO_V1(wait_signal);
PG_FUNCTION_INFO_V1(give_signal);

////////////////////////////////////////////////////////
//
//  Задание 2
//
////////////////////////////////////////////////////////

// Функция выводит строку "Hello, C!!!", как сообщение об ошибке
//
Datum 
hello_c(PG_FUNCTION_ARGS)
{
	ereport(NOTICE, errmsg("Hello, C!!!"));

	PG_RETURN_VOID();
}

////////////////////////////////////////////////////////
//
//  Задание 3
//
////////////////////////////////////////////////////////

// Функция выводит сумму двух первых аргументов
// по модулю третьего
//
Datum 
add_modulo(PG_FUNCTION_ARGS)
{
	int summand1, summand2, mod, ret;

	summand1 = PG_GETARG_INT32(0);
	summand2 = PG_GETARG_INT32(1);
	mod = PG_GETARG_INT32(2);

	if (summand1 > 0 && summand2 > 0 && mod > 0)
		ret = (summand1 + summand2) % mod;
	else
	{
		ret = -1;
		ereport(NOTICE, errmsg("ERROR: negative argument"));
	}
	PG_RETURN_INT32(ret);
}

////////////////////////////////////////////////////////
//
//  Задание 4
//
////////////////////////////////////////////////////////

// Функция выводит строку представлющую аргументы, как уравнение
//
Datum 
equation(PG_FUNCTION_ARGS)
{
	char *arg1, *arg2, *arg3;

	arg1 = text_to_cstring(PG_GETARG_TEXT_P_COPY(0));
	arg2 = text_to_cstring(PG_GETARG_TEXT_P_COPY(1));
	arg3 = text_to_cstring(PG_GETARG_TEXT_P_COPY(2));

	PG_RETURN_TEXT_P(cstring_to_text(psprintf("%s + %s = %s", arg1, arg2, arg3)));
}

////////////////////////////////////////////////////////
//
//  Задание 5
//
////////////////////////////////////////////////////////

// Определение типа данных - комплекное число
//
typedef struct
{
	double real;
	double imaginary;

} complex_number;

// Опредление input-функции нового типа
//
Datum 
complex_number_in(PG_FUNCTION_ARGS)
{
	char* s = PG_GETARG_CSTRING(0);

	complex_number* a = (complex_number*)palloc(sizeof(complex_number));

	sscanf(s, "(%lf,%lf)", &(a->real), &(a->imaginary));

	PG_RETURN_POINTER(a);
}

// Определение output-функции нового типа
//
Datum 
complex_number_out(PG_FUNCTION_ARGS)
{
	complex_number* a = (complex_number*)PG_GETARG_POINTER(0);

	PG_RETURN_CSTRING(psprintf("(%lf,%lf)", a->real, a->imaginary));
}

// Функция сложения комплексных чисел
//
Datum 
complex_add(PG_FUNCTION_ARGS)
{
	complex_number* a = (complex_number*)PG_GETARG_POINTER(0);
	complex_number* b = (complex_number*)PG_GETARG_POINTER(1);

	complex_number* c = (complex_number*)palloc(sizeof(complex_number));

	c->real = a->real + b->real;
	c->imaginary = a->imaginary + b->imaginary;

	PG_RETURN_POINTER(c);
}

////////////////////////////////////////////////////////
//
//  Задание 6
//
////////////////////////////////////////////////////////

// Функция принимает кортеж типа emp и целочисленный параметр
// Возращает true,если поле salary кортежа превышает этот параметр
//
Datum
check_salary(PG_FUNCTION_ARGS)
{
	HeapTupleHeader  t = PG_GETARG_HEAPTUPLEHEADER(0);
	int32            limit = PG_GETARG_INT32(1);
	bool isnull = false;
	Datum salary;

	salary = GetAttributeByName(t, "salary", &isnull);
	if (isnull)
		PG_RETURN_BOOL(false);

	PG_RETURN_BOOL(DatumGetInt32(salary) > limit);
}

// Функция принимает два парметра: text и integer
// формирует из них кортерж таблицы emp и возвращает его
//
Datum 
generate_record(PG_FUNCTION_ARGS)
{
	Oid 			 resultTypeId;
	TupleDesc 		 resultTupleDesc;
	AttInMetadata* a;
	char* str;
	int32			 x;
	HeapTuple 		 h;
	char* values[2];

	str = text_to_cstring(PG_GETARG_TEXT_P_COPY(0));

	x = PG_GETARG_INT32(1);

	if (get_call_result_type(fcinfo, &resultTypeId, &resultTupleDesc) != TYPEFUNC_COMPOSITE)
		elog(ERROR, "Function returning record called in context that cannot accept type record");

	a = TupleDescGetAttInMetadata(resultTupleDesc);

	values[0] = psprintf("%s!!!", str);
	values[1] = psprintf("%d", 2 * x);

	h = BuildTupleFromCStrings(a, values);

	return HeapTupleGetDatum(h);
}

////////////////////////////////////////////////////////
//
//  Задание 7
//
////////////////////////////////////////////////////////

// Функция принимает две пары: text и integer
// формирует из них таблицу emp, состоящую двух кортежей
// и возвращает её
//
Datum 
generate_table(PG_FUNCTION_ARGS)
{
	ReturnSetInfo* rsinfo;
	Datum values[2];
	bool nulls[2];

	InitMaterializedSRF(fcinfo, 0);
	rsinfo = (ReturnSetInfo*)fcinfo->resultinfo;

	memset(nulls, 0, sizeof(nulls));

	values[0] = PG_GETARG_DATUM(0);
	values[1] = PG_GETARG_DATUM(1);
	tuplestore_putvalues(rsinfo->setResult, rsinfo->setDesc, values, nulls);

	values[0] = PG_GETARG_DATUM(2);
	values[1] = PG_GETARG_DATUM(3);
	tuplestore_putvalues(rsinfo->setResult, rsinfo->setDesc, values, nulls);

	return (Datum)0;
}

////////////////////////////////////////////////////////
//
//  Задания 9-10
//
////////////////////////////////////////////////////////

typedef struct SharedStruct
{
	char* message;
} SharedStruct;

static shmem_request_hook_type prev_shmem_request_hook = NULL;
static void requestSharedMemory(void);

static SharedStruct* shared_struct = NULL;
static Latch* lt;

void _PG_init(void);

void
_PG_init(void)
{
	prev_shmem_request_hook = shmem_request_hook;
	shmem_request_hook = requestSharedMemory;
}

static void
requestSharedMemory(void)
{
	if (prev_shmem_request_hook)
		prev_shmem_request_hook();	

	RequestAddinShmemSpace(sizeof(shared_struct));
	RequestNamedLWLockTranche("SharedStruct", 1);

	RequestAddinShmemSpace(sizeof(lt));
	RequestNamedLWLockTranche("Latch", 1);
	
}

static void 
startupSharedMemory1(void)
{
	bool found;

	LWLockAcquire(AddinShmemInitLock, LW_EXCLUSIVE);	
	shared_struct = ShmemInitStruct("SharedStruct", sizeof(shared_struct), &found);
	
	if (!found)	
		shared_struct->message = NULL;
	
	LWLockRelease(AddinShmemInitLock);
}

static void 
startupSharedMemory2(void)
{
	bool found;

	LWLockAcquire(AddinShmemInitLock, LW_EXCLUSIVE);
	lt = ShmemInitStruct("Latch", sizeof(lt), &found);

	if (!found)	
		InitSharedLatch(lt);

	
	LWLockRelease(AddinShmemInitLock);
}

// Функция возвращает значение  
// структуры разделяемой памяти
//
Datum 
get_shered_message(PG_FUNCTION_ARGS)
{
	if (shared_struct == NULL)
		startupSharedMemory1();

	PG_RETURN_VOID();
}

// Функция устанавливет значение  
// структуры разделяемой памяти
//
Datum 
set_shered_message(PG_FUNCTION_ARGS)
{
	if (shared_struct == NULL)
		startupSharedMemory1();

	if (shared_struct->message != NULL)
		pfree(shared_struct->message);

	shared_struct->message = text_to_cstring(PG_GETARG_TEXT_P_COPY(0));

	PG_RETURN_VOID();
}

// Функция ожидающая утсановку защелки  
//
Datum 
wait_signal(PG_FUNCTION_ARGS)
{
	int buff = 0;
	char* str;

	if (lt == NULL)	
		startupSharedMemory2();

	OwnLatch(lt);

	for (;;)
	{
		ResetLatch(lt);
		if (buff & WL_LATCH_SET)
		{			
			str = psprintf("It's OK!!!");
			break;
		}
		if (buff & WL_TIMEOUT)
		{			
			str = psprintf("TIMEOUT!!!");
			break;
		}
		buff = WaitLatch(lt, WL_LATCH_SET | WL_TIMEOUT, 10000, PG_WAIT_EXTENSION);
	}
	DisownLatch(lt);

	PG_RETURN_TEXT_P(cstring_to_text(str));
}

// Функция устанавливающая защелку  
//
Datum 
give_signal(PG_FUNCTION_ARGS)
{
	if (lt == NULL)	
		startupSharedMemory2();

	SetLatch(lt);
	PG_RETURN_VOID();
}
