<?php

class WalkhubStep implements WalkhubObject {

  /**
   * @filter check_plain
   */
  public $title;

  /**
   * @filter none
   */
  public $titleRaw;

  /**
   * @filter check_plain
   */
  public $description;

  /**
   * @filter none
   */
  public $descriptionRaw;

  /**
   * @filter check_plain
   */
  public $command;

  /**
   * @filter none
   */
  public $commandRaw;

  /**
   * @filter check_plain
   */
  public $pureCommand;

  /**
   * @filter none
   */
  public $pureCommandRaw;

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

  /**
   * @filter none
   */
  public $canEdit;

  public function __construct(stdClass $node = NULL) {
    if ($node) {
      module_load_include('inc', 'walkhub');
      $this->titleRaw = $this->title = $node->title;
      $this->descriptionRaw = $this->description = walkhub_field_get_value($node, 'body');
      $this->commandRaw = $this->command = walkhub_field_get_value($node, 'field_command_1');
      $this->andWait = strlen($this->command) > 7 && substr($this->command, -7) === 'AndWait';
      $this->pureCommandRaw = $this->pureCommand = $this->andWait ? substr($this->command, 0, -7) : $this->command;
      $this->arg1 = walkhub_field_get_value($node, 'field_command_2');
      $this->arg2 = walkhub_field_get_value($node, 'field_command_3');
      $this->highlight = walkhub_field_get_value($node, 'field_step_highlight');
      $this->canEdit = node_access('update', $node);
    }
  }

  /**
   * Applies the contents of this container object to an existing entity.
   *
   * @param stdClass $entity
   */
  public function applyOnEntity(stdClass $entity) {
    $entity->title = $this->titleRaw;
    $entity->body[$entity->language][0]['value'] = $this->descriptionRaw;
  }
}

function _step_resource_retrieve($uuid) {
  module_load_include('inc', 'walkhub');
  $node = _walkhub_node_retrieve($uuid, 'step');
  return $node ? walkhub_serialize_safely(new WalkhubStep($node)) : $node;
}

function _step_resource_update($uuid, $step) {
  module_load_include('inc', 'walkhub');
  $step = walkhub_convert_stdclass_to_object((object) $step, 'WalkhubStep');
  $node = _walkhub_node_retrieve($uuid, 'step');
  $step->applyOnEntity($node);
  node_save($node);
  return walkhub_serialize_safely(new WalkhubStep($node));
}

function _step_resource_access($op = 'view', $args = array()) {
  module_load_include('inc', 'walkhub');
  return _walkhub_node_resource_access('step', $op, $args);
}
