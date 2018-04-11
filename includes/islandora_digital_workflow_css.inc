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
    $css .= '.stage_' . $stage . " {\n" .
            '  color:' . $colors['color'] . ";\n" .
            '  background-color:' . $colors['background-color'] . ";\n" .
            '}' . "\n\n";
  }
  $path = drupal_get_path('module', 'islandora_digital_workflow');

  $bytes = file_put_contents($path . '/css/islandora_digital_workflow_dynamic.css', $css);
  if ($bytes) {
    drupal_set_message('Saved dynamic CSS to ' . $path . '/css/islandora_digital_workflow_dynamic.css');
  }
  else {
    drupal_set_message('There was a problem saving the dynamic generated CSS to ' .
        $path . '/css/islandora_digital_workflow_dynamic.css.  This is likely ' .
        'a file permissions issue.  In order to use this ability, the file must ' .
        'be owned by apache. <blockquote><code>$ cd ' . $path . '/css' . "<br>" .
        '$ sudo chown apache islandora_digital_workflow_dynamic.css</code></blockquote>');
  }
}
