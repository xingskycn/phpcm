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

zend_function_entry cm_methods[] = {
    PHP_ME(cm,  __construct,     NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
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
