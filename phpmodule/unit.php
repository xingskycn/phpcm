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
	$this->assertTrue($cm->set("key", "value"));
    }
    
    public function testSetWithDependency()
    {
	$cm = new Cm();
	$this->assertTrue($cm->set("key", "value", "one"));
	$this->assertTrue($cm->set("key", "value", "one", 3600*24));
    }

    public function testSetInvalidKeys()
    {
	$cm = new Cm();
	$this->assertEquals($cm->set("key\000r", "value"), "Non-string key!");
	$this->assertEquals($cm->set("keyr", "value", "dkey\000rd"), "Non-string dependency key!");
    }

    public function testGet()
    {
	$cm = new Cm();
	$this->assertTrue($cm->set("key", "value"));
	$this->assertEquals($cm->get("key"), "value");
	$this->assertNull($cm->get("keyd"));
    }

    public function testDelete()
    {
	$cm = new Cm();
	$this->assertTrue($cm->set("key", "value"), "set success");
	$this->assertTrue($cm->remove("key"), "remove success");
	$this->assertFalse($cm->remove("key1"), "remove not required");
	$this->assertNull($cm->get("key"));
    }

    public function testCheckDependency()
    {
	$cm = new Cm();
	$this->assertTrue($cm->set("one", "dataValid"));
	$this->assertTrue($cm->set("two", "byDependency", "one"));
	$this->assertEquals($cm->get("two"), "byDependency");
	$this->assertTrue($cm->set("one", "dataInvalid"));
	$this->assertNull($cm->get("two"));
    }

    public function testCheckDependencyReturn()
    {
	$cm = new Cm();
	$this->assertTrue($cm->set("one", "dataValid"));
	$this->assertTrue($cm->set("two", "byDependency", "one"));
	$this->assertEquals($cm->get("two"), "byDependency");
	$this->assertTrue($cm->set("one", "dataInvalid"));
	$this->assertNull($cm->get("two"));
	$this->assertTrue($cm->set("one", "dataValid"));
	$this->assertEquals($cm->get("two"), "byDependency");
    }

    public function testCheckDependencyByDependency()
    {
	$cm = new Cm();
	$this->assertTrue($cm->set("root", "dataValid1"));
	$this->assertTrue($cm->set("root/1", "dataValid2", "root"));
	$this->assertTrue($cm->set("root/1/1", "byDependency", "root/1"));
	$this->assertEquals($cm->get("root/1/1"), "byDependency");
	$this->assertTrue($cm->set("root", "dataInValid1"));
	$this->assertNull($cm->get("root/1/1"));
    }

}
