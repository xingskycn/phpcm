#ifndef PHP_CM_H
#define PHP_CM_H

#define PHP_CM_EXTNAME  "CM"
#define PHP_CM_EXTVER   "0.2"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif 

extern "C" {
#include "php.h"
}

extern zend_module_entry CM_module_entry;
#define phpext_CM_ptr &CM_module_entry;

#endif /* PHP_CM_H */
