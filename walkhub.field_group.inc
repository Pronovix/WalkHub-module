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
  $field_group->identifier = 'group_advance|node|walkthrough|form';
  $field_group->group_name = 'group_advance';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'walkthrough';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Advanced',
    'weight' => '3',
    'children' => array(
      0 => 'field_postrequisite',
      1 => 'field_prerequisite',
      2 => 'field_steps',
      3 => 'field_tags',
      4 => 'field_tokens',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'label' => 'Advanced',
      'instance_settings' => array(
        'required_fields' => 1,
        'classes' => '',
        'description' => '',
      ),
      'formatter' => 'collapsed',
    ),
  );
  $export['group_advance|node|walkthrough|form'] = $field_group;

  return $export;
}
