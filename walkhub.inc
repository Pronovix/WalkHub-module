<?php

/**
 * Interface for all Walkhub response objects.
 */
interface WalkhubObject {}

/**
 * Loads an entity from it's UUID.
 *
 * @param string $type
 *   Entity type.
 * @param string $uuid
 *   Entity UUID.
 *
 * @return mixed
 */
function walkhub_load_entity($type, $uuid) {
  return entity_load($type, FALSE, array('uuid' => $uuid));
}

/**
 * Returns a condensed representation of a Drupal field.
 *
 * @param stdClass $entity
 *   Entity.
 * @param string $field
 *   Field name.
 * @param bool $multi
 *   FALSE if it's a single value field, TRUE otherwise.
 * @param string $language
 *   Field language.
 * @param string $value_name
 *   Name of the value key of the field.
 *
 * @return array|string|null
 */
function walkhub_field_get_value($entity, $field, $multi = FALSE, $language = LANGUAGE_NONE, $value_name = 'value') {
  $field = isset($entity->{$field}[$language]) ? $entity->{$field}[$language] : NULL;
  if ($field) {
    if ($multi) {
      return array_map(function ($item) use ($value_name) {
        return $item[$value_name];
      }, $field);
    }

    return $field[0][$value_name];
  }

  return NULL;
}

/**
 * Extracts entity references from a reference field.
 *
 * @param string $entity_type
 *   Referenced entity type.
 * @param stdClass $entity
 *   Entity object.
 * @param string $field
 *   Reference field name.
 * @param string $language
 *   Reference field language.
 * @param string $value_name
 *   Name of the reference value key.
 *
 * @return array
 */
function walkhub_field_get_references($entity_type, $entity, $field, $language = LANGUAGE_NONE, $value_name = 'target_id') {
  $nids = walkhub_field_get_value($entity, $field, TRUE, $language, $value_name);
  $uuids = entity_get_uuid_by_id($entity_type, $nids);
  $ordered_uuids = array();
  foreach ($nids as $nid) {
    if (isset($uuids[$nid])) {
      $ordered_uuids[] = $uuids[$nid];
    }
  }

  return $ordered_uuids;
}

/**
 * Loads a node from it's UUID with a given type.
 *
 * @param string $uuid
 *   Node UUID.
 * @param string $type
 *   Node type.
 *
 * @return bool|stdClass
 */
function _walkhub_node_retrieve($uuid, $type) {
  $node = reset(walkhub_load_entity('node', $uuid));
  if (!$node) {
    return FALSE;
  }

  if ($node->type !== $type) {
    return FALSE;
  }

  return $node;
}

/**
 * Wraps the node resource's access callback to work with UUIDs.
 *
 * @param string $type
 *   Content type.
 * @param string $op
 *   Access operation.
 * @param array $args
 *   Arguments from services. The first item must be the UUID.
 *
 * @return bool
 */
function _walkhub_node_resource_access($type, $op = 'view', $args = array()) {
  module_load_include('inc', 'services', 'resources/node_resource.inc');
  $node = _walkhub_node_retrieve($args[0], $type);
  if (!$node) {
    return FALSE;
  }
  return _node_resource_access($op, array($node));
}
