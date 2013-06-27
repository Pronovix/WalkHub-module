<?php
/**
 * @file
 * walkthrough_sets.features.inc
 */

/**
 * Implements hook_node_info().
 */
function walkthrough_sets_node_info() {
  $items = array(
    'walkthrough_set' => array(
      'name' => t('Walkthrough Set'),
      'base' => 'node_content',
      'description' => '',
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
  );
  return $items;
}
