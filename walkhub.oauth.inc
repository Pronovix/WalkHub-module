<?php
/**
 * @file
 * walkhub.oauth.inc
 */

/**
 * Implements hook_default_oauth_common_context().
 */
function walkhub_default_oauth_common_context() {
  $export = array();

  $context = new stdClass();
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 1;
  $context->name = 'walkthrough_selenium_executor';
  $context->title = 'Walkthrough selenium executor';
  $context->authorization_options = array(
    'access_token_lifetime' => NULL,
    'page_title' => NULL,
    'message' => NULL,
    'warning' => NULL,
    'deny_access_title' => NULL,
    'grant_access_title' => NULL,
    'disable_auth_level_selection' => NULL,
    'signature_methods' => array(
      0 => 'HMAC-SHA1',
      1 => 'HMAC-SHA256',
      2 => 'HMAC-SHA384',
      3 => 'HMAC-SHA512',
    ),
    'default_authorization_levels' => array(),
  );
  $context->authorization_levels = array();
  $export['walkthrough_selenium_executor'] = $context;

  return $export;
}
