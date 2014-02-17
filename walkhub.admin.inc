<?php
/**
 * @file
 * Administrative functions for Walkhub module.
 */

/**
 * Form callback 'admin/config/services/walkhub'.
 */
function walkhub_configuration_form($form) {

  $form['configuration'] = array(
    '#type' => 'fieldset',
    '#title' => t('Walkhub settings'),
  );

  $form['proxy'] = array(
    '#type' => 'fieldset',
    '#title' => t('Proxy'),
  );

  $form['proxy']['walkhub_proxy_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Proxy URL'),
    '#default_value' => variable_get('walkhub_proxy_url'),
  );

  return system_settings_form($form);
}

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

/**
 * Save the added patterns into databse.
 *
 * @param $form
 * @param $form_state
 */
function walkhub_command_patterns_form_submit($form, &$form_state) {
  foreach ($form_state['values']['commands'] as $command => $pattern) {
    db_merge('walhub_patterns')
      ->key(array('command' => $command))
      ->fields(array(
          'pattern' => $pattern,
      ))
      ->execute();
  }
}
