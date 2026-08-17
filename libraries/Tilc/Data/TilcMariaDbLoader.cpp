#include "Tilc/Data/TilcMariaDbLoader.h"
#ifdef __linux__
    #include <dlfcn.h>
#elifdef _WIN32
    #include <windows.h>
#endif
#include <stdio.h>

mysql_init_fn tilc_mysql_init{};
mysql_real_connect_fn tilc_mysql_real_connect{};
mysql_query_fn tilc_mysql_query{};
mysql_fetch_row_fn tilc_mysql_fetch_row{};
mysql_stmt_init_fn tilc_mysql_stmt_init{};
mysql_stmt_prepare_fn tilc_mysql_stmt_prepare{};
mysql_stmt_execute_fn tilc_mysql_stmt_execute{};
mysql_stmt_store_result_fn tilc_mysql_stmt_store_result{};
mysql_stmt_fetch_fn tilc_mysql_stmt_fetch{};
mysql_stmt_close_fn tilc_mysql_stmt_close{};
mysql_stmt_error_fn tilc_mysql_stmt_error{};
mysql_num_fields_fn tilc_mysql_num_fields{};
mysql_stmt_field_count_fn tilc_mysql_stmt_field_count{};
mysql_stmt_result_metadata_fn tilc_mysql_stmt_result_metadata{};
mysql_fetch_fields_fn tilc_mysql_fetch_fields{};
mysql_stmt_bind_result_fn tilc_mysql_stmt_bind_result{};
mysql_stmt_bind_param_fn tilc_mysql_stmt_bind_param{};
mysql_close_fn tilc_mysql_close{};
mysql_error_fn tilc_mysql_error{};

void* TilcLoadMariaDb(const char* path)
{
    void* handle{};
#ifdef __linux__
    handle = dlopen(path, RTLD_NOW | RTLD_GLOBAL);
#elifdef _WIN32
    handle = LoadLibraryA(path);
#endif
    if (!handle)
    {
        return nullptr;
    }

#ifdef __linux__
    tilc_mysql_init = (MYSQL* (*)(MYSQL*))dlsym(handle, "mysql_init");
    tilc_mysql_real_connect = (MYSQL* (*)(MYSQL*, const char*, const char*, const char*, const char*, unsigned int, const char*, unsigned long))dlsym(handle, "mysql_real_connect");
    tilc_mysql_query = (int (*)(MYSQL*, const char*))dlsym(handle, "mysql_query");
    tilc_mysql_fetch_row = (MYSQL_ROW (*)(MYSQL_RES*))dlsym(handle, "mysql_fetch_row");
    tilc_mysql_stmt_init = (MYSQL_STMT* (*)(MYSQL*))dlsym(handle, "mysql_stmt_init");
    tilc_mysql_stmt_prepare = (int (*)(MYSQL_STMT*, const char*, unsigned long))dlsym(handle, "mysql_stmt_prepare");
    tilc_mysql_stmt_execute = (int (*)(MYSQL_STMT*))dlsym(handle, "mysql_stmt_execute");
    tilc_mysql_stmt_store_result = (int (*)(MYSQL_STMT*))dlsym(handle, "mysql_stmt_store_result");
    tilc_mysql_stmt_fetch = (int (*)(MYSQL_STMT*))dlsym(handle, "mysql_stmt_fetch");
    tilc_mysql_stmt_close = (int (*)(MYSQL_STMT*))dlsym(handle, "mysql_stmt_close");
    tilc_mysql_stmt_error = (const char* (*)(MYSQL_STMT*))dlsym(handle, "mysql_stmt_error");
    tilc_mysql_num_fields = (unsigned int (*)(MYSQL_RES*))dlsym(handle, "mysql_num_fields");
    tilc_mysql_stmt_field_count = (unsigned int (*)(MYSQL_STMT*))dlsym(handle, "mysql_stmt_field_count");
    tilc_mysql_stmt_result_metadata = (MYSQL_RES* (*)(MYSQL_STMT*))dlsym(handle, "mysql_stmt_result_metadata");
    tilc_mysql_fetch_fields = (MYSQL_FIELD* (*)(MYSQL_RES*))dlsym(handle, "mysql_fetch_fields");
    tilc_mysql_stmt_bind_result = (void (*)(MYSQL_STMT*, MYSQL_BIND*))dlsym(handle, "mysql_stmt_bind_result");
    tilc_mysql_stmt_bind_param = (int (*)(MYSQL_STMT*, MYSQL_BIND*))dlsym(handle, "mysql_stmt_bind_param");
    tilc_mysql_close = (void (*)(MYSQL*))dlsym(handle, "mysql_close");
    tilc_mysql_error = (const char* (*)(MYSQL*))dlsym(handle, "mysql_error");
#elifdef _WIN32
    tilc_mysql_init = (MYSQL * (*)(MYSQL*))GetProcAddress((HMODULE)handle, "mysql_init");
    tilc_mysql_real_connect = (MYSQL * (*)(MYSQL*, const char*, const char*, const char*, const char*, unsigned int, const char*, unsigned long))GetProcAddress((HMODULE)handle, "mysql_real_connect");
    tilc_mysql_query = (int (*)(MYSQL*, const char*))GetProcAddress((HMODULE)handle, "mysql_query");
    tilc_mysql_fetch_row = (MYSQL_ROW(*)(MYSQL_RES*))GetProcAddress((HMODULE)handle, "mysql_fetch_row");
    tilc_mysql_stmt_init = (MYSQL_STMT * (*)(MYSQL*))GetProcAddress((HMODULE)handle, "mysql_stmt_init");
    tilc_mysql_stmt_prepare = (int (*)(MYSQL_STMT*, const char*, unsigned long))GetProcAddress((HMODULE)handle, "mysql_stmt_prepare");
    tilc_mysql_stmt_execute = (int (*)(MYSQL_STMT*))GetProcAddress((HMODULE)handle, "mysql_stmt_execute");
    tilc_mysql_stmt_store_result = (int (*)(MYSQL_STMT*))GetProcAddress((HMODULE)handle, "mysql_stmt_store_result");
    tilc_mysql_stmt_fetch = (int (*)(MYSQL_STMT*))GetProcAddress((HMODULE)handle, "mysql_stmt_fetch");
    tilc_mysql_stmt_close = (int (*)(MYSQL_STMT*))GetProcAddress((HMODULE)handle, "mysql_stmt_close");
    tilc_mysql_stmt_error = (const char* (*)(MYSQL_STMT*))GetProcAddress((HMODULE)handle, "mysql_stmt_error");
    tilc_mysql_num_fields = (unsigned int (*)(MYSQL_RES*))GetProcAddress((HMODULE)handle, "mysql_num_fields");
    tilc_mysql_stmt_field_count = (unsigned int (*)(MYSQL_STMT*))GetProcAddress((HMODULE)handle, "mysql_stmt_field_count");
    tilc_mysql_stmt_result_metadata = (MYSQL_RES * (*)(MYSQL_STMT*))GetProcAddress((HMODULE)handle, "mysql_stmt_result_metadata");
    tilc_mysql_fetch_fields = (MYSQL_FIELD * (*)(MYSQL_RES*))GetProcAddress((HMODULE)handle, "mysql_fetch_fields");
    tilc_mysql_stmt_bind_result = (void (*)(MYSQL_STMT*, MYSQL_BIND*))GetProcAddress((HMODULE)handle, "mysql_stmt_bind_result");
    tilc_mysql_stmt_bind_param = (int (*)(MYSQL_STMT*, MYSQL_BIND*))GetProcAddress((HMODULE)handle, "mysql_stmt_bind_param");
    tilc_mysql_close = (void (*)(MYSQL*))GetProcAddress((HMODULE)handle, "mysql_close");
    tilc_mysql_error = (const char* (*)(MYSQL*))GetProcAddress((HMODULE)handle, "mysql_error");
#endif

    if (!tilc_mysql_init || !tilc_mysql_real_connect || !tilc_mysql_query || !tilc_mysql_fetch_row || !tilc_mysql_stmt_init ||
        !tilc_mysql_stmt_prepare || !tilc_mysql_stmt_execute || !tilc_mysql_stmt_store_result || !tilc_mysql_stmt_fetch ||
        !tilc_mysql_stmt_close || !tilc_mysql_stmt_error || !tilc_mysql_num_fields ||
        !tilc_mysql_stmt_field_count || !tilc_mysql_stmt_result_metadata ||
        !tilc_mysql_fetch_fields || !tilc_mysql_stmt_bind_result ||
        !tilc_mysql_stmt_bind_param ||
        !tilc_mysql_close || !tilc_mysql_error)
    {
        if (handle)
        {
#ifdef __linux__
            dlclose(handle);
#elifdef _WIN32
            FreeLibrary((HMODULE)handle);
#endif
        }
        return nullptr;
    }

    return handle;
}
