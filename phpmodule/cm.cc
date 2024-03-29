#include <iostream>

#include "php_cm.hpp"
#include "cm_interface.hpp"
#include "cm_wrapper.hpp"
#include "cm_class.hpp"
#include "cm_ServerPair.hpp"
#include "cm_RVal.hpp"
#include "cm_debug.hpp"

zend_class_entry *cm_ce;

zend_object_handlers cm_object_handlers;

struct cm_object {
    zend_object std;
    CmInterface *cm;
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

ServerPair makeSPfromPhpArray(zval **data)
{
    ServerPair server;
    server.port = 0;
    server.isReplica = false;
    if (Z_TYPE_PP(data) == IS_ARRAY) {
	HashTable *z_conf_row;
	z_conf_row = Z_ARRVAL_PP(data);
	zval **entry;

	if (zend_hash_find(z_conf_row, "host", sizeof("host"), (void **) &entry) == SUCCESS) {
	    if (Z_TYPE_PP(entry) == IS_STRING) {
	        server.serverName = Z_STRVAL_PP(entry);
	        server.stable = true;
	    } else {
		zend_error(E_RECOVERABLE_ERROR, "'host' must be String");
		return server;
	    }
	} else if (zend_hash_find(z_conf_row, "newhost", sizeof("newhost"), (void **) &entry) == SUCCESS) {
	    if ((Z_TYPE_PP(entry) == IS_STRING)) {
	        server.serverName = Z_STRVAL_PP(entry);
	        server.stable = false;
	    } else {
		zend_error(E_RECOVERABLE_ERROR, "'newhost' must be String");
		return server;
	    }
	} else {
	    server.port = -1;
	    return server;
	}

	if (zend_hash_find(z_conf_row, "port", sizeof("port"), (void **) &entry) == SUCCESS) {
	    if (Z_TYPE_PP(entry) == IS_LONG) {
	        server.port = Z_LVAL_PP(entry);
	    } else {
		zend_error(E_RECOVERABLE_ERROR, "'port' must be Int");
		return server;
	    }
	} else {
	    server.port = 11211;
	}
    } else {
	zend_error(E_RECOVERABLE_ERROR, "configuration Row is not Array");
	return server;
    }
    return server;
}

PHP_METHOD(cm, __construct)
{
    zval *object = getThis();
    std::vector<ServerPair> configuration;
    std::vector<ServerPair> newconfiguration;
    zval* zval_conf = NULL;
    bool useResharding = false;
    bool debug = false;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a|b", &zval_conf, &debug) == FAILURE) {
        RETURN_NULL();
    }

    if ((zval_conf == NULL) || (Z_TYPE_P(zval_conf) != IS_ARRAY)) {
        zend_error(E_RECOVERABLE_ERROR, "configuration is not Array");
        RETURN_NULL();
    }

    HashTable *z_conf = NULL;
    z_conf = Z_ARRVAL_P(zval_conf);

    HashPosition pointer;
    zval **data;
    for(zend_hash_internal_pointer_reset_ex(z_conf, &pointer);
        zend_hash_get_current_data_ex(z_conf, (void**) &data, &pointer) == SUCCESS;
        zend_hash_move_forward_ex(z_conf, &pointer)) {

	ServerPair server = makeSPfromPhpArray(data);
	if (server.port > 0) {
	    if (server.stable) {
		configuration.push_back(server);
		newconfiguration.push_back(server);
	    } else {
		newconfiguration.push_back(server);
		useResharding = true;
	    }
	} else if (server.port == -1) { //not 'host' or 'newhost' entry, try replica array
	    if (Z_TYPE_PP(data) != IS_ARRAY) {
		zend_error(E_RECOVERABLE_ERROR, "configuration ROW is not Array");
		RETURN_NULL();
	    }
	    bool isNewShard=false;
	    bool isNewShardSet=false;
	    HashTable *z_conf_row;
	    z_conf_row = Z_ARRVAL_PP(data);
	    std::vector<ServerPair> replicas;
	    HashPosition rpointer;
	    zval **replicadata;
	    for(zend_hash_internal_pointer_reset_ex(z_conf_row, &rpointer);
	        zend_hash_get_current_data_ex(z_conf_row, (void**) &replicadata, &rpointer) == SUCCESS;
	        zend_hash_move_forward_ex(z_conf_row, &rpointer)) {
		if (Z_TYPE_PP(replicadata) != IS_ARRAY) {
		    zend_error(E_RECOVERABLE_ERROR, "configuration ROW is not Array and is NOT Array of Replica configuration ROW");
		    RETURN_NULL();
		}
		ServerPair server = makeSPfromPhpArray(replicadata);
		if (server.port > 0) {
		    if (server.stable && isNewShard) {
			zend_error(E_RECOVERABLE_ERROR, "bad NEW shard with ReShard in configuration (all replicas must be 'newhost')");
		    }
		    if (!server.stable && !isNewShard) {
			zend_error(E_RECOVERABLE_ERROR, "bad NEW shard with ReShard in configuration (all replicas must be 'newhost')");
		    }
		    replicas.push_back(server);
		    if (!isNewShardSet) {
			isNewShardSet = true;
			if (!server.stable) {
			    isNewShard = true;
			} else {
			    isNewShard = false;
			}
		    }
		} else if (server.port == -1) {
		    zend_error(E_RECOVERABLE_ERROR, "configuration ROW is not has not 'host' or 'newhost' entry");
		    RETURN_NULL();
		} else if (server.port == 0) {
		    RETURN_NULL();
		}
	    }
	    if (replicas.size() > 0) {
		ServerPair replica;
		replica.port = -1;
		replica.stable = false;
		replica.isReplica = true;
		replica.replica = replicas;
		if (!isNewShard) {
		    configuration.push_back(replica);
		    newconfiguration.push_back(replica);
		} else {
		    useResharding = true;
		    newconfiguration.push_back(replica);
		}
	    } else {
		zend_error(E_RECOVERABLE_ERROR, "configuration ROW is empty Array");
		RETURN_NULL();
	    }
	} else if (server.port == 0) {
	    RETURN_NULL();
	}
    }
    if (configuration.size() == 0) {
	zend_error(E_RECOVERABLE_ERROR, "configuration is Empty");
	RETURN_NULL();
    } else if (configuration.size() > 254) {
	zend_error(E_RECOVERABLE_ERROR, "configuration too big");
	RETURN_NULL();
    }
    if (!useResharding) {
        Cm *cmReal = NULL;
        cmReal = new Cm(configuration, debug);
        cm_object *obj = (cm_object *)zend_object_store_get_object(object TSRMLS_CC);
        obj->cm = cmReal;
    } else {
        Cm *cmOld = new Cm(configuration, debug);
        Cm *cmNew = new Cm(newconfiguration, debug);
        CmWrapper *cmW = new CmWrapper(cmOld, cmNew, debug);
        cm_object *obj = (cm_object *)zend_object_store_get_object(object TSRMLS_CC);
        obj->cm = cmW;
    }
}

//bool flush();
PHP_METHOD(cm, flush)
{
    CmInterface *cm;
    cm_object *obj = (cm_object *)zend_object_store_get_object(
        getThis() TSRMLS_CC);
    cm = obj->cm;
    if (cm != NULL) {
        zval* zval_arr = NULL;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|a", &zval_arr) == FAILURE) {
            RETURN_NULL();
        }

        if ((zval_arr == NULL) || (Z_TYPE_P(zval_arr) != IS_ARRAY)) {
            RETURN_BOOL(cm->flush());
        }

        HashTable *z_arr = NULL;
        z_arr = Z_ARRVAL_P(zval_arr);

        HashPosition pointer;
        zval **data;
        bool success=true;
        for(zend_hash_internal_pointer_reset_ex(z_arr, &pointer);
            zend_hash_get_current_data_ex(z_arr, (void**) &data, &pointer) == SUCCESS;
            zend_hash_move_forward_ex(z_arr, &pointer)) {
            if (Z_TYPE_PP(data) != IS_LONG) {
                zend_error(E_RECOVERABLE_ERROR, "array entry is not Long (shard number)");
            }
            long shardId = Z_LVAL_PP(data);
            success = success && cm->flush(shardId);
        }
        RETURN_BOOL(success);

    }
}

//bool remove(char *key);
PHP_METHOD(cm, remove)
{
    CmInterface *cm;
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
            //check quiet mode
            zend_error(E_WARNING, "Non-string key!");
            RETURN_NULL();
        }

        RETURN_BOOL(cm->remove(key));
    }
}

PHP_METHOD(cm, delete)
{
    CmInterface *cm;
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
            //check quiet mode
            zend_error(E_WARNING, "Non-string key!");
            RETURN_NULL();
        }

        RETURN_BOOL(cm->remove(key));
    }
}


//char* get(char *key, size_t *return_value_length)
PHP_METHOD(cm, get)
{
    CmInterface *cm;
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
            //check quiet mode
            zend_error(E_WARNING, "Non-string key!");
            RETURN_NULL();
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

//std::map<std::string, RVal> mget(std::vector<std::string> keys)
PHP_METHOD(cm, mget)
{
    CmInterface *cm;
    cm_object *obj = (cm_object *)zend_object_store_get_object(
        getThis() TSRMLS_CC);
    cm = obj->cm;
    if (cm != NULL) {
        std::vector<std::string> keys;
        zval* zval_keys = NULL;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &zval_keys) == FAILURE) {
            RETURN_NULL();
        }

        if ((zval_keys == NULL) || (Z_TYPE_P(zval_keys) != IS_ARRAY)) {
            zend_error(E_RECOVERABLE_ERROR, "keys is not Array");
            RETURN_NULL();
        }

        HashTable *zval_hkeys;
        zval_hkeys = Z_ARRVAL_P(zval_keys);

        HashPosition pointer;
        zval **data;
        for(zend_hash_internal_pointer_reset_ex(zval_hkeys, &pointer);
            zend_hash_get_current_data_ex(zval_hkeys, (void**) &data, &pointer) == SUCCESS;
            zend_hash_move_forward_ex(zval_hkeys, &pointer)) {
            keys.push_back(Z_STRVAL_PP(data));
        }
        std::map<std::string, RVal> ret = cm->mget(keys);
        array_init(return_value);
        for (std::map<std::string, RVal>::iterator it = ret.begin(); it != ret.end(); ++it) {
            add_assoc_stringl(return_value, it->first.c_str(), it->second.value, it->second.length, 1);
        }
    } else {
        RETURN_NULL();
    }
}


//bool set(char *key, char *value, bool isDependency, char *dependency, long expire)
PHP_METHOD(cm, set)
{
    CmInterface *cm;
    cm_object *obj = (cm_object *)zend_object_store_get_object(
        getThis() TSRMLS_CC);
    cm = obj->cm;
    if (cm != NULL) {
        char *key;
        int key_len;
        char *value;
        int value_len;
        char *dependency = NULL;
        int dependency_len=0;
        long expire=0;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|sl", &key, &key_len, &value, &value_len, &dependency, &dependency_len, &expire) == FAILURE) {
            RETURN_NULL();
        }
        if (strnlen(key, key_len+1) != key_len) {
            //check quiet mode
            zend_error(E_WARNING, "Non-string key!");
            RETURN_NULL();
        }
        if (dependency_len == 0) {
            dependency = new char [1];
            strcpy(dependency, "");
        }
        if ((dependency != NULL)&&(strnlen(dependency, dependency_len+1) != dependency_len)) {
            //check quiet mode
            zend_error(E_WARNING, "Non-string dependency key!");
            RETURN_NULL();
        }

        RETURN_BOOL(cm->set(key, value, value_len, dependency, expire));
    }
    RETURN_NULL();
}

//bool add(char *key, char *value, bool isDependency, char *dependency, long expire)
PHP_METHOD(cm, add)
{
    CmInterface *cm;
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
            //check quiet mode
            zend_error(E_WARNING, "Non-string key!");
            RETURN_NULL();
        }
        if (dependency_len == 0) {
            dependency = new char [1];
            strcpy(dependency, "");
        }
        if (strnlen(dependency, dependency_len+1) != dependency_len) {
            //check quiet mode
            zend_error(E_WARNING, "Non-string dependency key!");
            RETURN_NULL();
        }

        RETURN_BOOL(cm->add(key, value, value_len, dependency, expire));
    }
    RETURN_NULL();
}

zend_function_entry cm_methods[] = {
    PHP_ME(cm,  __construct,     NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(cm,  set,             NULL, ZEND_ACC_PUBLIC)
    PHP_ME(cm,  add,             NULL, ZEND_ACC_PUBLIC)
    PHP_ME(cm,  get,             NULL, ZEND_ACC_PUBLIC)
    PHP_ME(cm,  mget,            NULL, ZEND_ACC_PUBLIC)
    PHP_ME(cm,  remove,          NULL, ZEND_ACC_PUBLIC)
    PHP_ME(cm,  delete,          NULL, ZEND_ACC_PUBLIC)
    PHP_ME(cm,  flush,          NULL, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

PHP_MINIT_FUNCTION(cm)
{
    startLogging();
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "Cm", cm_methods);
    cm_ce = zend_register_internal_class(&ce TSRMLS_CC);
    cm_ce->create_object = cm_create_handler;
    memcpy(&cm_object_handlers,
        zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    cm_object_handlers.clone_obj = NULL;
    return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(cm)
{
    stopLogging();
    return SUCCESS;
}


zend_module_entry cm_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    PHP_CM_EXTNAME,
    NULL,        /* Functions */
    PHP_MINIT(cm),        /* MINIT */
    PHP_MSHUTDOWN(cm),        /* MSHUTDOWN */
    NULL,        /* RINIT */
    NULL,        /* RSHUTDOWN */
    NULL,        /* MINFO */
#if ZEND_MODULE_API_NO >= 20010901
    PHP_CM_EXTVER,
#endif
    STANDARD_MODULE_PROPERTIES
};

ZEND_GET_MODULE(cm)
