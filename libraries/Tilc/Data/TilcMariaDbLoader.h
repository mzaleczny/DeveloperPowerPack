#pragma once
#include <stddef.h>
#include "mysql.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef MYSQL* (*mysql_init_fn)(MYSQL*);
typedef MYSQL* (*mysql_real_connect_fn)(MYSQL*, const char*, const char*, const char*, const char*, unsigned int, const char*, unsigned long);
typedef int (*mysql_query_fn)(MYSQL*, const char*);
typedef MYSQL_ROW (*mysql_fetch_row_fn)(MYSQL_RES*);
typedef MYSQL_STMT* (*mysql_stmt_init_fn)(MYSQL* mysql);
typedef int (*mysql_stmt_prepare_fn)(MYSQL_STMT* stmt, const char* query, unsigned long length);
typedef int (*mysql_stmt_execute_fn)(MYSQL_STMT* stmt);
typedef int (*mysql_stmt_fetch_fn)(MYSQL_STMT* stmt);
typedef int (*mysql_stmt_close_fn)(MYSQL_STMT* stmt);
typedef unsigned int (*mysql_num_fields_fn)(MYSQL_RES* res);
typedef unsigned int (*mysql_stmt_field_count_fn)(MYSQL_STMT* stmt);
typedef MYSQL_RES* (*mysql_stmt_result_metadata_fn)(MYSQL_STMT* stmt);
typedef MYSQL_FIELD* (*mysql_fetch_fields_fn)(MYSQL_RES* meta);
typedef void (*mysql_stmt_bind_result_fn)(MYSQL_STMT* stmt, MYSQL_BIND* bind);
typedef int (*mysql_stmt_bind_param_fn)(MYSQL_STMT *stmt, MYSQL_BIND *bind);
typedef void (*mysql_close_fn)(MYSQL*);

extern mysql_init_fn tilc_mysql_init;
extern mysql_real_connect_fn tilc_mysql_real_connect;
extern mysql_query_fn tilc_mysql_query;
extern mysql_fetch_row_fn tilc_mysql_fetch_row;
extern mysql_stmt_init_fn tilc_mysql_stmt_init;
extern mysql_stmt_prepare_fn tilc_mysql_stmt_prepare;
extern mysql_stmt_execute_fn tilc_mysql_stmt_execute;
extern mysql_stmt_fetch_fn tilc_mysql_stmt_fetch;
extern mysql_stmt_close_fn tilc_mysql_stmt_close;
extern mysql_num_fields_fn tilc_mysql_num_fields;
extern mysql_stmt_field_count_fn tilc_mysql_stmt_field_count;
extern mysql_stmt_result_metadata_fn tilc_mysql_stmt_result_metadata;
extern mysql_fetch_fields_fn tilc_mysql_fetch_fields;
extern mysql_stmt_bind_result_fn tilc_mysql_stmt_bind_result;
extern mysql_stmt_bind_param_fn tilc_mysql_stmt_bind_param;
extern mysql_close_fn tilc_mysql_close;

void* TilcLoadMariaDb(const char* path);

#ifdef __cplusplus
}
#endif
