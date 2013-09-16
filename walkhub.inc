<?php

/**
 * Interface for all Walkhub response objects.
 */
interface WalkhubObject {

  /**
   * Applies the contents of this container object to an existing entity.
   *
   * @param stdClass $entity
   */
  public function applyOnEntity(stdClass $entity);
}

/**
 * Extracts and filter the properties of WalkhubObject descendants.
 *
 * This function works differently on object. If an object is a descendant of
 * WalkhubObject, then only those accessible properties will be serialized which
 * have the @filter directive in its docblock. This directive must be followed
 * by a name of the filtering function.
 *
 * Example:
 * @filter check_plain
 *
 * If the object is a regular object, then all of its accessible properties will
 * be added to the extraction.
 *
 * @param object $obj
 *   Object to serialize
 *
 * @return stdClass
 *   Extracted object
 */
function walkhub_serialize_safely($obj) {
  $result = new stdClass;

  $refl = new ReflectionObject($obj);
  foreach ($refl->getProperties() as $property) {
    $doc = $property->getDocComment();
    $matches = array();
    if (preg_match('/@filter ([a-z0-9_]+)/im', $doc, $matches)) {
      $filter = $matches[1];
      if ($filter === 'none') {
        $filter = function ($val) { return $val; };
      }
      if (!is_callable($filter)) {
        continue;
      }

      $result->{$property->getName()} = _walkhub_serialize_safely_filter_value($property->getValue($obj), $filter);
    }
  }

  return $result;
}

/**
 * Helper callback for walkhub_serialize_safely().
 *
 * This function applies the filter function on scalar values, or calls itself
 * recursively in case of arrays or objects.
 *
 * @param mixed $value
 *   Value to filter
 * @param $filter
 *   Filter funciton
 *
 * @return array
 */
function _walkhub_serialize_safely_filter_value($value, $filter) {
  if (is_object($value)) {
    $value = ($value instanceof WalkhubObject) ?
      walkhub_serialize_safely($value) :
      (array) $value;
  }
  if (is_array($value)) {
    return array_map(function ($item) use ($filter) {
      return _walkhub_serialize_safely_filter_value($item, $filter);
    }, $value);
  }

  return $filter($value);
}

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
 * Returns a condensed representation of a Drupal image field.
 *
 * @param $entity
 *   Entity.
 * @param $field
 *   Field name.
 * @param bool $multi
 *   FALSE if it's a single value field, TRUE otherwise.
 * @param string $language
 *   Field language.
 *
 * @return array|mixed
 */
function walkhub_image_field_get_url($entity, $field, $multi = FALSE, $language = LANGUAGE_NONE) {
  $values = (array) walkhub_field_get_value($entity, $field, $multi, $language, 'uri');
  $values = array_filter(array_map(function ($uri) {
    return ($wrapper = file_stream_wrapper_get_instance_by_uri($uri)) ?
      $wrapper->getExternalUrl() :
      FALSE;
  }, $values));

  return $multi ? $values : reset($values);
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
  if (!$nids) {
    return array();
  }

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
 * @param bool $disable_exception
 *   Disables throwing an exception.
 *
 * @throws ServicesException
 * @return bool|stdClass
 */
function _walkhub_node_retrieve($uuid, $type, $disable_exception = FALSE) {
  $entities = walkhub_load_entity('node', $uuid);
  $node = reset($entities);

  if ((!$node) || (is_array($type) ? !in_array($node->type, $type) : ($node->type !== $type))) {
    if ($disable_exception) {
      return FALSE;
    }
    else {
      throw new ServicesException(t('Failed to load !uuid', array('!uuid' => $uuid)), 404);
    }
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

/**
 * Converts an stdClass into a real object.
 *
 * This function is helpful restoring the original object from a JSON object.
 *
 * @param stdClass $object
 *   The object, holding the data.
 * @param string $type
 *   Type of the object to restore.
 *
 * @return mixed
 *   Restored object.
 */
function walkhub_convert_stdclass_to_object(stdClass $object, $type) {
  $result = new $type();

  foreach ($object as $property => $value) {
    $result->{$property} = $value;
  }

  return $result;
}
