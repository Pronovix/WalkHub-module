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
 *
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

function _walkthrough_resource_access($op = 'view', $args = array()) {
  module_load_include('inc', 'walkhub');
  return _walkhub_node_resource_access('walkthrough', $op, $args);
}
