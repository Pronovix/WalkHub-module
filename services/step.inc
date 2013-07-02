<?php

class WalkhubStep implements WalkhubObject {

  /**
   * @filter check_plain
   */
  public $title;

  /**
   * @filter check_plain
   */
  public $description;

  /**
   * @filter check_plain
   */
  public $command;

  /**
   * @filter check_plain
   */
  public $pureCommand;

  /**
   * @filter none
   */
  public $arg1;

  /**
   * @filter none
   */
  public $arg2;

  /**
   * @filter none
   */
  public $highlight;

  /**
   * @filter none
   */
  public $andWait;

  public function __construct(stdClass $node = NULL) {
    if ($node) {
      module_load_include('inc', 'walkhub');
      $this->title = $node->title;
      $this->description = walkhub_field_get_value($node, 'body');
      $this->command = walkhub_field_get_value($node, 'field_command_1');
      $this->andWait = strlen($this->command) > 7 && substr($this->command, -7) === 'AndWait';
      $this->pureCommand = $this->andWait ? substr($this->command, 0, -7) : $this->command;
      $this->arg1 = walkhub_field_get_value($node, 'field_command_2');
      $this->arg2 = walkhub_field_get_value($node, 'field_command_3');
      $this->highlight = walkhub_field_get_value($node, 'field_step_highlight');
    }
  }
}

function _step_resource_retrieve($uuid) {
  module_load_include('inc', 'walkhub');
  $node = _walkhub_node_retrieve($uuid, 'step');
  return $node ? walkhub_serialize_safely(new WalkhubStep($node)) : $node;
}

function _step_resource_access($op = 'view', $args = array()) {
  module_load_include('inc', 'walkhub');
  return _walkhub_node_resource_access('step', $op, $args);
}
