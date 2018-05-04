<?php

function islandora_digital_workflow_dashboard() {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/lookup.form');
  $markup = theme('islandora_digital_workflow_lookup', array('searchterm' => ''));
  module_load_include('inc', 'islandora_digital_workflow', 'includes/access_permissions');
  $dashboard_data = islandora_digital_workflow_get_user_role_dashboard();
  return $markup . theme('islandora_digital_workflow_dashboard', array('dashboard_data' => $dashboard_data));
}

/**
 * This will set up the values for the dashboard view template.
 *
 * @return type
 */
function islandora_digital_workflow_get_user_role_dashboard() {
  // The template will use the role of the user to show specific sections.
  module_load_include('inc', 'islandora_digital_workflow', 'includes/access_permissions');

  // Add any other template variables to the array
  $template_variables = array();
  $template_variables = array_merge($template_variables);
  return $template_variables;
}
