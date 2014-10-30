<?php

$cm = new Cm();
var_dump($cm);
var_dump($cm->set("key", "value"));
var_dump($cm->set("key", "value", true));
var_dump($cm->set("key", "value", true, "one"));
var_dump($cm->set("key", "value", true, "one", 3600*24));
var_dump($cm->set("key\000r", "value"));
var_dump($cm->set("keyr", "value", true, "dkey\000rd"));
