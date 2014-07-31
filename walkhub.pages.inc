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
 * Submit handler for 'walkhub_record_form'.
 */
function walkhub_record_form_submit($form, &$form_state) {
  $walkthrough_node = (object) array(
    'type' => 'walkthrough',
    'language' => LANGUAGE_NONE,
    'title' => $form_state['values']['title'],
    '__password_parameter' => (bool) $form_state['values']['password_parameter'],
    'field_raw_input' => array(
      LANGUAGE_NONE => array(
        array(
          'value' => $form_state['values']['steps'],
        ),
      ),
    ),
    'field_severity_of_the_change' => array(
      LANGUAGE_NONE => array(
        array('value' => $form_state['values']['severity']),
      ),
    ),
  );

  node_object_prepare($walkthrough_node);
  node_save($walkthrough_node);

  $form_state['redirect'] = "node/{$walkthrough_node->nid}";
}

/**
 * Multi step form to create walkthroughs.
 */
function walkhub_create_walkthrough_form($form, &$form_state) {
  // Set up some default values at start.
  if (!isset($form_state['step'])) {
    // Initialize the walkthrough object.
    $walkthrough = new stdClass();
    $walkthrough->type = 'walkthrough';
    $walkthrough->language = LANGUAGE_NONE;
    node_object_prepare($walkthrough);
    $form_state['walkthrough'] = $walkthrough;

    // Initialize a step counter
    $form_state['step'] = 1;
  }

  // According to the current step load the proper form.
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
 * First step: Add the raw html selenium code.
 */
function walkhub_create_walkthrough_form_step_1($form, $form_state) {
  $form['selenium_code'] = array(
    '#type' => 'textarea',
    '#title' => t('Paste your selenium code'),
    '#rows' => 50,
    '#description' => t('Learn how to create selenium tests: !link', array('!link' => l(t('link'),
            'http://www.seleniumhq.org/docs/02_selenium_ide.jsp', array('attributes' => array('target'=>'_blank'))))),
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
 * Second step: Information about the walkthrough.
 */
function walkhub_create_walkthrough_form_step_2($form, $form_state) {
  global $user;

  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#required' => TRUE,
    '#default_value' => !empty($form_state['walkthrough']->title) ? $form_state['walkthrough']->title : NULL,
  );

  $severity_field = field_info_field('field_severity_of_the_change');
  $severity_info_instance = field_info_instance('node', 'field_severity_of_the_change', 'walkthrough');
  $form['severity'] = array(
    '#type' => 'select',
    '#title' => $severity_info_instance['label'],
    '#required' => TRUE,
    '#options' => $severity_field['settings']['allowed_values'],
  );

  $form['body'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#required' => TRUE,
    '#default_value' => !empty($form_state['walkthrough']->body[LANGUAGE_NONE][0]['value']) ?
            $form_state['walkthrough']->body[LANGUAGE_NONE][0]['value'] : NULL,
  );

  $user_groups = og_get_entity_groups('user', $user);
  $options = array();
  if (!empty($user_groups['node'])) {
    $group_nodes = node_load_multiple($user_groups['node']);
    foreach ($group_nodes as $nid => $node) {
      $options[$nid] = $node->title;
    }

    $form['user_groups'] = array(
      '#type' => 'select',
      '#title' => t('Collection'),
      '#options' => $options,
      '#empty_option' => 'Do not post into collection',
      '#empty_value' => FALSE,
      '#multiple' => TRUE,
      '#chosen' => TRUE,
    );
  }

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
 * Next button submit callback.
 */
function walkhub_create_walkthrough_form_next($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
  $form_state['walkthrough']->field_raw_input[LANGUAGE_NONE][0]['value'] = $form_state['values']['selenium_code'];
  $form_state['step']++;
}

/**
 * Previous button submit callback.
 */
function walkhub_create_walkthrough_form_previous($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
  $form_state['step']--;
}

/**
 * Submit function to save the defined walkthrough.
 */
function walkhub_create_walkthrough_form_save($form, &$form_state) {
  // Set the body, title and severity fields.
  $form_state['walkthrough']->title = $form_state['values']['title'];
  $form_state['walkthrough']->body[LANGUAGE_NONE][0]['value'] = $form_state['values']['body'];
  $form_state['walkthrough']->field_severity_of_the_change[LANGUAGE_NONE][0]['value'] = $form_state['values']['severity'];
  // Set the group audience field value.
  foreach ($form_state['values']['user_groups'] as $group) {
    $form_state['walkthrough']->og_group_ref[LANGUAGE_NONE][] = array(
      'target_id' => $group,
    );
  }

  // Save the walkthrough.
  node_save($form_state['walkthrough']);
  // Redirect the user to the newly created walkthrough page.
  $form_state['redirect'] = "node/{$form_state['walkthrough']->nid}";
}

/**
 * Page callback for 'node/%/embed.js'.
 */
function walkhub_embedjs_page($node) {
  return array(
    'nid' => $node->nid,
    'walkhub' => $_SERVER['HTTP_HOST'],
    'origin' => walkhub_get_origin(),
    'buttonuri' => url('node/' . $node->nid, array(
      'query' => array(
        'buttononly' => 1,
      ),
      'absolute' => TRUE,
    )),
    'parameters' => (array) (isset($_GET['parameters']) ? $_GET['parameters'] : array()),
    'useproxy' => (bool) (isset($_GET['useproxy']) ? $_GET['useproxy'] : FALSE),
  );
}

/**
 * Delivery callback for the embed.js
 *
 * @TODO add closure compiler support.
 */
function walkthrough_deliver_embedjs($page_callback_result) {
  drupal_add_http_header('Content-Type', 'application/javascript');

  $json = NULL;
  $key = NULL;
  if (is_array($page_callback_result) || is_object($page_callback_result)) {
    $json = drupal_json_encode($page_callback_result);
    $key = walkhub_get_id_hash(node_load($page_callback_result['nid']));
  }

  if ($json !== NULL) {
    print "window.WalkhubEmbedData = window.WalkhubEmbedData || {};\n";
    print "window.WalkhubEmbedData['{$key}'] = {$json};\n";
  }
  // TODO handle other cases

  readfile(drupal_get_path('module', 'walkhub') . '/embed.js');
}
