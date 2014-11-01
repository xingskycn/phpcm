<?php

class CmTest extends PHPUnit_Framework_TestCase
{
    public function testConstruct()
    {
	$cm = new Cm();
	$this->assertTrue($cm instanceof Cm);
    }

    public function testSet()
    {
	$cm = new Cm();
	$this->assertTrue($cm->set("key", "value"));
	$this->assertTrue($cm->set("key", "value", true));
    }
    
    public function testSetWithDependency()
    {
	$cm = new Cm();
	$this->assertTrue($cm->set("key", "value", true, "one"));
	$this->assertTrue($cm->set("key", "value", true, "one", 3600*24));
    }

    public function testSetInvalidKeys()
    {
	$cm = new Cm();
	$this->assertEquals($cm->set("key\000r", "value"), "Non-string key!");
	$this->assertEquals($cm->set("keyr", "value", true, "dkey\000rd"), "Non-string dependency key!");
    }

    public function testGet()
    {
	$cm = new Cm();
	$this->assertTrue($cm->set("key", "value", true));
	$this->assertEquals($cm->get("key", true), "value");
	$this->assertNull($cm->get("keyd", true));
    }

    public function testDelete()
    {
	$cm = new Cm();
	$this->assertTrue($cm->set("key", "value", true), "set success");
	$this->assertTrue($cm->remove("key"), "remove success");
	$this->assertFalse($cm->remove("key1"), "remove not required");
	$this->assertNull($cm->get("key", true));
    }

    public function testCheckDependency()
    {
	$cm = new Cm();
	$this->assertTrue($cm->set("one", "dataValid", true));
	$this->assertTrue($cm->set("two", "byDependency", true, "one"));
	$this->assertEquals($cm->get("two", true), "byDependency");
	$this->assertTrue($cm->set("one", "dataInvalid", true));
	$this->assertNull($cm->get("two", true));
    }

}