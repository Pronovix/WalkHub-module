<?php
/**
 * @file
 * Administrative functions for Walkhub module.
 */

/**
 * Renders the commend patterns form.
 *
 * @param $form
 * @param $form_state
 *
 * @return array
 */
function walkhub_command_patterns_form($form, &$form_state) {
  // Get the supported command list and the stored patterns from the database.
  $commands = array_keys(_walkhub_get_supported_selenium_commands());
  $patterns = db_query("SELECT * FROM {walhub_patterns}")->fetchAllAssoc('command');

  $form['commands'] = array(
    '#tree' => TRUE,
  );
  foreach ($commands as $command) {
    $form['commands'][$command] = array(
      '#type' => 'textarea',
      '#title' => $command,
      '#default_value' => isset($patterns[$command]) ? $patterns[$command]->pattern : '',
    );
  }

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}
