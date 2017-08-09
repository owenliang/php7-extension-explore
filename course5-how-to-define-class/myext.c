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

extern void zim_myext_constructor(zend_execute_data *execute_data, zval *return_value);
extern void zim_myext_version(zend_execute_data *execute_data, zval *return_value);
extern void zim_myext_strtolower(zend_execute_data *execute_data, zval *return_value);
extern void zim_myext_strtoupper(zend_execute_data *execute_data, zval *return_value);

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
        {"__construct", zim_myext_constructor, NULL, 0, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR},
        {"version", zim_myext_version, NULL, 0, ZEND_ACC_PUBLIC},
        {"strtolower", zim_myext_strtolower, myext_strtolwer_arginfo, 1, ZEND_ACC_PUBLIC/*method flag*/},
        {"strtoupper", zim_myext_strtoupper, myext_strtoupper_arginfo, 1, ZEND_ACC_PUBLIC},
        {NULL, NULL, NULL, 0, 0},
};

zend_function_entry interface_funcs[] = {
        // fname,handler,arg_info,,num_args,flags
        {"version", NULL, NULL, 0, ZEND_ACC_PUBLIC | ZEND_ACC_ABSTRACT},
        {NULL, NULL, NULL, 0, 0},
};

zend_class_entry *myext_interface_handle = NULL; // interface handle
zend_class_entry *myext_class_handle = NULL; // base class handle
zend_class_entry *myext_child_class_handle = NULL; // child class handle

int extension_startup(int type, int module_number) {
    TRACE("extension_startup");
    zend_register_ini_entries(ini_defs, module_number);

    //
    //  Interface myext_interface
    //

    // interface defination
    zend_class_entry myext_interface_def;
    INIT_CLASS_ENTRY_EX(myext_interface_def, "myext_interface", sizeof("myext_interface") - 1, interface_funcs);

    // get interface handle
    assert(myext_interface_handle = zend_register_internal_interface(&myext_interface_def));

    //
    //  Class myext
    //

    // class defination
    zend_class_entry myext_class_def;
    INIT_CLASS_ENTRY_EX(myext_class_def, "myext", sizeof("myext") - 1, funcs);

    // get class handle
    assert(myext_class_handle = zend_register_internal_class(&myext_class_def));

    // implements interface
    assert(zend_do_implement_interface(myext_class_handle, myext_interface_handle) == SUCCESS);

    // add property to handle
    zval version_zval;
    ZVAL_PSTRING(&version_zval, "1.0.0"); // must be allocted from persistant memory
    assert(zend_declare_property(myext_class_handle, "version", sizeof("version") - 1, &version_zval, ZEND_ACC_PROTECTED) == SUCCESS);

    // add static property to handle
    zval author_zval;
    ZVAL_PSTRING(&author_zval, "owenliang");
    assert(zend_declare_property(myext_class_handle, "author", sizeof("author") - 1, &author_zval, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC) == SUCCESS);

    // add constant to handle
    zval build_date_zval;
    ZVAL_PSTRING(&build_date_zval, "2017-08-09 14:48");
    assert(zend_declare_class_constant(myext_class_handle, "BUILD_DATE", sizeof("build_date") - 1, &build_date_zval) == SUCCESS);

    //
    //  Class myext_child (inherit from Class myext)
    //
    zend_class_entry myext_child_class_def;
    INIT_CLASS_ENTRY_EX(myext_child_class_def, "myext_child", sizeof("myext_child") - 1, NULL);
    assert(myext_child_class_handle = zend_register_internal_class_ex(&myext_child_class_def, myext_class_handle));

    // final class, no more child class
    myext_child_class_handle->ce_flags |= ZEND_ACC_FINAL;

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

// Equals to PHP_METHOD(myext, strtolwer)
//
// zim_ means Zend Internal Method
void zim_myext_constructor(zend_execute_data *execute_data, zval *return_value) {
    TRACE("zim_myext_constructor");

    zval *this = &(execute_data->This);

    // class handle of this object
    zend_class_entry *class_handle = this->value.obj->ce;

    zend_string *ver_prop_name = zend_string_init("version", sizeof("version") - 1, 0);

    zend_string *new_ver_prop = zend_string_init("1.0.1", sizeof("1.0.1") - 1, 0);
    zval ver_zval;
    ZVAL_STR(&ver_zval, new_ver_prop);

    zend_update_property_ex(class_handle, this, ver_prop_name, &ver_zval);

    zend_string_release(ver_prop_name);
    zval_ptr_dtor(&ver_zval);
}

void zim_myext_version(zend_execute_data *execute_data, zval *return_value) {
    TRACE("zim_myext_version");

    // same as $this
    zval *this = &(execute_data->This);

    // class handle of this object
    zend_class_entry *class_handle = this->value.obj->ce;

    zval *ver_prop = zend_read_property(class_handle, this, "version", sizeof("version") - 1, 0, NULL/*always pass null*/);
    if (Z_TYPE_P(ver_prop) == IS_STRING) {
        zend_string *dup = zend_string_init(ver_prop->value.str->val, ver_prop->value.str->len, 0);
        ZVAL_STR(return_value, dup);
    } else {
        ZVAL_BOOL(return_value, 0);
    }
}

void zim_myext_strtolower(zend_execute_data *execute_data, zval *return_value) {
    TRACE("zim_myext_strtolower");

    int num_args = ZEND_CALL_NUM_ARGS(execute_data);
    zval *args = ZEND_CALL_ARG(execute_data, 1);

    TRACE("num_args=%d", num_args);
    *return_value = strcase_convert(&args[0], 1);
}

void zim_myext_strtoupper(zend_execute_data *execute_data, zval *return_value) {
    TRACE("zim_myext_strtoupper");

    int num_args = ZEND_CALL_NUM_ARGS(execute_data);
    zval *args = ZEND_CALL_ARG(execute_data, 1);

    TRACE("num_args=%d", num_args);
    *return_value = strcase_convert(&args[0], 0);
}

zend_module_entry module = {
    STANDARD_MODULE_HEADER_EX,  // size,zend_api,zend_debug,zts
    NULL,   // ini_entry
    NULL,   // deps
    "myext",    //name
    NULL,   // functions
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
