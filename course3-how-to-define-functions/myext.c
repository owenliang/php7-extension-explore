#include "myext.h"

#define MYEXT_INI_NAME_GITHUB "myext.github"
#define MYEXT_INI_VALUE_GITHUB "https://github.com/owenliang/php7-extension-explore"

// overwrite in php.ini like this:
//
// myext.github = "some value"
//
zend_ini_entry_def ini_defs[] = {
        // name,on_modify,mh_arg1,mh_arg2,mh_arg3,value,displayer,modifiable,name_length,value_length
        {MYEXT_INI_NAME_GITHUB, NULL, NULL, NULL, NULL, MYEXT_INI_VALUE_GITHUB, NULL, ZEND_INI_ALL, sizeof(MYEXT_INI_NAME_GITHUB) - 1, sizeof(MYEXT_INI_VALUE_GITHUB) - 1},
        {NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0},
};

int extension_startup(int type, int module_number) {
    TRACE("extension_startup");
    zend_register_ini_entries(ini_defs, module_number);
    return SUCCESS;
}

int extension_shutdown(int type, int module_number) {
    TRACE("extension_shutdown");
    return SUCCESS;
}

int extension_before_request(int type, int module_number) {
    TRACE("extension_before_request");
    const char* value = zend_ini_string(MYEXT_INI_NAME_GITHUB, sizeof(MYEXT_INI_NAME_GITHUB) - 1, 0);
    TRACE("ini: %s=%s", MYEXT_INI_NAME_GITHUB, value);
    return SUCCESS;
}

int extension_after_request(int type, int module_number) {
    TRACE("extension_after_request");
    return SUCCESS;
}

void extension_info(zend_module_entry *zend_module) {
    php_info_print_table_start();
    php_info_print_table_header(2, "myext support", "enabled");
    php_info_print_table_row(2, "author", "owenliang");
    php_info_print_table_row(2, "course name", "course1-how-to-export-a-module");
    php_info_print_table_end();
}

zval strcase_convert(const zval *zv, int lowercase) {
    zval retval;

    zend_uchar zv_type = zval_get_type(zv); // equals to Z_TYPE_P(zv)
    if (zv_type == IS_STRING) {
        zend_string *raw = zv->value.str;   // Z_STR_P(zv)
        zend_string *dup = zend_string_dup(raw, 0);
        size_t i;
        for (i = 0; i < dup->len/*ZSTR_LEN*/; ++i) {
            if (lowercase) {
                dup->val[i] = tolower(dup->val[i]);
            } else {
                dup->val[i] = toupper(dup->val[i]);
            }
        }
        ZVAL_STR(&retval, dup);
    } else {
        ZVAL_BOOL(&retval, 0);
    }
    return retval;
}

// Equals to PHP_FUNCTION(strtolwer)
//
// zif_ means Zend Internal Function
void zif_strtolower(zend_execute_data *execute_data, zval *return_value) {
    TRACE("zif_strtolower");

    int num_args = ZEND_CALL_NUM_ARGS(execute_data);
    zval *args = ZEND_CALL_ARG(execute_data, 1);

    TRACE("num_args=%d", num_args);
    *return_value = strcase_convert(&args[0], 1);
}

void zif_strtoupper(zend_execute_data *execute_data, zval *return_value) {
    TRACE("zif_strtoupper");

    int num_args = ZEND_CALL_NUM_ARGS(execute_data);
    zval *args = ZEND_CALL_ARG(execute_data, 1);

    TRACE("num_args=%d", num_args);
    *return_value = strcase_convert(&args[0], 0);
}

// zif_strtolower's params defination
zend_internal_arg_info myext_strtolwer_arginfo[] = {
        // required_num_args(interger stored in pointer)
        {(const char *)(zend_uintptr_t)1, NULL, 0, 0, 0, 0},
        // name, class_name, type_hint, pass_by_reference, allow_null, is_variadic
        {"string", NULL, IS_STRING, 0, 0, 0},
};
zend_internal_arg_info myext_strtoupper_arginfo[] = {
        {(const char *)(zend_uintptr_t)1, NULL, 0, 0, 0, 0},
        {"string", NULL, IS_STRING, 0, 0, 0},
};

zend_function_entry funcs[] = {
        // fname,handler,arg_info,,num_args,flags
        {"my_strtolower", zif_strtolower, myext_strtolwer_arginfo, 1, 0},
        {"my_strtoupper", zif_strtoupper, myext_strtoupper_arginfo, 1, 0},
        {NULL, NULL, NULL, 0, 0},
};

zend_module_entry module = {
    STANDARD_MODULE_HEADER_EX,  // size,zend_api,zend_debug,zts
    NULL,   // ini_entry
    NULL,   // deps
    "myext",    //name
    funcs,   // functions
    extension_startup,  // module_startup_func
    extension_shutdown, // module_shutdown_func
    extension_before_request,   // request_startup_func
    extension_after_request,    // request_shutdown_func
    extension_info, // info_func
    "1.0",  // version
    // globals_size,globals_ptr,globals_ctor,globals_dtor,post_deactivate_func,module_started,type,
    // handle,module_number,build_id
    STANDARD_MODULE_PROPERTIES,
};

zend_module_dep deps[] = {
        {"standard", NULL, NULL, MODULE_DEP_REQUIRED},
        { NULL, NULL, NULL, 0},
};

ZEND_DLEXPORT zend_module_entry *get_module() {
    module.deps = deps;
    return &module;
}
