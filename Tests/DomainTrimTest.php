<?php

include 'walkhub.module';

class DomainTrimTest extends PHPUnit_Framework_TestCase  {

  /**
   * @dataProvider provider
   */
  public function testDomainTrim($original, $correctly_trimmed) {
    $this->assertEquals(_walkhub_get_domain_from_walkthrough_url_parameter($original), $correctly_trimmed);
  }

  public function provider() {
    $data = array(
      array('',                   ''),
      array('localhost',          'localhost'),
      array('pronovix.com',       'pronovix.com'),
      array('pronovix.com/',      'pronovix.com'),
      array('pronovix.com',       'pronovix.com'),
      array('pronovix.com/about', 'pronovix.com/about'),
    );

    foreach (array('http', 'https') as $protocol) {
      $domains_with_protocol = array(
        array("$protocol://pronovix.com",        'pronovix.com'),
        array("$protocol://pronovix.com/",       'pronovix.com'),
        array("$protocol://pronovix.com/about",  'pronovix.com/about'),
        array("$protocol://pronovix.com/about/", 'pronovix.com/about'),
      );

      $data = array_merge($data, $domains_with_protocol);
    }

    return $data;
  }

}