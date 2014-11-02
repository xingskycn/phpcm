<?php

class BadConfigTest extends PHPUnit_Framework_TestCase
{

    public function testEmpty()
    {
	try {
	    $cm = new Cm();
	} catch (Exception $e) {
	    $this->assertEquals($e->getMessage(), "Cm::__construct() expects exactly 1 parameter, 0 given");
	}
    }

    public function testEmptyArray()
    {
	try {
	    $cm = new Cm([]);
	} catch (Exception $e) {
	    $this->assertEquals($e->getMessage(), "configuration is Empty");
	}
    }

    public function testEmptyShard()
    {
	try {
	    $cm = new Cm([[]]);
	} catch (Exception $e) {
	    $this->assertEquals($e->getMessage(), "configuration ROW is empty Array");
	}
    }

    public function testBadConfigType()
    {
	try {
	    $cm = new Cm("String here");
	} catch (Exception $e) {
	    $this->assertEquals($e->getMessage(), "Cm::__construct() expects parameter 1 to be array, string given");
	}
    }

    public function testBadHostType()
    {
	try {
	    $cm = new Cm([ [ 'host'=>1 ] ]);
	} catch (Exception $e) {
	    $this->assertEquals($e->getMessage(), "'host' must be String");
	}
    }

    public function testBadNewhostType()
    {
	try {
	    $cm = new Cm([ [ 'newhost'=>1 ] ]);
	} catch (Exception $e) {
	    $this->assertEquals($e->getMessage(), "'newhost' must be String");
	}
    }

    public function testBadPortType()
    {
	try {
	    $cm = new Cm([ [ 'host'=>'localhost', 'port'=>'string' ] ]);
	} catch (Exception $e) {
	    $this->assertEquals($e->getMessage(), "'port' must be Int");
	}
    }

    public function testEmptyReplica()
    {
	try {
	    $cm = new Cm([[[]]]);
	} catch (Exception $e) {
	    $this->assertEquals($e->getMessage(), "configuration ROW is not has not 'host' or 'newhost' entry");
	}
    }

    public function testBadHostOrPortIntoReplica()
    {
	try {
	    $cm = new Cm([[[ 'host'=>1 ]]]);
	} catch (Exception $e) {
	    $this->assertEquals($e->getMessage(), "'host' must be String");
	}

	try {
	    $cm = new Cm([[[ 'host'=>'localhost', 'port'=>'s' ]]]);
	} catch (Exception $e) {
	    $this->assertEquals($e->getMessage(), "'port' must be Int");
	}
    }

    public function testShardIsString()
    {
	try {
	    $cm = new Cm([ "shard" ]);
	} catch (Exception $e) {
	    $this->assertEquals($e->getMessage(), "configuration Row is not Array");
	}
    }


    public function testReplicaIsString()
    {
	try {
	    $cm = new Cm([ [['host'=>'localhost'],"shard"] ]);
	} catch (Exception $e) {
	    $this->assertEquals($e->getMessage(), "configuration ROW is not Array and is NOT Array of Replica configuration ROW");
	}
    }

}
