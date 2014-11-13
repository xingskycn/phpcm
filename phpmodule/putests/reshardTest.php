<?php

class ReshardTest extends PHPUnit_Framework_TestCase
{

    public function testConstruct()
    {
	$cm = new Cm([ 
	    ['host'=>'127.0.0.1', 'port'=>11211],
	    ['host'=>'127.0.0.1', 'port'=>11212],
	    ['newhost'=>'127.0.0.1', 'port'=>11221],
	]);
	$this->assertTrue($cm instanceof Cm);
	$cmd = new Cm([ 
	    ['host'=>'127.0.0.1', 'port'=>11211],
	    ['host'=>'127.0.0.1', 'port'=>11212],
	    ['newhost'=>'127.0.0.1', 'port'=>11221],
	], true);
	$this->assertTrue($cmd instanceof Cm);
    }

    public function testAdd()
    {
	$this->clear();
	//old (no newshard) configuration
	$cm = new Cm([
	    ['host'=>'127.0.0.1', 'port'=>11211],
	    ['host'=>'127.0.0.1', 'port'=>11212],
//	    ['newhost'=>'127.0.0.1', 'port'=>11221],
	], true);
	$this->assertTrue($cm->add("b", "valueB"));
	$this->assertTrue($cm->add("c", "valueC"));
	$this->assertTrue($cm->add("d", "valueD"));

	//check first shard
	$this->assertEquals($this->getFromPort(11211, "b"), "valueB", "first shard B");
	$this->assertEquals($this->getFromPort(11211, "c"), null, "first shard C");
	$this->assertEquals($this->getFromPort(11211, "d"), "valueD", "first shard D");

	//check second shard
	$this->assertEquals($this->getFromPort(11212, "b"), null, "second shard b");
	$this->assertEquals($this->getFromPort(11212, "c"), "valueC", "second shard c");
	$this->assertEquals($this->getFromPort(11212, "d"), null, "second shard d");

	//configuration with newhost
	$cm = new Cm([
	    ['host'=>'127.0.0.1', 'port'=>11211],
	    ['host'=>'127.0.0.1', 'port'=>11212],
	    ['newhost'=>'127.0.0.1', 'port'=>11221],
	], true);
	//old keys avalibaly
	$this->assertEquals($cm->get("b"), "valueB", "check b in new config");
	$this->assertEquals($cm->get("c"), "valueC", "check c in new config");
	$this->assertEquals($cm->get("d"), "valueD", "check d in new config");

	//reSet keys
	$cm->add("b", "valueB");
	$cm->add("c", "valueC");
	$cm->add("d", "valueD");

	//check keys for both configurations
	//check first shard
	$this->assertEquals($this->getFromPort(11211, "b"), "valueB", "new/first shard B");
	$this->assertEquals($this->getFromPort(11211, "c"), "valueC", "new/first shard C");
	$this->assertEquals($this->getFromPort(11211, "d"), "valueD", "new/first shard D");

	//check second shard
	$this->assertEquals($this->getFromPort(11212, "b"), null, "new/second shard B");
	$this->assertEquals($this->getFromPort(11212, "c"), "valueC", "new/second shard C");
	$this->assertEquals($this->getFromPort(11212, "d"), "valueD", "new/second shard D");

	//check new shard
	$this->assertEquals($this->getFromPort(11221, "b"), "valueB", "new/new shard B");
	$this->assertEquals($this->getFromPort(11221, "c"), null, "new/new shard C");
	$this->assertEquals($this->getFromPort(11221, "d"), null, "new/new shard D");

	$cm = new Cm([
	    ['host'=>'127.0.0.1', 'port'=>11211],
	    ['host'=>'127.0.0.1', 'port'=>11212],
	    ['host'=>'127.0.0.1', 'port'=>11221],
	], true);
	//old keys avalibaly
	$this->assertEquals($cm->get("b"), "valueB", "check b in only new config");
	$this->assertEquals($cm->get("c"), "valueC", "check c in only new config");
	$this->assertEquals($cm->get("d"), "valueD", "check d in only new config");

    }

    public function testSet()
    {
	$this->clear();
	//old (no newshard) configuration
	$cm = new Cm([
	    ['host'=>'127.0.0.1', 'port'=>11211],
	    ['host'=>'127.0.0.1', 'port'=>11212],
//	    ['newhost'=>'127.0.0.1', 'port'=>11221],
	], true);
	$this->assertTrue($cm->set("b", "valueB"));
	$this->assertTrue($cm->set("c", "valueC"));
	$this->assertTrue($cm->set("d", "valueD"));

	//check first shard
	$this->assertEquals($this->getFromPort(11211, "b"), "valueB", "first shard B");
	$this->assertEquals($this->getFromPort(11211, "c"), null, "first shard C");
	$this->assertEquals($this->getFromPort(11211, "d"), "valueD", "first shard D");

	//check second shard
	$this->assertEquals($this->getFromPort(11212, "b"), null, "second shard b");
	$this->assertEquals($this->getFromPort(11212, "c"), "valueC", "second shard c");
	$this->assertEquals($this->getFromPort(11212, "d"), null, "second shard d");

	//configuration with newhost
	$cm = new Cm([
	    ['host'=>'127.0.0.1', 'port'=>11211],
	    ['host'=>'127.0.0.1', 'port'=>11212],
	    ['newhost'=>'127.0.0.1', 'port'=>11221],
	], true);
	//old keys avalibaly
	$this->assertEquals($cm->get("b"), "valueB", "check b in new config");
	$this->assertEquals($cm->get("c"), "valueC", "check c in new config");
	$this->assertEquals($cm->get("d"), "valueD", "check d in new config");

	//reSet keys
	$this->assertTrue($cm->set("b", "valueB"));
	$this->assertTrue($cm->set("c", "valueC"));
	$this->assertTrue($cm->set("d", "valueD"));

	//check keys for both configurations
	//check first shard
	$this->assertEquals($this->getFromPort(11211, "b"), "valueB", "new/first shard B");
	$this->assertEquals($this->getFromPort(11211, "c"), "valueC", "new/first shard C");
	$this->assertEquals($this->getFromPort(11211, "d"), "valueD", "new/first shard D");

	//check second shard
	$this->assertEquals($this->getFromPort(11212, "b"), null, "new/second shard B");
	$this->assertEquals($this->getFromPort(11212, "c"), "valueC", "new/second shard C");
	$this->assertEquals($this->getFromPort(11212, "d"), "valueD", "new/second shard D");

	//check new shard
	$this->assertEquals($this->getFromPort(11221, "b"), "valueB", "new/new shard B");
	$this->assertEquals($this->getFromPort(11221, "c"), null, "new/new shard C");
	$this->assertEquals($this->getFromPort(11221, "d"), null, "new/new shard D");

	$cm = new Cm([
	    ['host'=>'127.0.0.1', 'port'=>11211],
	    ['host'=>'127.0.0.1', 'port'=>11212],
	    ['host'=>'127.0.0.1', 'port'=>11221],
	], true);
	//old keys avalibaly
	$this->assertEquals($cm->get("b"), "valueB", "check b in only new config");
	$this->assertEquals($cm->get("c"), "valueC", "check c in only new config");
	$this->assertEquals($cm->get("d"), "valueD", "check d in only new config");
    }

    private function getFromPort($port, $key)
    {
	$cmr = new Cm([['host'=>'127.0.0.1', 'port'=>$port]], true);
	return $cmr->get($key);
    }

    private function clear()
    {
	foreach (array(11211, 11212, 11221) as $port){
	    $cmr = new Cm([['host'=>'127.0.0.1', 'port'=>$port]]);
	    $cmr->remove('b');
	    $cmr->remove('c');
	    $cmr->remove('d');
	}
    }

}

