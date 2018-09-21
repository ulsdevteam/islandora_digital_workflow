<?php

/**
 * @file
 * Unit that will create the dynamic CSS based on the configuration.
 */

/**
 * This will update the dynamically created CSS based on the color settings.
 */
function islandora_digital_workflow_update_dynamic_css() {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
  $module_path = drupal_get_path('module', 'islandora_digital_workflow');

  $all_actions = islandora_digital_workflow_get_all_actions();
  $css = theme('islandora_digital_workflow_dynamic_css', array(
      'all_actions' => $all_actions,
      'module_path' => $module_path,
  ));
  // Loop through all of the islandora_digital_workflow_actions records and
  // generate the CSS based on their values.
  $div_css_names = array();
  foreach ($all_actions as $action_arr) {
    if ($action_arr['glyph']) {
      $css .= "." . $action_arr['class_name'] . " {\n" .
              '  background: url("/' . $module_path . '/' . $action_arr['glyph'] . '") 0px 0px no-repeat !important' . ";\n" .
              '}' . "\n\n";
      $div_css_names[] = "div." . $action_arr['class_name'];
    }
  }
  $css .= "\n" . implode(", ", $div_css_names) . " {\n" .
      '  width: 20px;' . "\n" .
      '  height: 20px;' . "\n" .
      '  float: left;' . "\n" .
      '  padding: 0px 1px;' . "\n" .
      '  background-position: top left;' . "\n" .
      '}' . "\n\n";

  $path = drupal_realpath('public://');
  $bytes = file_put_contents($path . '/islandora_digital_workflow_dynamic.css', $css);
  if ($bytes) {
    drupal_set_message('Saved dynamic CSS to ' . $path . '/islandora_digital_workflow_dynamic.css');
  }
  else {
    drupal_set_message('There was a problem saving the dynamic generated CSS to ' .
        $path . '/css/islandora_digital_workflow_dynamic.css.  This is likely ' .
        'a file permissions issue.  In order to use this ability, the files folder must ' .
        'be owned by apache. <blockquote><code>$ sudo chown apache ' . $path .
        '</code></blockquote>');
  }
}
