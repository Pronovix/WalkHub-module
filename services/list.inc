<?php

function _list_resource_retrieve($view_name) {
  module_load_include('resource.inc', 'services_views');
  // TODO: make sure that the view is being checked for at least the following:
  // - node based view
  // - lists walkthroughs
  // - tagged with walkhub
  // - lists uuids
  $items = services_views_retrieve($view_name);
  $return = array();
  foreach ($items as $item) {
    $newitem = array();
    foreach ($item as $key => $value) {
      if (strpos($key, 'node_') === 0) {
        $key = substr($key, 5);
      }
      $newitem[$key] = $value;
    }
    $return[] = $newitem;
  }

  return $return;
}

function _list_resource_index($name = '', $description = '', $tag = '', $base_table = '', $human_name = '') {
  $q = db_select('views_view', 'v')
    ->fields('v', array('vid', 'name', 'description', 'tag', 'base_table', 'human_name', 'core'));
  if ($name) {
    $q->condition('name', $name, 'LIKE');
  }
  if ($description) {
    $q->condition('description', $description, 'LIKE');
  }
  if ($tag) {
    $q->condition('tag', "%{$tag}%", 'LIKE');
  }
  if ($base_table) {
    $q->condition('base_table', $base_table, 'LIKE');
  }
  if ($human_name) {
    $q->condition('human_name', $human_name, 'LIKE');
  }

  return array_values(array_filter($q->execute()->fetchAllAssoc('vid'), function ($view) {
    $loaded_view = views_get_view($view->name);
    return $loaded_view && $loaded_view->access('default');
  }));
}

function _list_resource_access($op = 'view', $args = array()) {
  // TODO figure out what to do here
  return TRUE;
}
