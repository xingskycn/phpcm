<?php

echo "__construct ================\n";
$cm = new Cm();
var_dump($cm);
echo "SET ========================\n";
var_dump($cm->set("key", "value"));
var_dump($cm->set("key", "value", true));
echo "SET with dependency ========\n";
var_dump($cm->set("key", "value", true, "one"));
var_dump($cm->set("key", "value", true, "one", 3600*24));
echo "SET invalid keys == ========\n";
var_dump($cm->set("key\000r", "value"));
var_dump($cm->set("keyr", "value", true, "dkey\000rd"));
echo "GET ========================\n";
var_dump($cm->get("key"));
var_dump($cm->get("keyd"));
echo "DELETE =====================\n";
var_dump($cm->remove("key"));
var_dump($cm->remove("key1"));
var_dump($cm->get("key"));
