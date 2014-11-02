<?php

class BadConfigTest extends PHPUnit_Framework_TestCase
{

    public function testEmpty()
    {
	try {
	    $cm = new Cm();
	    $this->assertTrue(false);
	} catch (Exception $e) {
	    $this->assertEquals($e->getMessage(), "Cm::__construct() expects exactly 1 parameter, 0 given");
	}
    }

    public function testEmptyArray()
    {
	try {
	    $cm = new Cm([]);
	    $this->assertTrue(false);
	} catch (Exception $e) {
	    $this->assertEquals($e->getMessage(), "configuration is Empty");
	}
    }

    public function testEmptyShard()
    {
	try {
	    $cm = new Cm([[]]);
	    $this->assertTrue(false);
	} catch (Exception $e) {
	    $this->assertEquals($e->getMessage(), "configuration ROW is empty Array");
	}
    }

    public function testBadConfigType()
    {
	try {
	    $cm = new Cm("String here");
	    $this->assertTrue(false);
	} catch (Exception $e) {
	    $this->assertEquals($e->getMessage(), "Cm::__construct() expects parameter 1 to be array, string given");
	}
    }

    public function testBadHostType()
    {
	try {
	    $cm = new Cm([ [ 'host'=>1 ] ]);
	    $this->assertTrue(false);
	} catch (Exception $e) {
	    $this->assertEquals($e->getMessage(), "'host' must be String");
	}
    }

    public function testBadNewhostType()
    {
	try {
	    $cm = new Cm([ [ 'newhost'=>1 ] ]);
	    $this->assertTrue(false);
	} catch (Exception $e) {
	    $this->assertEquals($e->getMessage(), "'newhost' must be String");
	}
    }

    public function testBadPortType()
    {
	try {
	    $cm = new Cm([ [ 'host'=>'localhost', 'port'=>'string' ] ]);
	    $this->assertTrue(false);
	} catch (Exception $e) {
	    $this->assertEquals($e->getMessage(), "'port' must be Int");
	}
    }

    public function testEmptyReplica()
    {
	try {
	    $cm = new Cm([[[]]]);
	    $this->assertTrue(false);
	} catch (Exception $e) {
	    $this->assertEquals($e->getMessage(), "configuration ROW is not has not 'host' or 'newhost' entry");
	}
    }

    public function testBadHostOrPortIntoReplica()
    {
	try {
	    $cm = new Cm([[[ 'host'=>1 ]]]);
	    $this->assertTrue(false);
	} catch (Exception $e) {
	    $this->assertEquals($e->getMessage(), "'host' must be String");
	}

	try {
	    $cm = new Cm([[[ 'host'=>'localhost', 'port'=>'s' ]]]);
	    $this->assertTrue(false);
	} catch (Exception $e) {
	    $this->assertEquals($e->getMessage(), "'port' must be Int");
	}
    }

    public function testShardIsString()
    {
	try {
	    $cm = new Cm([ "shard" ]);
	    $this->assertTrue(false);
	} catch (Exception $e) {
	    $this->assertEquals($e->getMessage(), "configuration Row is not Array");
	}
    }


    public function testReplicaIsString()
    {
	try {
	    $cm = new Cm([ [['host'=>'localhost'],"shard"] ]);
	    $this->assertTrue(false);
	} catch (Exception $e) {
	    $this->assertEquals($e->getMessage(), "configuration ROW is not Array and is NOT Array of Replica configuration ROW");
	}
    }

    public function testTooBigConfig()
    {
	$config = [];
	for ($i=0; $i<260; $i++) {
	    $config []= [ 'host'=>'127.0.0.1', 'port'=>11211 ];
	}
	try {
	    $cm = new Cm($config);
	    $this->assertTrue(false);
	} catch (Exception $e) {
	    $this->assertEquals($e->getMessage(), "configuration too big");
	}
    }

}
