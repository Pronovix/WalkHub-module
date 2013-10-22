<?php

class WalkhubStep implements WalkhubObject {

  /**
   * @filter none
   */
  public $uuid;

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

  /**
   * @filter none
   */
  public $showTitle;

  public function __construct(FieldCollectionItemEntity $step = NULL) {
    if ($step) {
      module_load_include('inc', 'walkhub');
      if ($step->hostEntityType() !== 'node' || $step->hostEntityBundle() !== 'walkthrough') {
        return;
      }
      $this->uuid = $step->uuid;
      $this->titleRaw = $this->title = walkhub_field_get_value($step, 'field_fc_step_name');
      $this->descriptionRaw = $this->description = walkhub_field_get_value($step, 'field_fc_step_description');
      $this->commandRaw = $this->command = walkhub_field_get_value($step, 'field_fc_step_command_1');
      $this->andWait = strlen($this->command) > 7 && substr($this->command, -7) === 'AndWait';
      $this->pureCommandRaw = $this->pureCommand = $this->andWait ? substr($this->command, 0, -7) : $this->command;
      $this->arg1 = walkhub_field_get_value($step, 'field_fc_step_command_2');
      $this->arg2 = walkhub_field_get_value($step, 'field_fc_step_command_3');
      $this->highlight = walkhub_field_get_value($step, 'field_fc_step_highlight');
      $this->canEdit = node_access('update', $step->hostEntity());
      $this->showTitle = (bool) walkhub_field_get_value($step, 'field_fc_step_show_title');
    }
  }

  /**
   * Applies the contents of this container object to an existing entity.
   *
   * @param stdClass $entity
   */
  public function applyOnEntity($entity) {
    $entity->field_fc_step_name[$entity->langcode()][0]['value'] = $this->titleRaw;
    $entity->field_fc_step_description[$entity->langcode()][0]['value'] = $this->descriptionRaw;
    $entity->field_fc_step_show_title[$entity->langcode()][0]['value'] = (int)(bool)$this->showTitle;
  }
}

function _step_resource_retrieve($uuid) {
  module_load_include('inc', 'walkhub');
  $entities = walkhub_load_entity('field_collection_item', $uuid);
  $entity = reset($entities);
  return $entity ? walkhub_serialize_safely(new WalkhubStep($entity)) : $entity;
}

function _step_resource_update($uuid, $step) {
  module_load_include('inc', 'walkhub');
  $step = walkhub_convert_stdclass_to_object((object) $step, 'WalkhubStep');
  $fci = _walkhub_field_collection_item_retrieve($uuid, 'field_fc_steps');
  $step->applyOnEntity($fci);
  $fci->save(TRUE);
  return walkhub_serialize_safely(new WalkhubStep($fci));
}

function _step_resource_access($op = 'view', $args = array()) {
  module_load_include('inc', 'walkhub');
  module_load_include('inc', 'services', 'resources/node_resource.inc');
  $entities = walkhub_load_entity('field_collection_item', $args[0]);
  if (!$entities) {
    return FALSE;
  }
  $entity = reset($entities);
  $node = $entity->hostEntity();
  if ((!$node) || ($node->type !== 'walkthrough')) {
    return FALSE;
  }
  return _walkhub_node_resource_access('walkthrough', $op, array($node->uuid));
}
