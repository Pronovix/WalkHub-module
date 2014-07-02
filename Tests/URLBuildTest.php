<?php

include 'walkhub.module';

class URLBuildTest extends PHPUnit_Framework_TestCase {

  /**
   * @dataProvider provider
   */
  public function testURLBuild($original, $replace, $expected) {
    $this->assertEquals($expected, walkhub_build_url($original, $replace));
  }

  public function provider() {
    $data = [
      ['http://foo.bar', ['host' => '[baz]'], 'http://[baz]'],
      ['http://foo.bar/baz', ['query' => 'spam=ham'], 'http://foo.bar/baz?spam=ham'],
    ];

    return $data;
  }

}
