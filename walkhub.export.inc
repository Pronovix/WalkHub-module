<?php

define('WALKHUB_SELENIUM_EXPORT_VIA_WALKHUB', TRUE);
define('WALKHUB_SELENIUM_EXPORT_WITHOUT_WALKHUB', FALSE);

define('WALKHUB_SELENIUM_EXPORT_NEXT_BUTTON_SELECTOR', '.wtbubble-next');

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

 * @param string $value
 *   Value to replace in.
 * @param array $parameters
 *   Array of strings.
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
 * @param $walkthrough stdClass
 *   Drupal node.
 *
 * @param $via_walkhub bool
 *   True if the export is Via the walkhub, false otherwise.
 *
 * @param $class_to_extend
 *   PHPUnit class to extend.
 *   It should be a derivative class of PHPUnit_EXtensions_Selenium2TestCase,
 *   e.g.: Sauce\Sausage\WebDriverTestCase
 *
 * @return string
 *   Exported phpunit class.
 */
function _walkhub_get_phpunit_export_output(stdClass $walkthrough, $via_walkhub = WALKHUB_SELENIUM_EXPORT_VIA_WALKHUB, $class_to_extend = 'PHPUnit_Extensions_Selenium2TestCase') {
  $output = _walkhub_phpunit_header($walkthrough, $via_walkhub, $class_to_extend);
  $output .= _walkhub_get_phpunit_steps($walkthrough, $via_walkhub);
  $output .= _walkhub_phpunit_footer($walkthrough);
  return $output;
}

/**
 * @param stdClass $walkthrough
 *   Walkthrough to export the prerequisites for.
 *
 * @return string
 *   PHPUnit steps to play through the prerequisite steps.
 */
function _walkhub_phpunit_export_prerequisites(stdClass $walkthrough) {
  module_load_include('inc', 'walkthrough');

  $return = '';
  $prerequisites = _walkthrough_prerequisites_retrieve($walkthrough->uuid);
  foreach ($prerequisites as $prerequisite) {
    $prerequisite = node_load($prerequisite['nid']);
    $return .= _walkhub_phpunit_export_get_prerequisite_steps($prerequisite);
  }
  return $return;
}

/**
 * PHPUnit export for playing through the steps of a prerequisite.
 *
 * Playing through these prerequisites do not produce screenshots.
 *
 * @param $walkthrough
 *   Prerequisite walkthrough.
 *
 * @return string
 *   PHPUnit code.
 */
function _walkhub_phpunit_export_get_prerequisite_steps($walkthrough) {
  $node_wrapper = entity_metadata_wrapper('node', $walkthrough);
  $number_of_steps = count($node_wrapper->field_fc_steps) - 1;

  $output = '/** Prerequisite: ' . $walkthrough->title . " **/\n";
  $output .= _walkhub_get_phpunit_comment($walkthrough);

  $commands = _walkhub_phpunit_export_wait_for_next_button();
  $commands .= _walkhub_phpunit_export_click_next_button();
  $output .= str_repeat($commands, $number_of_steps);
  $output .= "\n";

  // Click "Next" on the dialog between the two walkthroughs.
  $output .= '$this->byCssSelector(".wtbubble-extrabutton + ' . WALKHUB_SELENIUM_EXPORT_NEXT_BUTTON_SELECTOR . '")->click();' . "\n";

  return $output;
}

/**
 * Get the screenshot creating function of the selenium export.
 *
 * @return string
 */
function _walkhub_phpunit_export_screenshot_function() {
  return '
  /**
   * Helper function to create screenshots.
   *
   * @param array $box
   *   Bounding box of the element to take screenshot of.
   *   array(
   *     x => int,
   *     y => int,
   *     width => int,
   *     height => int,
   *   )
   * @param string $filename
   *   Filename to write the screenshot to (in png format).
   */
  protected function walkthroughCreateScreenshot($box, $filename) {
  $screenshot = tempnam(sys_get_temp_dir(), "whshot");

  file_put_contents($screenshot, $this->currentScreenshot());

  $src = imagecreatefrompng($screenshot);
  $dest = imagecreatetruecolor($box["width"], $box["height"]);

  imagecopy($dest, $src, 0, 0, $box["x"], $box["y"], $box["width"], $box["height"]);
  imagepng($dest, $filename);

  unlink($screenshot);
}
';
}

function _walkhub_phpunit_header($node, $via_walkhub = WALKHUB_SELENIUM_EXPORT_VIA_WALKHUB, $class_to_extend = 'PHPUnit_Extensions_Selenium2TestCase') {
  $class_name = _walkhub_get_phpunit_class_name($node);

  $output = "<?php\n";
  $output .= "define('WT_BROWSER', getenv('WT_BROWSER') ?: 'firefox');\n";

  $output .= _walkhub_get_phpunit_comment($node);
  $output .= "class $class_name extends $class_to_extend {\n";

  if ($via_walkhub) {
    $output .= _walkhub_phpunit_export_screenshot_function();
  }

  $output .= _walkhub_get_phpunit_export_setup();
  $output .= '  public function ' . _walkhub_get_phpunit_function_name($node) . "() {\n";

  return $output;
}

function _walkhub_phpunit_footer($node) {
  $output = "  }\n";
  $output .= "}\n";
  return $output;
}

/**
 * @param $walkthrough stdClass
 *   Walkthrough node.
 * @param $via_walkhub bool
 *   If export steps via the walkhub or just as raw PHPUnit tests.
 *
 * @return string
 *   Exported steps.
 */
function _walkhub_get_phpunit_steps(stdClass $walkthrough, $via_walkhub = TRUE) {
  $output = '';

  // Resize window.
  $output .= '$this->currentWindow()->size(array("width" => $this->windowWidth, "height" => $this->windowHeight));' . "\n";


  if ($via_walkhub) {
    $output .= _walkhub_phpunit_export_steps_via_walkhub($walkthrough);
  }
  else {
    $output .= _walkhub_phpunit_export_steps_raw($walkthrough);
  }

  return $output;
}

/**
 * Export PHPunit steps from a Walkthrough node.
 *
 * @param $node
 *   Walkthrough node.
 *
 * @return string
 *   PHPunit steps.
 */
function _walkhub_phpunit_export_steps_raw($node) {
  $parameters = _walkhub_get_parameters($node);

  $output = '';
  foreach ($node->field_fc_steps[LANGUAGE_NONE] as $step) {
    $step_item = field_collection_item_load($step['value']);

    // Add comment.
    if (!empty($step_item->field_fc_step_description[LANGUAGE_NONE][0]['value'])) {
      $output .= _walkhub_php_multiline_comment($step_item->field_fc_step_description[LANGUAGE_NONE][0]['value']);
    }

    // Call _walkhub_phpuit_export_step with the parameters.
    $arguments = array();
    for ($i = 1; $i <= 3; $i++) {
      $field_name = "field_fc_step_command_$i";
      $value = _walkhub_replace_parameters($step_item->{$field_name}[LANGUAGE_NONE][0]['value'], $parameters);
      $arguments[] = $value;
    }
    $output .= call_user_func_array('_walkhub_phpunit_export_step', $arguments) . "\n\n";
  }

  return $output;
}

/**
 * Export PHPunit steps via a walkhub.
 *
 * Steps will play a walkthrough on a walkhub.
 *
 * @param object $node
 *   Walkthrough node.
 *
 * @return string
 *   PHPUnit steps.
 */
function _walkhub_phpunit_export_steps_via_walkhub($node) {
  $output = '$this->timeouts()->implicitWait(30000);' . "\n";

  // Unpublished nodes can only be accessed by their owner.
  if ($node->status == NODE_NOT_PUBLISHED) {
    $account = user_load($node->uid);
    if ($account && $account->uid > 0) {
      $one_time_login_link = user_pass_reset_url($account) . '/login';
      $output .= '$this->url("' . $one_time_login_link . '");' . "\n";
    }
  }

  $walkthrough_node_url = url('node/' . $node->nid, array(
    'absolute' => TRUE,
    'query' => array(
        'wt_full_window' => 1,
        'wt_play_prerequisites' => 1,
      ),
    )
  );
  $output .= '$this->url("' . $walkthrough_node_url . '");' . "\n";
  $output .= '$this->byPartialLinkText("Start walkthrough")->click();' . "\n";

  $output .= '$this->byCssSelector(".ui-widget button[type=\"button\"]")->click();' . "\n";

  $output .= '$frame = $this->byCssSelector("#ui-id-2");' . "\n";

  // Get iframe bounding box to take screenshot of.
  $output .= '$box = array();' . "\n";
  $output .= '$box += $frame->size();' . "\n";
  $output .= '$box += $frame->location();' . "\n";

  $output .= '$this->frame($frame);' . "\n";

  $output .= _walkhub_phpunit_export_prerequisites($node);

  $node_wrapper = entity_metadata_wrapper('node', $node);

  // Get steps.
  $steps = array();
  foreach ($node_wrapper->field_fc_steps as $step) {
    $steps[] = $step->item_id->value();
  }

  // Skip first step as it just loads the page.
  array_shift($steps);

  foreach ($steps as $step_id) {
    // Wait for popup.
    $output .= "\n\n";
    $output .= _walkhub_phpunit_export_wait_for_next_button();

    // Take screenshot.
    $screenshot_filename = $node->uuid . ':' . $step_id . '.png';
    $output .= 'if ($this->screenshotPath) {' . "\n";
    $output .= '  $this->walkthroughCreateScreenshot($box, $this->screenshotPath . "' . $screenshot_filename . '");' . "\n";
    $output .= "}\n";

    // Click Next.
    $output .= _walkhub_phpunit_export_click_next_button();
  }

  return $output;
}

/**
 * Helper function for the PHPUnit code to wait for the "Next" button to show.
 *
 * Waits 30 seconds for the button to show.
 *
 * @return string
 *   PHPUnit code.
 */
function _walkhub_phpunit_export_wait_for_next_button() {
  $output = '$tires = 0;' . "\n";
  $output .= 'while (!$this->byCssSelector("' . WALKHUB_SELENIUM_EXPORT_NEXT_BUTTON_SELECTOR .'") && $tries < 30) {$tries++; sleep(1);}' . "\n";

  // Wait for the scroll.
  $output .= "sleep(1);\n";

  return $output;
}

/**
 * Helper function for the PHPUnit code to click on the next button.
 *
 * @return string
 *   PHPUnit code.
 */
function _walkhub_phpunit_export_click_next_button() {
  return '$this->byCssSelector("' . WALKHUB_SELENIUM_EXPORT_NEXT_BUTTON_SELECTOR . '")->click();' . "\n";
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
  return 'test' . _walkhub_get_phpunit_class_name($node);
}

function _walkhub_get_phpunit_class_name($node) {
  return preg_replace('/[^a-zA-Z0-9+]/', '', ucwords($node->title));
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
 * Export setUp() selenium function and class variables.
 *
 * we do not set the baseUrl here, because step sommands are called using the
 * full url.
 *
 * @return string
 *   Selenium setup function.
 */
function _walkhub_get_phpunit_export_setup() {
  if ((bool) preg_match('/^https?:\/\/([A-Z0-9][A-Z0-9_-]*(?:\.[A-Z0-9][A-Z0-9_-]*)+):?(\d+)?\/?/i', $GLOBALS['base_url'])) {
    $base_url = $GLOBALS['base_url'];
  }
  else {
    throw new Exception(t('Invalid base url'));
  }
  return '
  protected $screenshotPath;

  protected $windowWidth;
  protected $windowHeight;

  public static $browsers = array(
    array(
      "browserName" => WT_BROWSER,
    ),
  );

  public function setUp() {
    $this->setBrowserUrl("' . $base_url .'");
    $this->screenshotPath = getenv("WT_SCREENSHOT_PATH") ?: "" ;

    $this->windowWidth = getenv("WT_WINDOW_WIDTH") ?: 1024;
    $this->windowHeight = getenv("WT_WINDOW_HEIGHT") ?: 768;
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
      $output .= ', "' . $command3 . '"';
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
  $output = '$this->click("' . $command2 . '");' . "\n";
  $output .= '$this->waitForPageToLoad("30000");';

  return $output;
}
