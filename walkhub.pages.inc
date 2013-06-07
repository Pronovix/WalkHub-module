<?php

/**
 * Loading page for a walkthrough.
 */
function walkhub_walkhub_page() {
  $content = t('Walkthrough is loading...');
  drupal_set_title($content);

  return $content;
}
