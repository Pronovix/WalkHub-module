<?php

function _list_resource_retrieve($view_name) {
  module_load_include('resource.inc', 'services_views');
  // TODO: make sure that the view is being checked for at least the following:
  // - node based view
  // - lists walkthroughs
  // - tagged with walkthrough
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

function _list_resource_access($op = 'view', $args = array()) {
  // TODO figure out what to do here
  return TRUE;
}
