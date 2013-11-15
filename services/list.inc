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

/**
 *
 *
 * @param $command
 * @param $domain
 * @param $selector
 */
function _step_suggestion_retrieve($command, $domain, $arg1 = NULL, $arg2 = NULL, $uid = NULL) {
  $suggestions = array();

  $query = db_select('node', 'n');

  // Join the steps field collection fields.
  $query->join('field_data_field_fc_steps', 'step', 'n.nid = step.entity_id');
  $query->join('field_data_field_fc_step_command_1', 'command', 'step.entity_id = command.entity_id');
  $query->join('field_data_field_fc_step_command_2', 'arg1', 'step.entity_id = arg1.entity_id');
  $query->join('field_data_field_fc_step_command_3', 'arg2', 'step.entity_id = arg2.entity_id');

  // Join the params field collection fields.
  $query->join('field_data_field_parameters', 'parameters', 'step.entity_id = parameters.entity_id');
  $query->join('field_data_field_parameter_name', 'parameter_name',
    'step.entity_id = parameter_name.entity_id AND parameter_name.field_parameter_name_value = domain');
  $query->join('field_data_field_parameters', 'parameter', 'step.entity_id = parameter.entity_id');

  $query->condition('n.status', 1);
  // Load every walkthrough that has at least one match the arguments.
  $query->condition(
    db_or()->condition('command.field_fc_step_command_1_value', $command)
        ->condition('arg1.field_fc_step_command_1_value', $arg1)
        ->condition('arg1.field_fc_step_command_2_value', $arg2)
  );

  // Define the output fields.
  $query->fields('n', array('nid'));

  // Group by the author user id.
  $query->groupBy('node.uid');

  // Execute the query.
  $results = $query->execute()->fetchAll();
  foreach ($results as $result) {
    $node = node_load($result->nid);

  }

  return $suggestions;
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
