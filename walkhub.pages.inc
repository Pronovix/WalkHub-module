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
