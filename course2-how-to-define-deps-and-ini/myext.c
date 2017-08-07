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
