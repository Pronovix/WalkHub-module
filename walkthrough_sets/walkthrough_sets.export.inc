<?php
/**
 * @file
 *  Export a walkthrough set to different formats.
 */

/**
 * Export a walkthrough_set node to PHPUnit test case.
 *
 * @param $node
 *  Walkthrough set node.
 *
 * @param bool $via_walkthrough
 *
 * @return string
 *  PHPUnit test case.
 */
function _walkthrough_set_get_phpunit_export_output($node, $via_walkthrough = TRUE) {
  module_load_include('inc', 'walkhub', 'walkhub.export');

  $output = "<?php\n";

  $output .= "require_once 'PHPUnit/Extensions/SeleniumTestCase.php';\n";

  $output .=  _walkhub_get_phpunit_comment($node);
  $output .= "class Example extends PHPUnit_Extensions_SeleniumTestCase {\n";
  $output .= _walkhub_get_phpunit_export_setup();
  $output .= '  public function ' . _walkhub_get_phpunit_function_name($node) . "() {\n";

  // Check that the $node has a field with field_walkthroughs machine name.
  if (isset($node->field_walkthroughs)) {
    // Load the field_walkthroughs field value
    $walkthrough_set_walkthroughs = field_get_items('node', $node, 'field_walkthroughs', LANGUAGE_NONE);

    // If it's not empty
    if (!empty($walkthrough_set_walkthroughs)) {
      // Since the field_walkthroughs field is an unlimited field_collection reference field check one by one
      foreach($walkthrough_set_walkthroughs as $item) {
        // Load the walkthrough_list field_collection entity.
        $walkthrough_list = entity_load('field_collection_item', array($item['value']));
        $walkthrough_list = reset($walkthrough_list);

        if (isset($walkthrough_list->field_walkthrough)) {
          // Load the field_walkthrough field items.
          $walkthrough_list_items = field_get_items('field_collection_item', $walkthrough_list, 'field_walkthrough');

          foreach ($walkthrough_list_items as $list_item) {
            // Load the walkthrough node and export it to PHPUnit.
            if ($walkthrough = node_load($list_item['target_id'])) {
              if ($via_walkthrough) {
                $output .= _walkhub_phpunit_export_steps_via_walkhub($walkthrough);
              }
              else {
                $output .= _walkhub_phpunit_export_steps($walkthrough);
              }
            }
          }
        }
      }
    }
  }

  $output .= "  }\n";

  $output .= "}\n";

  return $output;
}
