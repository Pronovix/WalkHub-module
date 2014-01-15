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
      $this->steps = walkhub_field_get_references('field_collection_item', $node, 'field_fc_steps', $node->language, 'value');
      $this->parameters = _walkhub_get_parameters($node);
      $this->url = call_user_func_array('url', node_uri($node) + array('options' => array('absolute' => TRUE))) . '/start';
    }
  }

  /**
   * Applies the contents of this container object to an existing entity.
   *
   * @param stdClass $entity
   */
  public function applyOnEntity($entity) {
    // TODO: Implement applyOnEntity() method.
  }
}

function _walkthrough_resource_retrieve($uuid) {
  module_load_include('inc', 'walkhub');
  $node = _walkhub_node_retrieve($uuid, 'walkthrough');
  return $node ? walkhub_serialize_safely(new WalkhubWalkthrough($node)) : $node;
}

/**
 * Get walkthrough prerequisites.
 *
 * @param string $uuid
 *  The walkthrough node uuid.
 *
 * @return array
 *  An array of prerequisites
 */
function _walkthrough_prerequisites_retrieve($uuid) {
  $return = array();

  $walkthrough = _walkhub_node_retrieve($uuid, 'walkthrough');
  $prerequisites = field_get_items('node', $walkthrough, 'field_prerequisites', LANGUAGE_NONE);

  if (!empty($prerequisites)) {
    foreach($prerequisites as $prerequisite) {
      if ($node = node_load($prerequisite['target_id'])) {
        $return[] = array(
          'title' => $node->title,
          'nid' => $node->nid,
        );
      }
    }
  }

  return $return;
}

/**
 * Export a walkthrough to PHPUnit case.
 *
 * @param string $uuid
 *  The walkthrough node uuid.
 *
 * @return string
 *  PHPUnit test case.
 */
function _walkthrough_phpunit_retrieve($uuid, $class_to_extend = 'PHPUnit_Extensions_Selenium2TestCase') {
  dd(func_get_args());
  $return = array();

  $walkthrough = _walkhub_node_retrieve($uuid, 'walkthrough');
  if (!empty($walkthrough)) {
    module_load_include('inc', 'walkhub', 'walkhub.export');
    $return = _walkhub_get_phpunit_export_output($walkthrough, WALKHUB_SELENIUM_EXPORT_VIA_WALKHUB, $class_to_extend);
  }

  return $return;
}

/**
 * Sets a walkthrough screening flag.
 *
 * @param string $uuid
 * @param array $flag_value
 */
function _walkthrough_update_screening_flag($uuid, $flag_value) {
  $entities = walkhub_load_entity('node', $uuid);
  $node = reset($entities);

  if ($node) {
    $mark_for_screening = flag_get_flag('mark_for_screening');
    $action = $flag_value['flag'] ? 'flag' : 'unflag';
    $mark_for_screening->flag($action, $node->nid);
  }
}

function _walkthrough_resource_access($op = 'view', $args = array()) {
  module_load_include('inc', 'walkhub');
  return _walkhub_node_resource_access('walkthrough', $op, $args);
}

function _walkthrough_or_walkthrough_set_resource_access($op = 'view', $args = array()) {
  module_load_include('inc', 'services', 'resources/node_resource');

  $uuid = $args[0];
  $allowed_types = array('walkthrough', 'walkthrough_set');

  foreach ($allowed_types as $type) {
    $node = _walkhub_node_retrieve($uuid, $type, TRUE);
    if ($node) {
      return _node_resource_access($op, array($node));
    }
  }

  throw new ServicesException(t('Failed to load !uuid as Walkthrough or Walkthrough suite', array('!uuid' => $uuid)), 404);
}
