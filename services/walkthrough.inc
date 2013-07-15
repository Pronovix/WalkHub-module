<?php

class WalkhubWalkthrough implements WalkhubObject {

  /**
   * @filter none
   */
  public $uuid;

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
   * @filter check_plain
   */
  public $parameters;

  /**
   * @filter none
   */
  public $url;

  public function __construct(stdClass $node = NULL) {
    if ($node) {
      module_load_include('inc', 'walkhub');
      $this->uuid = $node->uuid;
      $this->name = $node->title;
      $this->description = walkhub_field_get_value($node, 'body');
      $this->steps = walkhub_field_get_references('node', $node, 'field_steps');
      $this->parameters = _walkhub_get_parameters($node);
      $this->url = call_user_func_array('url', node_uri($node) + array('options' => array('absolute' => TRUE))) . '/start';
    }
  }

  /**
   * Applies the contents of this container object to an existing entity.
   *
   * @param stdClass $entity
   */
  public function applyOnEntity(stdClass $entity) {
    // TODO: Implement applyOnEntity() method.
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
