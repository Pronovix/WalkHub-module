<?php

function _listing_resource_index() {
  module_load_include('inc', 'walkhub');

  $parameters = $_GET;
  unset($parameters['q']);

  $filters = isset($parameters['filters']) ? $parameters['filters'] : array();
  $bundle = isset($parameters['bundle']) ? $parameters['bundle'] : NULL;
  $uuids = isset($parameters['uuids']) ? $parameters['uuids'] : array();
  $orderby = isset($parameters['orderby']) ? $parameters['orderby'] : NULL;
  $limit = isset($parameters['limit']) ? $parameters['limit'] : 0;
  $offset = isset($parameters['offset']) ? $parameters['offset'] : 0;

  if (empty($bundle) && empty($uuids) && empty($filters)) {
    return array();
  }

  $q = new EntityFieldQuery();

  $q->entityCondition('entity_type', 'node');
  if ($uuids) {
    $q->propertyCondition('uuid', $uuids, 'IN');
  }
  if ($bundle) {
    $q->entityCondition('bundle', $bundle, is_array($bundle) ? 'IN' : '=');
  }
  if ($filters && is_array($filters)) {
    foreach ($filters as $prop => $value) {
      if (is_array($value)) {
        if (isset($value['value']) && isset($value['operator'])) {
          $q->propertyCondition($prop, $value['value'], $value['operator']);
        }
      }
      else {
        $q->propertyCondition($prop, $value);
      }
    }
  }

  if ($orderby) {
    list($field, $direction) = explode(' ', "{$orderby} ASC");
    $q->propertyOrderBy($field, strtoupper($direction));
  }

  $q->addTag('node_access');

  if ($limit) {
    $q->range($offset, $limit);
  }

  drupal_alter('walkhub_listing', $q);

  $result = $q->execute();

  if (!$result || empty($result['node'])) {
    return array();
  }

  return array_filter(array_values(array_map(function ($node) {
    $serialize_class = 'Walkhub' . str_replace(' ', '', ucwords(str_replace('_', ' ', $node->type)));
    return class_exists($serialize_class) ?
      walkhub_serialize_safely(new $serialize_class($node)) :
      NULL;
  }, node_load_multiple(array_keys($result['node'])))));
}

function _list_resource_access() {
  return TRUE;
}
