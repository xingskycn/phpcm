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
	$scmA = new Cm([$shardA]);
	$scmB = new Cm([$shardB]);

	//cleanup
	$scm->remove("a");
	$scm->remove("b");
	$scmA->remove("a");
	$scmA->remove("b");
	$scmB->remove("a");
	$scmB->remove("b");


	$this->assertTrue($scm->set("a", "valueA"), "st-1");
	$this->assertTrue($scm->set("b", "valueB"), "st-2");
	$this->assertEquals($scm->get("a"), "valueA", "st-3");
	$this->assertEquals($scm->get("b"), "valueB", "st-4");

	$this->assertEquals($scmA->get("b"), "valueB", "st-5");
	$this->assertNull($scmA->get("a"), "st-6");

	$this->assertEquals($scmB->get("a"), "valueA", "st-7");
	$this->assertNull($scmB->get("b"), "st-8");
    }

    public function testShardingByLatestByte()
    {
	$shardA = [ 'host' => '127.0.0.1', 'port'=>11211 ];
	$shardB = [ 'host' => '127.0.0.1', 'port'=>11212 ];
	$scm = new Cm([
	    $shardA,
	    $shardB,
	]);
	$scmA = new Cm([$shardA]);
	$scmB = new Cm([$shardB]);

	//cleanup
	$scm->remove("aa");
	$scm->remove("ab");
	$scmA->remove("aa");
	$scmA->remove("ab");
	$scmB->remove("aa");
	$scmB->remove("ab");


	$this->assertTrue($scm->set("aa", "valueA"), "st-1");
	$this->assertTrue($scm->set("ab", "valueB"), "st-2");
	$this->assertEquals($scm->get("aa"), "valueA", "st-3");
	$this->assertEquals($scm->get("ab"), "valueB", "st-4");

	$this->assertEquals($scmA->get("ab"), "valueB", "st-5");
	$this->assertNull($scmA->get("aa"), "st-6");

	$this->assertEquals($scmB->get("aa"), "valueA", "st-7");
	$this->assertNull($scmB->get("ab"), "st-8");
    }

    public function testShardingByLatestByteAdd()
    {
	$shardA = [ 'host' => '127.0.0.1', 'port'=>11211 ];
	$shardB = [ 'host' => '127.0.0.1', 'port'=>11212 ];
	$scm = new Cm([
	    $shardA,
	    $shardB,
	]);
	$scmA = new Cm([$shardA]);
	$scmB = new Cm([$shardB]);

	//cleanup
	$scm->remove("aa");
	$scm->remove("ab");
	$scmA->remove("aa");
	$scmA->remove("ab");
	$scmB->remove("aa");
	$scmB->remove("ab");


	$this->assertTrue($scm->add("aa", "valueA"), "st-1");
	$this->assertTrue($scm->add("ab", "valueB"), "st-2");
	$this->assertEquals($scm->get("aa"), "valueA", "st-3");
	$this->assertEquals($scm->get("ab"), "valueB", "st-4");

	$this->assertEquals($scmA->get("ab"), "valueB", "st-5");
	$this->assertNull($scmA->get("aa"), "st-6");

	$this->assertEquals($scmB->get("aa"), "valueA", "st-7");
	$this->assertNull($scmB->get("ab"), "st-8");
    }


    public function testReplication()
    {
	$replica0 = [ 'host' => '127.0.0.1', 'port'=>11211 ];
	$replica1 = [ 'host' => '127.0.0.1', 'port'=>11212 ];
	$shardA = [ $replica0, $replica1 ];
	$scm = new Cm([
	    $shardA
	]);
	$this->assertTrue($scm->set("replicated", "valueR"), "set success");
	$rcm0 = new Cm([ $replica0 ]);
	$rcm1 = new Cm([ $replica1 ]);
	$this->assertEquals($rcm0->get("replicated"), "valueR", "eq-1");
	$this->assertEquals($rcm1->get("replicated"), "valueR", "eq-2");
    }

    public function testFlushSingleShard()
    {
        $shardA = [ 'host' => '127.0.0.1', 'port'=>11211 ];
        $shardB = [ 'host' => '127.0.0.1', 'port'=>11212 ];
        $scm = new Cm([
            $shardA,
            $shardB
        ]);
        $this->assertTrue($scm->set("a", "valA"), "set a");
        $this->assertTrue($scm->set("b", "valB"), "set b");
        $this->assertTrue($scm->flush([ 0 ]), "flush");
        $this->assertNull($scm->get("b"), "check b");
        $this->assertEquals($scm->get("a"), "valA", "check a");
    }

    public function testFlushSingleShardSecond()
    {
        $shardA = [ 'host' => '127.0.0.1', 'port'=>11211 ];
        $shardB = [ 'host' => '127.0.0.1', 'port'=>11212 ];
        $scm = new Cm([
            $shardA,
            $shardB
        ]);
        $this->assertTrue($scm->set("a", "valA"), "set a");
        $this->assertTrue($scm->set("b", "valB"), "set b");
        $this->assertTrue($scm->flush([ 1 ]), "flush");
        $this->assertNull($scm->get("a"), "check a");
        $this->assertEquals($scm->get("b"), "valB", "check b");
    }

    public function testReplicationAndSharding()
    {
	$replica0 = [ 'host' => '127.0.0.1', 'port'=>11211 ];
	$replica1 = [ 'host' => '127.0.0.1', 'port'=>11212 ];
	$shardA = [ $replica0, $replica1 ];
	$shardB = [ 'host' => '127.0.0.1', 'port'=>11221 ];
	$scm = new Cm([
	    $shardA,
	    $shardB
	]);
	$scm->remove("a");
	$scm->remove("b");

	$this->assertTrue($scm->set("a", "valueA"), "set success a");
	$this->assertTrue($scm->set("b", "valueB"), "set success b");
	$scmA = new Cm([ $shardA ]);
	$this->assertEquals($scmA->get("b"), "valueB", "eq-1");
	$scmB = new Cm([ $shardB ]);
	$this->assertEquals($scmB->get("a"), "valueA", "eq-2");
    }


}
