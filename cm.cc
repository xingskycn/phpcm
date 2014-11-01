#include <iostream>

#include "php_cm.hpp"
#include "cm_class.hpp"

zend_class_entry *cm_ce;

zend_object_handlers cm_object_handlers;

struct cm_object {
    zend_object std;
    Cm *cm;
};

void cm_free_storage(void *object TSRMLS_DC)
{
    cm_object *obj = (cm_object *)object;
    delete obj->cm;

    zend_hash_destroy(obj->std.properties);
    FREE_HASHTABLE(obj->std.properties);

    efree(obj);
}

zend_object_value cm_create_handler(zend_class_entry *type TSRMLS_DC)
{
    zval *tmp;
    zend_object_value retval;

    cm_object *obj = (cm_object *)emalloc(sizeof(cm_object));
    memset(obj, 0, sizeof(cm_object));
    obj->std.ce = type;

    ALLOC_HASHTABLE(obj->std.properties);
    zend_hash_init(obj->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
    object_properties_init((zend_object*) &(obj->std), type);

    retval.handle = zend_objects_store_put(obj, NULL,
        cm_free_storage, NULL TSRMLS_CC);
    retval.handlers = &cm_object_handlers;

    return retval;
}

PHP_METHOD(cm, __construct)
{
//    long maxGear;
    Cm *cm = NULL;
    zval *object = getThis();

//    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &maxGear) == FAILURE) {
//        RETURN_NULL();
//    }

    cm = new Cm();
    cm_object *obj = (cm_object *)zend_object_store_get_object(object TSRMLS_CC);
    obj->cm = cm;
}

//bool remove(char *key);
PHP_METHOD(cm, remove)
{
    Cm *cm;
    cm_object *obj = (cm_object *)zend_object_store_get_object(
        getThis() TSRMLS_CC);
    cm = obj->cm;
    if (cm != NULL) {
        char *key;
        int key_len;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &key_len) == FAILURE) {
            RETURN_NULL();
        }
        if (strnlen(key, key_len+1) != key_len) {
            RETURN_STRING("Non-string key!", 1);
        }

        RETURN_BOOL(cm->remove(key));
    }
}


//char* get(char *key, size_t *return_value_length)
PHP_METHOD(cm, get)
{
    Cm *cm;
    cm_object *obj = (cm_object *)zend_object_store_get_object(
        getThis() TSRMLS_CC);
    cm = obj->cm;
    if (cm != NULL) {
        char *key;
        int key_len;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &key_len) == FAILURE) {
            RETURN_NULL();
        }
        if (strnlen(key, key_len+1) != key_len) {
            RETURN_STRING("Non-string key!", 1);
        }

        size_t value_length=0;
        char* value;
        value = cm->get(key, &value_length);
        if (value == NULL) {
            RETURN_NULL();
        } else {
            RETURN_STRINGL(value, value_length, 1);
        }
    }
}

//bool set(char *key, char *value, bool isDependency, char *dependency, long expire)
PHP_METHOD(cm, set)
{
    Cm *cm;
    cm_object *obj = (cm_object *)zend_object_store_get_object(
        getThis() TSRMLS_CC);
    cm = obj->cm;
    if (cm != NULL) {
        char *key;
        int key_len;
        char *value;
        int value_len;
        char *dependency;
        int dependency_len=0;
        long expire=0;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|sl", &key, &key_len, &value, &value_len, &dependency, &dependency_len, &expire) == FAILURE) {
            RETURN_NULL();
        }
        if (strnlen(key, key_len+1) != key_len) {
            RETURN_STRING("Non-string key!", 1);
        }
        if (dependency_len == 0) {
            dependency = new char [1];
            strcpy(dependency, "");
        }
        if (strnlen(dependency, dependency_len+1) != dependency_len) {
            RETURN_STRING("Non-string dependency key!", 1);
        }

        RETURN_BOOL(cm->set(key, value, value_len, dependency, expire));
    }
    RETURN_NULL();
}

zend_function_entry cm_methods[] = {
    PHP_ME(cm,  __construct,     NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(cm,  set,             NULL, ZEND_ACC_PUBLIC)
    PHP_ME(cm,  get,             NULL, ZEND_ACC_PUBLIC)
    PHP_ME(cm,  remove,          NULL, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

PHP_MINIT_FUNCTION(cm)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "Cm", cm_methods);
    cm_ce = zend_register_internal_class(&ce TSRMLS_CC);
    cm_ce->create_object = cm_create_handler;
    memcpy(&cm_object_handlers,
        zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    cm_object_handlers.clone_obj = NULL;
    return SUCCESS;
}

zend_module_entry cm_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    PHP_CM_EXTNAME,
    NULL,        /* Functions */
    PHP_MINIT(cm),        /* MINIT */
    NULL,        /* MSHUTDOWN */
    NULL,        /* RINIT */
    NULL,        /* RSHUTDOWN */
    NULL,        /* MINFO */
#if ZEND_MODULE_API_NO >= 20010901
    PHP_CM_EXTVER,
#endif
    STANDARD_MODULE_PROPERTIES
};

ZEND_GET_MODULE(cm)
