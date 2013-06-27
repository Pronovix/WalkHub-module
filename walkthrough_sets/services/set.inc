<?php

class WalkhubWalkthroughSet implements WalkhubObject {
  public $title;
  public $description;
  public $links;

  public function __construct(stdClass $node = NULL) {
    if ($node) {
      module_load_include('inc', 'walkhub');
      $this->title = $node->title;
      $this->description = walkhub_field_get_value($node, 'description');
      $this->links = walkhub_field_get_value($node, 'field_walkthroughs', TRUE);
    }
  }
}

function _walkthrough_set_resource_retrieve($uuid) {
  module_load_include('inc', 'walkhub');
  $node = _walkhub_node_retrieve($uuid, 'walkthrough_set');
  return $node ? (new WalkhubWalkthroughSet($node)) : $node;
}

function _walkthrough_set_resource_access($op = 'view', $args = array()) {
  module_load_include('inc', 'walkhub');
  return _walkhub_node_resource_access('walkthrough_set', $op, $args);
}
