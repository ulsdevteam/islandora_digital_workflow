<?php

function islandora_digital_workflow_dashboard() {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/lookup.form');
  $markup = theme('islandora_digital_workflow_lookup', array('searchterm' => ''));
  module_load_include('inc', 'islandora_digital_workflow', 'includes/access_permissions');
  $dashboard_data = islandora_digital_workflow_get_user_role_dashboard();
  $tasks_table = islandora_digital_workflow_get_tasks_tables();
  return $markup . $tasks_table . theme('islandora_digital_workflow_dashboard', array('dashboard_data' => $dashboard_data));
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

function islandora_digital_workflow_get_tasks_tables() {
  $tables = array();
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  // runs a query to get any batches that have been created, but the details
  // have not been entered.
  $batch_records_no_details = islandora_digital_workflow_find_batch_records_no_details();
  if (count($batch_records_no_details)) {
    dpm($batch_records_no_details);
    $tables[] = theme('islandora_digital_workflow_batch_records_table', array(
      'batch_records' => $batch_records_no_details,
      'table_title' => 'Batch records with no details',
      'table_description' => 'Please complete or delete the following batch records.',
    ));
  }
  // make links to edit those batches like: islandora/islandora_digital_workflow/edit_batch/test12

  return implode("\n", $tables);
}

