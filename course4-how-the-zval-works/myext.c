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
        zend_string *dup = zend_string_init(raw->val, raw->len, 0);
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

void zif_testzval(zend_execute_data *execute_data, zval *return_value) {
    TRACE("zif_testzval");

    // undef
    zval undef_zval;
    ZVAL_UNDEF(&undef_zval);
    assert(Z_TYPE(undef_zval) == IS_UNDEF);

    // null
    zval null_zval;
    ZVAL_NULL(&null_zval);
    assert(Z_TYPE(null_zval) == IS_NULL);

    // bool
    zval bool_zval;
    ZVAL_BOOL(&bool_zval, 1);
    assert(Z_TYPE(bool_zval) == IS_TRUE);

    // long
    zval long_zval;
    ZVAL_LONG(&long_zval, 1024);
    assert(Z_TYPE(long_zval) == IS_LONG);
    assert(Z_LVAL_P(&long_zval) == 1024);

    // string
    zval str_zval;
    zend_string *str = zend_string_init("IS_STRING", sizeof("IS_STRING") - 1, 0);
    ZVAL_STR(&str_zval, str);
    assert(Z_TYPE(str_zval) == IS_STRING);
    assert(strncmp(Z_STRVAL_P(&str_zval), "IS_STRING", Z_STRLEN_P(&str_zval)) == 0);
    zval_addref_p(&str_zval); // add 1 ref for arr_zval below
    zval_addref_p(&str_zval); // add 1 ref for ref_zval below
    zval_ptr_dtor(&str_zval);

    // array
    zval arr_zval;
    zend_array *arr = emalloc(sizeof(*arr));
    zend_hash_init(arr, 0, NULL, ZVAL_PTR_DTOR, 0);

    ZVAL_ARR(&arr_zval, arr);
    assert(Z_TYPE(arr_zval) == IS_ARRAY);
    assert((void*)arr_zval.value.counted == (void*)&(arr_zval.value.arr->gc));
    zend_symtable_str_update(arr, "str_zval", sizeof("str_zval") - 1, &str_zval);
    zval *zval_in_arr = zend_symtable_str_find(arr, "str_zval", sizeof("str_zval") - 1);
    assert(zval_in_arr);
    assert(Z_TYPE_P(zval_in_arr) == IS_STRING);
    assert(GC_REFCOUNT(Z_COUNTED_P(zval_in_arr)) == 2);
    uint32_t num_elems = zend_hash_num_elements(Z_ARRVAL_P(&arr_zval));
    assert(num_elems == 1);
    zval_ptr_dtor(&arr_zval);

    // reference
    zval ref_zval;
    zend_reference *ref = emalloc(sizeof(*ref));
    GC_REFCOUNT(ref) = 1;
    GC_TYPE_INFO(ref) = IS_REFERENCE;
    memcpy(&(ref->val), &str_zval, sizeof(str_zval));
    ZVAL_REF(&ref_zval, ref);
    assert(Z_TYPE(ref_zval) == IS_REFERENCE);
    zval_addref_p(&ref_zval); // add 1 ref for ref_ref_zval
    zval_ptr_dtor(&ref_zval);

    // zval ref to reference
    zval ref_ref_zval;
    memcpy(&ref_ref_zval, &ref_zval, sizeof(ref_zval));
    zval *real_zval = Z_REFVAL_P(&ref_ref_zval);
    zend_string *real_str = Z_STR_P(real_zval);
    assert(real_str == str);
    zend_string_release(real_str);
    ZVAL_STR(real_zval, zend_string_init("IS_STRING_TOO", sizeof("IS_STRING_TOO") - 1, 0));
    zval_ptr_dtor(&ref_ref_zval);

    // tips:
    // zval_addref_p/zval_delref_p can only be applied to zval with ref count
    zval double_zval;
    ZVAL_DOUBLE(&double_zval, 13.14);
    assert(!Z_REFCOUNTED_P(&double_zval));
    // the following line will trigger assert-coredump
    // zval_delref_p(&double_zval);

    // zval copy
    zval copy_from;
    ZVAL_STR(&copy_from, zend_string_init("test ZVAL_COPY", sizeof("test ZVAL_COPY") - 1, 0));

    zval copy_to1;
    ZVAL_COPY(&copy_to1, &copy_from);

    // same as ZVAL_COPY above
    zval copy_to2;
    memcpy(&copy_to2, &copy_from, sizeof(copy_from)); // same as ZVAL_COPY_VALUE
    if (Z_REFCOUNTED_P(&copy_from)) {
        zval_addref_p(&copy_to2);
    }

    // release all reference count
    zval_ptr_dtor(&copy_from);
    zval_ptr_dtor(&copy_from);
    zval_ptr_dtor(&copy_from);
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
        {"my_testzval", zif_testzval, NULL, 0, 0},
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
