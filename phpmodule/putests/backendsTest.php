<?php

class BackendsTest extends PHPUnit_Framework_TestCase
{

    public function testSharding()
    {
	$shardA = [ 'host' => '127.0.0.1', 'port'=>11211 ];
	$shardB = [ 'host' => '127.0.0.1', 'port'=>11212 ];
	$scm = new Cm([
	    $shardA,
	    $shardB,
	]);
	$this->assertTrue($scm->remove("a"), "st-r-1");
	$this->assertTrue($scm->remove("b"), "st-r-2");

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

    public function testReplication()
    {
	$replica0 = [ 'host' => '127.0.0.1', 'port'=>11211 ];
	$replica1 = [ 'host' => '127.0.0.1', 'port'=>11212 ];
	$shardA = [ $replica0, $replica1 ];
	$scm = new Cm([
	    $shardA
	]);
	$this->assertTrue($csm->set("replicated", "valueR"), true);
	$rcm0 = new Cm([ $replica0 ]);
	$rcm1 = new Cm([ $replica1 ]);
	$this->assertEquals($rcm0->get("replicated"), "valueR");
	$this->assertEquals($rcm1->get("replicated"), "valueR");
    }

}
