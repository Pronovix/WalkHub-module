<?php

/**
 * Menu callback to export walkthrough node to selenium.
 *
 * @param $node
 *   Node to export.
 */
function walkhub_selenium_export($node) {
  $output = _walkhub_get_selenium_export_output($node);

  header('Content-type: text/html');
  header('Content-Disposition: attachment; filename="'. check_plain($node->title) . '.html"');
  echo $output;

  module_invoke_all('exit');
  exit();
}

/**
 * Helper function to get the selenium export output.
 *
 * @param $node
 *   Node to export.
 *
 * @return string
 *   Exported selenium testcase.
 */
function _walkhub_get_selenium_export_output($node) {
  $dom = _walkhub_prepare_selenium_export_document($node);

  _walkhub_export_selenium_steps($dom, $node);

  $output = $dom->saveXML();
  return $output;
}

/**
 * Helper function to prepare a selenium export DOMDocument.
 *
 * @param $node
 *   Node to export.
 *
 * @return DOMDocument
 *   Prepared DOMDocument.
 */
function _walkhub_prepare_selenium_export_document($node) {
  $parameters = _walkhub_get_parameters($node);

  $implementation = new DOMImplementation();
  $dtd = $implementation->createDocumentType('html',
    '-//W3C//DTD XHTML 1.0 Strict//EN',
    'http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd');

  $dom = $implementation->createDocument('', 'html', $dtd);
  $html = $dom->childNodes->item(1);

  $head = $dom->createElement('head');
  $head->setAttribute('profile', 'http://selenium-ide.openqa.org/profiles/test-case');
  $html->appendChild($head);

  $link = $dom->createElement('link');
  $link->setAttribute('rel', 'selenium.base');
  $link->setAttribute('href', $parameters['domain']);
  $head->appendChild($link);

  $title = $dom->createElement('title', check_plain($node->title));
  $head->appendChild($title);

  $body = $dom->createElement('body');
  $html->appendChild($body);

  $table = $dom->createElement('table');
  $table->setAttribute('cellpadding', 1);
  $table->setAttribute('cellspacing', 1);
  $table->setAttribute('border', 1);
  $body->appendChild($table);

  $thead = $dom->createElement('thead');
  $table->appendChild($thead);

  $tr = $dom->createElement('tr');
  $thead->appendChild($tr);

  $td = $dom->createElement('td', check_plain($node->title));
  $td->setAttribute('rowspan', 1);
  $td->setAttribute('colspan', 3);
  $tr->appendChild($td);

  $tbody = $dom->createElement('tbody');
  $table->appendChild($tbody);

  return $dom;
}

/**
 * Helper function to export selenium steps.
 *
 * @param DOMDocument $dom
 *   Prepared DOMDocument object.
 *   @see _walkhub_prepare_selenium_export_document()
 * @param $node
 *   Node to export.
 */
function _walkhub_export_selenium_steps(DOMDocument $dom, $node) {
  $xpath = new DOMXPath($dom);
  $tbody = $xpath->query('//tbody')->item(0);
  $parameters = _walkhub_get_parameters($node);

  foreach ($node->field_fc_steps[LANGUAGE_NONE] as $step) {
    $step_item = field_collection_item_load($step['value']);
    $tr = $dom->createElement('tr');

    for ($i=1; $i<=3; $i++) {
      $field_name = "field_fc_step_command_$i";
      $value = _walkhub_replace_parameters($step_item->{$field_name}[LANGUAGE_NONE][0]['value'], $parameters);
      $td = $dom->createElement('td', $value);
      $tr->appendChild($td);
    }

    $tbody->appendChild($tr);
  }
}

/**
 * Replaces the walkthrough parameters in a string.
 *
 * @see _walkhub_get_parameters().

 * @param $value string
 *   Value to replace in.
 * @param $parameters
 *   Walkhub parameters.
 *
 * @return string
 *   Replaced parameters.
 */
function _walkhub_replace_parameters($value, $parameters) {
  $search = array_keys($parameters);
  $search = array_map(function($item) {
    return "[$item]";
  }, $search);

  return str_replace($search, array_values($parameters), $value);
}
