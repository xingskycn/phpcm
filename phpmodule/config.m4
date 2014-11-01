PHP_ARG_ENABLE(cm,
    [Enable the "cm" extension],
    [  --enable-cm      Enable "cm" extension support])

if test $PHP_CM != "no"; then
    PHP_REQUIRE_CXX()
    PHP_SUBST(CM_SHARED_LIBADD)
    PHP_ADD_LIBRARY(stdc++, 1, CM_SHARED_LIBADD)
    PHP_NEW_EXTENSION(cm, cm.cc cm_class.cc cm_adapter.cc crc32.cc, $ext_shared)
fi
