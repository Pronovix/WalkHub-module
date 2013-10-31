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
function walkhub_create_walkthrough_form($form, $form_state) {
  if (!isset($form_state['step'])) {
    $form_state['step'] = 1;
  }


  switch ($form_state['step']) {
    case 1:
      $form += walkhub_create_walkthrough_form_step_1();
      break;
    case 2:
      $form += walkhub_create_walkthrough_form_step_2();
      break;
    case 3:
      $form += walkhub_create_walkthrough_form_step_3();
      break;
  }

  return $form;
}

/**
 * @return mixed
 */
function walkhub_create_walkthrough_form_step_1() {
  $form['selenium_code'] = array(
    '#type' => 'textarea',
    '#title' => t('Raw selenium'),
  );

  return $form;
}

/**
 * @return array
 */
function walkhub_create_walkthrough_form_step_2() {
  $form = array();
  return $form;
}

/**
 * @return array
 */
function walkhub_create_walkthrough_form_step_3() {
  $form = array();
  return $form;
}
