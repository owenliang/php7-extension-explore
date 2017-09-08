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
extern void zim_myext_strcase_convert(zend_execute_data *execute_data, zval *return_value);
extern void zim_myext_child_version(zend_execute_data *execute_data, zval *return_value);
extern void zim_myext_print_author(zend_execute_data *execute_data, zval *return_value);

extern void zif_myext_test_object(zend_execute_data *execute_data, zval *return_value);
extern void zif_myext_test_array(zend_execute_data *execute_data, zval *return_value);
extern void zif_myext_test_resource(zend_execute_data *execute_data, zval *return_value);
extern void zif_myext_test_include(zend_execute_data *execute_data, zval *return_value);
extern void zif_myext_test_exception(zend_execute_data *execute_data, zval *return_value);
extern void zif_myext_test_error(zend_execute_data *execute_data, zval *return_value);

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
        {"strcase_convert", zim_myext_strcase_convert, NULL, 2, ZEND_ACC_PRIVATE},
        {"print_author", zim_myext_print_author, NULL, 0, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC},
        {NULL, NULL, NULL, 0, 0},
};

zend_function_entry interface_funcs[] = {
        // fname,handler,arg_info,,num_args,flags
        {"version", NULL, NULL, 0, ZEND_ACC_PUBLIC | ZEND_ACC_ABSTRACT},
        {NULL, NULL, NULL, 0, 0},
};

zend_function_entry final_funcs[] = {
        // fname,handler,arg_info,,num_args,flags
        {"version", zim_myext_child_version, NULL, 0, ZEND_ACC_PUBLIC},
        {NULL, NULL, NULL, 0, 0},
};

zend_function_entry global_funcs[] = {
        // fname,handler,arg_info,,num_args,flags
        {"test_object", zif_myext_test_object, NULL, 0, 0},
        {"test_array", zif_myext_test_array, NULL, 0, 0},
        {"test_resource", zif_myext_test_resource, 0, 0},
        {"test_include", zif_myext_test_include, 0, 0},
        {"test_exception", zif_myext_test_exception, 0, 0},
        {"test_error", zif_myext_test_error, 0, 0},
        {NULL, NULL, NULL, 0, 0},
};

zend_class_entry *myext_interface_handle = NULL; // interface handle
zend_class_entry *myext_class_handle = NULL; // base class handle
zend_class_entry *myext_child_class_handle = NULL; // child class handle

// resource id
int myext_string_resource_id = 0;

// resource type description
#define MYEXT_STRING_RESOURCE_DTOR "myext_string_resource"

// resource destructor callback
void myext_string_resource_dtor(zend_resource *res) {
    assert(res->type == myext_string_resource_id);
    free(res->ptr);
}

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
    INIT_CLASS_ENTRY_EX(myext_child_class_def, "myext_child", sizeof("myext_child") - 1, final_funcs);
    assert(myext_child_class_handle = zend_register_internal_class_ex(&myext_child_class_def, myext_class_handle));

    // final class, no more child class
    myext_child_class_handle->ce_flags |= ZEND_ACC_FINAL;

    // register constant
    zend_constant c;
    c.name = zend_string_init("GITHUB", sizeof("GITHUB") - 1, 1); // persistant memory
    ZVAL_STR(&c.value, zend_string_init("https://github.com/owenliang/php7-extension-explore",
            sizeof("https://github.com/owenliang/php7-extension-explore"), 1)); // persistant memory
    c.flags = CONST_CS | CONST_PERSISTENT;
    c.module_number = module_number;
    assert(zend_register_constant(&c) == SUCCESS);

    // register resource type
    myext_string_resource_id = zend_register_list_destructors_ex(myext_string_resource_dtor, NULL, MYEXT_STRING_RESOURCE_DTOR, module_number);
    assert(myext_string_resource_id != FAILURE);

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

    // try active jit super globals
    zend_is_auto_global_str("_SERVER", sizeof("_SERVER") - 1);

    // find it in global symbol table
    zval *server = zend_hash_str_find(&EG(symbol_table), "_SERVER", sizeof("_SERVER") - 1);
    assert(server != NULL);
    assert(Z_TYPE_P(server) == IS_ARRAY);

    // var_dump($_SERVER)
    zval func_name;
    ZVAL_STR(&func_name, zend_string_init("var_dump", sizeof("var_dump") - 1, 0));
    zval retval;
    assert(call_user_function(&EG(function_table), NULL, &func_name, &retval, 1, server) == SUCCESS);
    zval_ptr_dtor(&func_name);
    zval_ptr_dtor(&retval);
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

// Myext's static method
void zim_myext_print_author(zend_execute_data *execute_data, zval *return_value) {
    TRACE("zim_myext_print_author");
    php_output_write("author=owenliang\n", sizeof("author=owenliang\n") - 1);
    ZVAL_BOOL(return_value, 1);
}


// global function
void zif_myext_test_object(zend_execute_data *execute_data, zval *return_value) {
    TRACE("zif_myext_test_object");

    // call myext's static method: print_author
    zend_string *myext_classname = zend_string_init("myext", sizeof("myext") - 1, 0);
    zend_class_entry *myext_handle = zend_lookup_class(myext_classname);
    zend_string_release(myext_classname);
    assert(myext_handle == myext_class_handle);

    zval retval;
    zend_fcall_info fci = {
            size: sizeof(zend_fcall_info),
            retval: &retval,
            params: NULL,
            object: NULL,
            no_separation: 1,
            param_count: 0,
    };
    ZVAL_UNDEF(&fci.function_name);

    zval *print_author_func = zend_hash_str_find(&(myext_handle->function_table), "print_author", sizeof("print_author") - 1);

    zend_fcall_info_cache fcic = {
            initialized: 1,
            function_handler: print_author_func->value.func,
            calling_scope: myext_handle,
            called_scope: NULL,
            object: NULL,
    };

    assert(zend_call_function(&fci, &fcic) == SUCCESS);
    assert(Z_TYPE_P(&retval) == IS_TRUE);

    // new a myext object
    zval myext_obj;
    assert(object_init_ex(&myext_obj, myext_handle) == SUCCESS);

    // call object's __construct
    zval ctor_name;
    zval ctor_retval;
    ZVAL_STR(&ctor_name, zend_string_init("__construct", sizeof("__construct") - 1, 0));
    assert(call_user_function(&EG(function_table), &myext_obj, &ctor_name, &ctor_retval, 0, NULL) == SUCCESS);
    zval_ptr_dtor(&ctor_name);

    // call object's method
    zval func_name;
    ZVAL_STR(&func_name, zend_string_init("strtolower", sizeof("strtolower") - 1, 0));
    zval param;
    ZVAL_STR(&param, zend_string_init("OWENLIANG", sizeof("OWENLIANG") - 1, 0));
    zval retval2;
    assert(call_user_function(&EG(function_table), &myext_obj, &func_name, &retval2, 1, &param) == SUCCESS);
    TRACE("$myext_obj->strtolower(OWENLIANG)=%.*s", retval2.value.str->len, retval2.value.str->val);
    zval_ptr_dtor(&func_name);
    zval_ptr_dtor(&param);
    zval_ptr_dtor(&retval2);

    // free object
    zval_ptr_dtor(&myext_obj);
}

void zim_myext_child_version(zend_execute_data *execute_data, zval *return_value) {
    TRACE("zim_myext_child_version");

    zval *this = &(execute_data->This);

    // call parent's version()
    zval retval;
    zend_fcall_info fci = {
            size: sizeof(zend_fcall_info),
            retval: &retval,
            params: NULL,
            object: this->value.obj,
            no_separation: 1,
            param_count: 0,
    };
    ZVAL_UNDEF(&fci.function_name);

    // find parent's version method
    zval *parent_version_func = zend_hash_str_find(&(this->value.obj->ce->parent->function_table), "version", sizeof("version") - 1);

    zend_fcall_info_cache fcic = {
            initialized: 1,
            function_handler: parent_version_func->value.func,
            calling_scope: this->value.obj->ce->parent,
            called_scope: this->value.obj->ce,
            object: this->value.obj,
    };

    assert(zend_call_function(&fci, &fcic) == SUCCESS);
    assert(Z_TYPE_P(&retval) == IS_STRING);

    int len = retval.value.str->len + sizeof(".child") - 1;
    char *child_version = emalloc(len);
    memcpy(child_version, retval.value.str->val, retval.value.str->len);
    memcpy(child_version + retval.value.str->len, ".child", sizeof(".child") - 1);

    ZVAL_STR(return_value, zend_string_init(child_version, len, 0));
    efree(child_version);
    zval_ptr_dtor(&retval);
}

void zim_myext_strcase_convert(zend_execute_data *execute_data, zval *return_value) {
    TRACE("zim_myext_strcase_convert");

    int num_args = ZEND_CALL_NUM_ARGS(execute_data);
    zval *args = ZEND_CALL_ARG(execute_data, 1);

    TRACE("num_args=%d", num_args);

    zval *zv = &args[0];
    zval *lowercase_zval = &args[1];

    convert_to_string(zv);
    convert_to_boolean(lowercase_zval);

    zend_string *raw = zv->value.str;   // Z_STR_P(zv)
    zend_string *dup = zend_string_init(raw->val, raw->len, 0);
    size_t i;
    for (i = 0; i < dup->len/*ZSTR_LEN*/; ++i) {
        if (Z_TYPE_P(lowercase_zval) == IS_TRUE) {
            dup->val[i] = tolower(dup->val[i]);
        } else {
            dup->val[i] = toupper(dup->val[i]);
        }
    }
    ZVAL_STR(return_value, dup);
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

    // read constant
    zend_string *cname = zend_string_init("GITHUB", sizeof("GITHUB") - 1, 0);
    zval *c_github = zend_get_constant(cname);
    assert(Z_TYPE_P(c_github) == IS_STRING);
    TRACE("zend_get_constant(GITHUB)=%.*s", Z_STRLEN_P(c_github), Z_STRVAL_P(c_github));
    zend_string_release(cname);
}

void zim_myext_strtolower(zend_execute_data *execute_data, zval *return_value) {
    TRACE("zim_myext_strtolower");

    zval *this = &(execute_data->This);

    int num_args = ZEND_CALL_NUM_ARGS(execute_data);
    zval *args = ZEND_CALL_ARG(execute_data, 1);

    TRACE("num_args=%d", num_args);

    zend_string *func_name = zend_string_init("strcase_convert", sizeof("strcase_convert") - 1, 0);
    zval func_name_zval;
    ZVAL_STR(&func_name_zval, func_name);

    zval params[2];
    memcpy(&params[0], &args[0], sizeof(args[0]));
    ZVAL_BOOL(&params[1], 1);

    // call method
    assert(call_user_function(&EG(function_table), this, &func_name_zval, return_value, 2, params) == SUCCESS);
    zval_ptr_dtor(&func_name_zval);
}

void zim_myext_strtoupper(zend_execute_data *execute_data, zval *return_value) {
    TRACE("zim_myext_strtoupper");

    int num_args = ZEND_CALL_NUM_ARGS(execute_data);
    zval *args = ZEND_CALL_ARG(execute_data, 1);

    TRACE("num_args=%d", num_args);

    // call global function
    zend_string *func_name = zend_string_init("strtoupper", sizeof("strtoupper") - 1, 0);
    zval func_name_zval;
    ZVAL_STR(&func_name_zval, func_name);

    call_user_function(&EG(function_table), NULL, &func_name_zval, return_value, 1, &args[0]);
    zval_ptr_dtor(&func_name_zval);
}

void zif_myext_test_array(zend_execute_data *execute_data, zval *return_value) {
    // init arr
    zval arr_zval;
    assert(array_init(&arr_zval) == SUCCESS);

    // add k-v
    add_assoc_long(&arr_zval, "date", 20170811);
    assert(zend_hash_str_exists(arr_zval.value.arr, "date", sizeof("date") - 1));

    // add v
    assert(add_next_index_string(&arr_zval, "hahaha") == SUCCESS);

    // arr count
    assert(zend_hash_num_elements(arr_zval.value.arr) == 2);

    // traversal arr
    zend_array *arr = arr_zval.value.arr;
    int i;
    for (i = 0; i < arr->nNumUsed; ++i) {
        zval *val = &(arr->arData[i].val);
        // handle indirect zval
        if (Z_TYPE_P(val) == IS_INDIRECT) {
            val = Z_INDIRECT_P(val);
        }
        // empty slots
        if (Z_TYPE_P(val) == IS_UNDEF) {
            continue;
        }

        if (arr->arData[i].key) { // must be array["date"]
            TRACE("arr['%.*s']=%ld", arr->arData[i].key->len, arr->arData[i].key->val, val->value.lval);
        } else { // must be array[0]
            TRACE("arr[%ld]=%.*s", arr->arData[i].h, val->value.str->len, val->value.str->val);
        }
    }

    // find key
    zval *zv_in_arr = zend_hash_str_find_ind(arr_zval.value.arr, "date", sizeof("date") - 1);
    assert(zv_in_arr->value.lval == 20170811);

    // del string key
    assert(zend_hash_str_del(arr_zval.value.arr, "date", sizeof("date") - 1) == SUCCESS);

    // del index key
    assert(zend_hash_index_del(arr_zval.value.arr, 0) == SUCCESS);

    // release arr
    zval_ptr_dtor(&arr_zval);
}

void zif_myext_test_resource(zend_execute_data *execute_data, zval *return_value) {
    TRACE("zif_myext_test_resource");
    char *string = strdup("i am a string resource");
    zend_resource *res = zend_register_resource(string, myext_string_resource_id);
    assert(GC_REFCOUNT(res) == 1);

    // wrappped with zval, refcount=2
    zval res_zval;
    ZVAL_RES(&res_zval, res);
    zval_addref_p(&res_zval);
    assert(GC_REFCOUNT(res) == 2);

    // release resource directly, left refcount=1
    zend_list_delete(res);
    assert(GC_REFCOUNT(res) == 1);

    // validate and get resource ptr
    char *s = zend_fetch_resource_ex(&res_zval, MYEXT_STRING_RESOURCE_DTOR, myext_string_resource_id);
    assert(strcmp(s, "i am a string resource") == 0);

    // release resource through zval, left refcount=0, zend_list_free is called
    zval_ptr_dtor(&res_zval);
}

void zif_myext_test_include(zend_execute_data *execute_data, zval *return_value) {
    TRACE("zif_myext_test_include");

    int num_args = ZEND_CALL_NUM_ARGS(execute_data);
    zval *args = ZEND_CALL_ARG(execute_data, 1);
    assert(num_args == 1);

    // relative path to absolute full path
    char realpath[MAXPATHLEN];
    if (!virtual_realpath(Z_STRVAL(args[0]), realpath)) {
        ZVAL_BOOL(return_value, 0);
        return;
    }

    TRACE("realpath=%s", realpath);

    zend_string *filename = zend_string_init(realpath, strlen(realpath), 0);

    // already loaded before
    zval *existed = zend_hash_find(&EG(included_files), filename);
    if (existed) {
        zend_string_release(filename);
        ZVAL_BOOL(return_value, 0);
        return;
    }

    // not opened file handle
    zend_file_handle file_handle = {
            filename:  filename->val,
            free_filename: 0,
            type: ZEND_HANDLE_FILENAME,
            opened_path: NULL,
            handle: {fp: NULL},
    };

    // compile file into opcode
    zend_op_array *op_array = zend_compile_file(&file_handle, ZEND_INCLUDE);
    assert(op_array);

    // close file handle
    zend_destroy_file_handle(&file_handle);

    // mark file is included
    zval empty_zv;
    ZVAL_NULL(&empty_zv);
    assert(zend_hash_add_new(&EG(included_files), filename, &empty_zv)); // 1 ref added inner
    zend_string_release(filename);

    // execute opcode
    zval result;
    ZVAL_UNDEF(&result);
    zend_execute(op_array, &result);

    // free opcode
    destroy_op_array(op_array);
    efree(op_array);

    // array included from php file
    assert(Z_TYPE(result) == IS_ARRAY);
    *return_value = result;
}

void zif_myext_test_exception(zend_execute_data *execute_data, zval *return_value) {
    TRACE("zif_myext_test_exception");

    int num_args = ZEND_CALL_NUM_ARGS(execute_data);
    zval *args = ZEND_CALL_ARG(execute_data, 1);
    assert(num_args == 1 && Z_TYPE_P(args) == IS_LONG);

    // throw self-defined exception
    if (Z_LVAL_P(args) == 1) {
        // inherit from base exception
        zend_function_entry nofuncs[] = {
                // fname,handler,arg_info,,num_args,flags
                {NULL, NULL, NULL, 0, 0},
        };
        zend_class_entry my_exception_def;
        INIT_CLASS_ENTRY_EX(my_exception_def, "MyException", sizeof("MyException") - 1, nofuncs);
        zend_class_entry *my_exception_handle = zend_register_internal_class_ex(&my_exception_def, zend_ce_exception);
        assert(my_exception_handle);
        // throw myexception
        zend_throw_exception(my_exception_handle, "I AM MY EXCEPTION", 10086);
    } else {
        // throw base exception directly
        zend_throw_exception(NULL, "I AM DEFAULT EXCEPTION", 10087);
    }
}

void zif_myext_test_error(zend_execute_data *execute_data, zval *return_value) {
    TRACE("zif_myext_test_error");

    php_error_docref(NULL, E_WARNING, "I AM %s", "WARNING");

    php_error_docref2(NULL, "string $parmas1", "integer $params2", E_NOTICE, "I AM %s", "JUST A NOTICE");

    // fatal error will stop executing script
    php_error_docref1(NULL, "string $params1", E_ERROR, "I AM %s", "FATAL ERROR");
}

zend_module_entry module = {
    STANDARD_MODULE_HEADER_EX,  // size,zend_api,zend_debug,zts
    NULL,   // ini_entry
    NULL,   // deps
    "myext",    //name
    global_funcs,   // functions
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
