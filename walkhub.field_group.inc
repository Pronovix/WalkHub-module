<?php
/**
 * @file
 * walkhub.field_group.inc
 */

/**
 * Implements hook_field_group_info().
 */
function walkhub_field_group_info() {
  $export = array();

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_selenium_commands|node|step|form';
  $field_group->group_name = 'group_selenium_commands';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'step';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Selenium commands',
    'weight' => '3',
    'children' => array(
      0 => 'field_command_1',
      1 => 'field_command_2',
      2 => 'field_command_3',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'formatter' => 'collapsible',
      'instance_settings' => array(
        'description' => '',
        'classes' => '',
        'required_fields' => 1,
      ),
    ),
  );
  $export['group_selenium_commands|node|step|form'] = $field_group;

  return $export;
}
