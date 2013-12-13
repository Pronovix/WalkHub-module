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
 * Retrieve a few walkthrough step descriptions that fits the context arguments.
 *
 * @param $domain
 *  The domain parameter where the walkthrough will execute.
 * @param $command
 *  The executable walkthrough command.
 * @param null $arg1
 *  Walkthrough argument first.
 * @param null $arg2
 *  Walkthrough argument second.
 * @param null $uid
 *  The uid of the walkthrough author.
 *
 * @return array
 *  A list descriptions.
 */
function _step_suggestion_retrieve($domain, $command, $arg1 = NULL, $arg2 = NULL, $uid = NULL) {
  $suggestions = array();

  $query = db_select('node', 'n');

  // Join the steps field collection fields.
  $query->join('field_data_field_fc_steps', 'step', 'n.nid = step.entity_id');

  $query->join('field_data_field_fc_step_description', 'description','step.field_fc_steps_value = description.entity_id');

  $query->join('field_data_field_fc_step_command_1', 'command',
    'step.field_fc_steps_value = command.entity_id AND command.field_fc_step_command_1_value = :command',
    array(':command' => $command));
  $query->leftJoin('field_data_field_fc_step_command_2', 'arg1', 'step.field_fc_steps_value = arg1.entity_id');
  $query->leftJoin('field_data_field_fc_step_command_3', 'arg2', 'step.field_fc_steps_value = arg2.entity_id');

  // Join the params field collection fields.
  $query->join('field_data_field_parameters', 'parameters', 'step.entity_id = parameters.entity_id');
  // Join the parameter name.
  $query->join('field_data_field_parameter_name', 'parameter_name',
    'parameters.field_parameters_value = parameter_name.entity_id');
  // Join only those steps that domain parameter is correspond with $domain.
  $query->leftJoin('field_data_field_parameter_default_value', 'parameter_default_value',
    'parameter_name.entity_id = parameter_default_value.entity_id AND
     parameter_name.field_parameter_name_value = :label AND
     parameter_default_value.field_parameter_default_value_value = :domain',
    array(':domain' => $domain, ':label' => 'domain'));

  // Select only public nodes.
  $query->condition('n.status', 1);
  // Select only steps with not null description fields.
  $query->isNotNull('description.field_fc_step_description_value');
  // Define the output fields.
  $query->fields('step', array('field_fc_steps_value'));
  $query->fields('n', array('uid'));
  $query->fields('arg1', array('field_fc_step_command_2_value'));
  $query->fields('arg2', array('field_fc_step_command_3_value'));
  $query->fields('description', array('field_fc_step_description_value'));

  // Execute the query.
  $steps = $query->execute()->fetchAll();

  foreach ($steps as $step) {
    $weight = 0;
    $suggestions[] = $step->field_fc_step_description_value;

    // If the same user submitted the description increase the weight.
    if ($step->uid == $uid) {
      $weight += 1;
    }

    if ($step->field_fc_step_command_2_value == $arg1) {
      $weight += 1;
    }

    if ($step->field_fc_step_command_3_value == $arg2) {
      $weight += 1;
    }

    $weight[] = $weight;
  }
  // Sort suggestions by weight.
  array_multisort($weight, $suggestions);

  return array_slice(array_values(array_filter(array_unique($suggestions))), 0, 5);
}

function _step_suggestion_resource_access() {
  return user_access('administer walkthrough');
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
