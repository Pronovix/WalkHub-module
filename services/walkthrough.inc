<?php

class WalkhubWalkthrough implements WalkhubObject {

  /**
   * @filter check_plain
   */
  public $name;

  /**
   * @filter check_plain
   */
  public $description;

  /**
   * @filter none
   */
  public $steps;

  /**
   * @filter none
   */
  public $prerequisite;

  /**
   * @filter none
   */
  public $postrequisite;

  /**
   * @filter check_plain
   */
  public $tokens;

  public function __construct(stdClass $node = NULL) {
    if ($node) {
      module_load_include('inc', 'walkhub');
      $this->name = $node->title;
      $this->description = walkhub_field_get_value($node, 'body');
      $this->steps = walkhub_field_get_references('node', $node, 'field_steps');
      $this->prerequisite = walkhub_field_get_references('node', $node, 'field_prerequisite');
      if (is_array($this->prerequisite) && count($this->prerequisite)) {
        $this->prerequisite = $this->prerequisite[0];
      }
      $this->postrequisite = walkhub_field_get_references('node', $node, 'field_postrequisite');
      if (is_array($this->postrequisite) && count($this->postrequisite)) {
        $this->postrequisite = $this->postrequisite[0];
      }
      $this->tokens = _walkhub_get_tokens($node);
    }
  }
}

function _walkthrough_resource_retrieve($uuid) {
  module_load_include('inc', 'walkhub');
  $node = _walkhub_node_retrieve($uuid, 'walkthrough');
  return $node ? walkhub_serialize_safely(new WalkhubWalkthrough($node)) : $node;
}

function _walkthrough_resource_access($op = 'view', $args = array()) {
  module_load_include('inc', 'walkhub');
  return _walkhub_node_resource_access('walkthrough', $op, $args);
}
