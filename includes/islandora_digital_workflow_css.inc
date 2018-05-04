<?php

/**
 * This will update the dynamically created CSS based on the color settings.
 */
function islandora_digital_workflow_update_dynamic_css() {
  $stage_values = json_decode(variable_get('islandora_digital_workflow_stages',
      '{"New":{"background-color":"#997","color":"#220"},"Scanned"' .
      ':{"background-color":"#472","color":"#FFF"},"Problem":{"' .
      'background-color":"#F00","color":"#FF3"},"Reviewed":{"' .
      'background-color":"#237","color":"#CCC"},"Done":{"' .
      'background-color":"#2C2","color":"#349"}}'), TRUE);

  $css = '';
  foreach ($stage_values as $stage => $colors) {
    $css .= '.stage_' . $stage . ", .stage_" . $stage . " a {\n" .
            '  color:' . $colors['color'] . ";\n" .
            '  background-color:' . $colors['background-color'] . ";\n" .
            '}' . "\n\n";
  }
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
