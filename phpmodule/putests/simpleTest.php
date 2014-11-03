<?php

class SimpleTest extends PHPUnit_Framework_TestCase
{
    private $cm;
    
    public function setup()
    {
	$this->cm = new Cm([ ['host' => '127.0.0.1', 'port'=>11211] ]);
    }

    public function testConstruct()
    {
	$cm = new Cm([ ['host'=>'127.0.0.1', 'port'=>11211] ]);
	$this->assertTrue($cm instanceof Cm);
    }

    public function testSet()
    {
	$this->assertTrue($this->cm->set("key", "value"));
	$this->assertTrue($this->cm->set("key", "value"));
    }
    
    public function testSetWithDependency()
    {
	$this->assertTrue($this->cm->set("key", "value", "one"));
	$this->assertTrue($this->cm->set("key", "value", "one", 3600*24));
    }

    public function testSetInvalidKeys()
    {
	try {
	    $this->assertNull($this->cm->set("key\000r", "value"));
	} catch (Exception $e) {
	    $this->assertEquals($e->getMessage(), "Non-string key!");
	}
	try {
	    $this->assertNull($this->cm->set("keyr", "value", "dkey\000rd"));
	} catch (Exception $e) {
	    $this->assertEquals($e->getMessage(), "Non-string dependency key!");
	}
    }

    public function testGet()
    {
	$this->assertTrue($this->cm->set("key", "value"));
	$this->assertEquals($this->cm->get("key"), "value");
	$this->assertNull($this->cm->get("keyd"));
    }

    public function testDelete()
    {
	$this->assertTrue($this->cm->set("key", "value"), "set success");
	$this->assertTrue($this->cm->remove("key"), "remove success");
	$this->assertFalse($this->cm->remove("key1"), "remove not required");
	$this->assertNull($this->cm->get("key"));
    }

    public function testAdd()
    {
	$this->cm->remove("key");
	$this->assertTrue($this->cm->add("key", "valueAdd"), "added");
	$this->assertEquals($this->cm->get("key"), "valueAdd", "first equals");
	$this->assertFalse($this->cm->add("key", "value"), "not added");
	$this->assertEquals($this->cm->get("key"), "valueAdd", "second equals");
    }

}
