<?php

/**
 * Loading page for a walkthrough.
 */
function walkhub_walkhub_page() {
  $content = t('Walkthrough is loading...');
  drupal_set_title($content);

  return $content;
}

/**
 * Autostart page callback.
 */
function walkhub_start_walkthrough($node) {
  $getparams = $_GET;
  unset($getparams['q']);
  $uri = entity_uri('node', $node);
  drupal_goto($uri['path'], array(
    'query' => array(
      'autostart' => 1,
    ) + $getparams,
  ));
}

/**
 *
 */
function walkhub_create_walkthrough_form($form, &$form_state) {
  if (!isset($form_state['step'])) {
    $walkthrough = new stdClass();
    $walkthrough->type = 'walkthrough';
    node_object_prepare($walkthrough);
    $form_state['walkthrough'] = $walkthrough;

    $form_state['step'] = 1;
  }

  switch ($form_state['step']) {
    case 1:
      $form += walkhub_create_walkthrough_form_step_1($form, $form_state);
      break;
    case 2:
      $form += walkhub_create_walkthrough_form_step_2($form, $form_state);
      break;
  }

  return $form;
}

/**
 * @return mixed
 */
function walkhub_create_walkthrough_form_step_1($form, $form_state) {
  $form['selenium_code'] = array(
    '#type' => 'textarea',
    '#title' => t('Raw selenium'),
    '#default_value' => !empty($form_state['walkthrough']->field_raw_input[LANGUAGE_NONE][0]['value']) ?
            $form_state['walkthrough']->field_raw_input[LANGUAGE_NONE][0]['value'] : NULL,
  );

  $form['next'] = array(
    '#type' => 'submit',
    '#value' => t('Next'),
    '#submit' => array('walkhub_create_walkthrough_form_next'),
  );

  return $form;
}

/**
 *
 *
 * @return array
 */
function walkhub_create_walkthrough_form_step_2($form, $form_state) {
  $form = array();

  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#required' => TRUE,
    '#default_value' => !empty($form_state['walkthrough']->title) ? $form_state['walkthrough']->title : NULL,
  );

  $form['body'] = array(
    '#type' => 'textarea',
    '#title' => t('Body'),
    '#required' => TRUE,
    '#default_value' => !empty($form_state['walkthrough']->body[LANGUAGE_NONE][0]['value']) ?
            $form_state['walkthrough']->body[LANGUAGE_NONE][0]['value'] : NULL,
  );

  $form['previous'] = array(
    '#type' => 'submit',
    '#value' => t('Previous'),
    '#limit_validation_errors' => array(),
    '#submit' => array('walkhub_create_walkthrough_form_previous'),
  );

  $form['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#submit' => array('walkhub_create_walkthrough_form_save'),
  );

  return $form;
}

/**
 * @param $form
 * @param $form_state
 */
function walkhub_create_walkthrough_form_next($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
  walkhub_create_walkthrough_save_values($form_state, $form_state['step']);
  $form_state['step']++;
}

/**
 * @param $form
 * @param $form_state
 */
function walkhub_create_walkthrough_form_previous($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
  walkhub_create_walkthrough_save_values($form_state, $form_state['step']);
  $form_state['step']--;
}

/**
 *
 */
function walkhub_create_walkthrough_form_save($form, &$form_state) {
  $walkthrough = node_save($form_state['walkthrough']);
  $form_state['#redirect'] = "node/{$walkthrough->nid}";
}

/**
 *
 */
function walkhub_create_walkthrough_save_values(&$form_state, $step) {
  switch ($step) {
    case 1:
      $form_state['walkthrough']->field_raw_input[LANGUAGE_NONE][0]['value'] = $form_state['values']['selenium_code'];
      break;
    case 2:
      $form_state['walkthrough']->title = $form_state['values']['title'];
      $form_state['walkthrough']->body[LANGUAGE_NONE][0]['value'] = $form_state['values']['body'];
      break;
  }
}
