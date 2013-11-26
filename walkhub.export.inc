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

/**
 * Page callback for phpUnit export functionality.
 *
 * @param $node stdClass
 *   Drupal node.
 */
function walkhub_phpunit_export($node) {
  $output = _walkhub_get_phpunit_export_output($node);
  echo $output;

  module_invoke_all('exit');
  exit();
}

/**
 * Export callback for phpUnit.
 *
 * @param $node stdclass.
 *   Drupal node.
 *
 * @return string
 *   Exported phpunit class.
 */
function _walkhub_get_phpunit_export_output($node) {
  $parameters = _walkhub_get_parameters($node);

  $output = "<?php\n";

  $output .=  _walkhub_get_phpunit_comment($node);
  $output .= "class Example extends PHPUnit_Extensions_SeleniumTestCase {\n";
  $output .= _walkhub_get_phpunit_export_setup();
  $output .= '  public function ' . _walkhub_get_phpunit_function_name($node) . "() {\n";

  foreach ($node->field_fc_steps[LANGUAGE_NONE] as $step) {
    $step_item = field_collection_item_load($step['value']);

    // Add comment.
    if (!empty($step_item->field_fc_step_description[LANGUAGE_NONE][0]['value'])) {
      $output .= _walkhub_php_multiline_comment($step_item->field_fc_step_description[LANGUAGE_NONE][0]['value']);
    }

    // Call _walkhub_phpuit_export_step with the parameters.
    $arguments = array();
    for ($i=1; $i<=3; $i++) {
      $field_name = "field_fc_step_command_$i";
      $value = _walkhub_replace_parameters($step_item->{$field_name}[LANGUAGE_NONE][0]['value'], $parameters);
      $arguments[] = $value;
    }
    $output .= call_user_func_array('_walkhub_phpunit_export_step', $arguments) . "\n\n";
  }

  $output .= "  }\n";

  $output .= "}\n";
  return $output;
}

/**
 * Gets a function name from a node title.
 *
 * @param $node
 *   Drupal node.
 *
 * @return string
 *   PHP compatible function name.
 */
function _walkhub_get_phpunit_function_name($node) {
  return 'test' . preg_replace('/[^a-zA-Z0-9+]/', '', ucwords($node->title));
}

/**
 * Get comment header for the node.
 *
 * @param $node
 *   Drupal node.
 *
 * @return string
 *   Comment header for the test.
 */
function _walkhub_get_phpunit_comment($node) {
  return _walkhub_php_multiline_comment($node->body[LANGUAGE_NONE][0]['value']);
}

/**
 * Helper function to wrap a string into a php comment block.
 *
 * @param $string
 *   Any string (even html.
 *
 * @return string
 *   Valid php comment string.
 */
function _walkhub_php_multiline_comment($string) {
  $output = "/**\n";

  // Strip html tags.
  $comment = strip_tags($string);
  // Remove stars, so the comment tag cannot be broken.
  $comment = str_replace('*', '', $comment);
  // Wrap to 80 lines.
  $comment = wordwrap($comment, 80);
  // Add * in the beginning of every line.
  $comment = preg_replace('/^/m', ' * ', $comment);

  $output .= $comment;
  $output .= "\n */\n";
  return $output;
}

/**
 * Export setUp() selenium function.
 *
 * we do not set the baseUrl here, because step sommands are called using the
 * full url.
 *
 * @return string
 *   Selenium setup function.
 */
function _walkhub_get_phpunit_export_setup() {
  return '
  protected function setUp() {
    $this->setBrowser("*chrome");
  }' . "\n\n";
}

/**
 * Exports a phpunit step.
 *
 * @param $command1
 *   Selenium command 1: Action.
 * @param $command2
 *   Selenium command 2: Parameter (selector).
 * @param $command3
 *   (optional) Selenium command 3: Parameter.
 *
 * @return string
 *   Phpunit exported step.
 */
function _walkhub_phpunit_export_step($command1, $command2, $command3) {
  $output = '';

  // Check if the selenium and the phpunit commands doesn't match and we have a
  // special export function for it.
  $function_candidate = '_walkhub_phpunit_export_step_' . strtolower($command1);
  if (function_exists($function_candidate)) {
    $output .= call_user_func_array($function_candidate, func_get_args());
  }
  else {
    // Otherwise export $this->command1(command2, command3); .
    $output .= '$this->';
    $output .= $command1 . '("' . $command2 . '"';
    if ($command3) {
      $output .= ', "'. $command3 .'"';
    }
    $output .= ');';
  }

  return $output;

}

/**
 * Phpunit step export callback for the clickAndWait() function.
 *
 * @see _walkhub_phpunit_export_step()
 */
function _walkhub_phpunit_export_step_clickandwait($command1, $command2, $command3) {
  $output =  '$this->click("' . $command2 . '");' . "\n";
  $output .= '$this->waitForPageToLoad("30000");';

  return $output;
}
