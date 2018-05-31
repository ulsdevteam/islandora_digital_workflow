<?php

/**
 * Get the dashboard output.
 *
 * @return string
 *   Markup that represents the digital workflow dashboard for the user.
 */
function islandora_digital_workflow_dashboard() {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/lookup.form');
  $lookup_form_markup = theme('islandora_digital_workflow_lookup', array('searchterm' => ''));
  module_load_include('inc', 'islandora_digital_workflow', 'includes/access_permissions');
  $dashboard_data = islandora_digital_workflow_get_user_role_dashboard();
  $tasks_table = islandora_digital_workflow_get_tasks_tables();
  $links = islandora_digital_workflow_get_stage_links();
  return $lookup_form_markup .
      theme('islandora_digital_workflow_dashboard', array(
          'dashboard_data' => $dashboard_data,
          'links' => $links)) .
      $tasks_table;
}

/**
 * Gets links to the Drupal view for each of the various stages.
 *
 * @return array
 *   An array to the links to Drupal views.
 */
function islandora_digital_workflow_get_stage_links() {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/taxonomy-utilities');
  // first link is "All" stages
  // remaining links are all based on the stage taxonomy terms  
  $content_types_vocab = taxonomy_vocabulary_machine_name_load(IDW_STAGE_TAXONOMY_MACHINE_NAME);
  $links = array(l('All Batches', '/islandora/islandora_digital_workflow/batches/all'));
  $stages = array('New', 'Scanned', 'Problem', 'Reviewed', 'Done');
  foreach ($stages as $stage) {
    $tid = islandora_digital_workflow_get_tid_by_name($stage, $content_types_vocab);
    $links[] = l($stage, '/islandora/islandora_digital_workflow/batches/' . $tid);
  }
  return $links;  
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

/**
 * Gets markup relative to the tasks that a given user can perform to be
 * displayed on their dashboard.
 *
 * @return string
 *   Markup that represents batch details / links to objects relevant to the
 * dashboard.
 */
function islandora_digital_workflow_get_tasks_tables() {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');

  $tables = array();
  // runs a query to get any batches that have been created, but the details
  // have not been entered.
  $batch_records_no_details = islandora_digital_workflow_find_batch_records_no_details();
  if (count($batch_records_no_details)) {
    $tables[] = theme('islandora_digital_workflow_batch_records_table', array(
      'batch_records' => $batch_records_no_details,
      'table_title' => 'Batch records with no details',
      'table_description' => 'Please complete or delete the following batch records:',
    ));
  }
  // make links to edit those batches like: islandora/islandora_digital_workflow/edit_batch/test12

  return implode("\n", $tables);
}

