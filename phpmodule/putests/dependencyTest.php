<?php

class DependencyTest extends PHPUnit_Framework_TestCase
{
    private $cm;
    
    public function setup()
    {
	$this->cm = new Cm([ ['host' => '127.0.0.1', 'port'=>11211] ]);
    }

    public function testMgetDependency()
    {
	$this->cm->remove("dep");
	$this->cm->remove("first");
	$this->cm->remove("second");
	$this->assertTrue($this->cm->set("dep", "dval"), "set dep");
	$this->assertTrue($this->cm->set("first", "fval", "dep"), "set first");
	$this->assertTrue($this->cm->set("second", "sval"), "set second");
	$this->assertTrue($this->cm->set("dep", "dval2"));
	$list = $this->cm->mget(array("first", "second"));
	$this->assertTrue(empty($list["first"]), "first expired");
	$this->assertFalse(empty($list["second"]), "second exists");
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



}
