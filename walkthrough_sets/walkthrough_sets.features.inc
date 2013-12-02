<?php
/**
 * @file
 * walkthrough_sets.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function walkthrough_sets_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_node_info().
 */
function walkthrough_sets_node_info() {
  $items = array(
    'walkthrough_set' => array(
      'name' => t('Walkthrough suite'),
      'base' => 'node_content',
      'description' => '',
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
  );
  return $items;
}
