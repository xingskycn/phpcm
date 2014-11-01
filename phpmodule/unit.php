<?php

class CmTest extends PHPUnit_Framework_TestCase
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
	$this->assertEquals($this->cm->set("key\000r", "value"), "Non-string key!");
	$this->assertEquals($this->cm->set("keyr", "value", "dkey\000rd"), "Non-string dependency key!");
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

    public function testCheckDependency()
    {
	$this->assertTrue($this->cm->set("one", "dataValid"));
	$this->assertTrue($this->cm->set("two", "byDependency", "one"));
	$this->assertEquals($this->cm->get("two"), "byDependency");
	$this->assertTrue($this->cm->set("one", "dataInvalid"));
	$this->assertNull($this->cm->get("two"));
    }

    public function testCheckDependencyReturn()
    {
	$this->assertTrue($this->cm->set("one", "dataValid"));
	$this->assertTrue($this->cm->set("two", "byDependency", "one"));
	$this->assertEquals($this->cm->get("two"), "byDependency");
	$this->assertTrue($this->cm->set("one", "dataInvalid"));
	$this->assertNull($this->cm->get("two"));
	$this->assertTrue($this->cm->set("one", "dataValid"));
	$this->assertEquals($this->cm->get("two"), "byDependency");
    }

    public function testCheckDependencyByDependency()
    {
	$this->assertTrue($this->cm->set("root", "dataValid1"));
	$this->assertTrue($this->cm->set("root/1", "dataValid2", "root"));
	$this->assertTrue($this->cm->set("root/1/1", "byDependency", "root/1"));
	$this->assertEquals($this->cm->get("root/1/1"), "byDependency");
	$this->assertTrue($this->cm->set("root", "dataInValid1"));
	$this->assertNull($this->cm->get("root/1/1"));
    }

    public function testSharding()
    {
	$shardA = [ 'host' => '127.0.0.1', 'port'=>11211 ];
	$shardB = [ 'host' => '127.0.0.1', 'port'=>11212 ];
	$scm = new Cm([
	    $shardA,
	    $shardB,
	]);
	$this->assertTrue($scm->set("a", "valueA"), "st-1");
	$this->assertTrue($scm->set("b", "valueB"), "st-2");
	$this->assertEquals($scm->get("a"), "valueA", "st-3");
	$this->assertEquals($scm->get("b"), "valueB", "st-4");
	$scmA = new Cm([$shardA]);
	$this->assertEquals($scmA->get("b"), "valueB", "st-5");
	$this->assertNull($scmA->get("a"), "st-6");
	$scmB = new Cm([$shardB]);
	$this->assertEquals($scmB->get("a"), "valueA", "st-7");
	$this->assertNull($scmB->get("b"), "st-8");
    }

}
