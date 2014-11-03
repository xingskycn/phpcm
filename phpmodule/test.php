<?php

$mc = new Memcache();
$mc->addServer('127.0.0.1', 11211);
$mc->set("a", "aVal");
$mc->set("b", "bVal");

var_dump($mc->get(array("a", "b")));
