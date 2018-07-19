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
  $tasks_tables = islandora_digital_workflow_get_tasks_tables();
  $links = islandora_digital_workflow_get_stage_links();
  return $lookup_form_markup .
      theme('islandora_digital_workflow_dashboard', array(
          'dashboard_data' => $dashboard_data,
          'links' => $links)) .
      theme('islandora_digital_workflow_dashboard_tasks', array(
          'tasks_tables' => $tasks_tables));
}

/**
 * Gets links to the Drupal view for each of the various stages.
 *
 * @return array
 *   An array to the links to Drupal views.
 */
function islandora_digital_workflow_get_stage_links() {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/taxonomy-utilities');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/node-utilities');
  
  // first link is "All" stages
  // remaining links are all based on the stage taxonomy terms  
  $content_types_vocab = taxonomy_vocabulary_machine_name_load(IDW_STAGE_TAXONOMY_MACHINE_NAME);
  $node_count = islandora_digital_workflow_get_count_of_nodes_by_stage();
  $all_batches_link = l('All Batches', '/islandora/islandora_digital_workflow/batches/all') . 
          ($node_count ? ' <small class="disabled_text">- ' . $node_count . ' batch' . ($node_count == 1 ? '' : 'es'). '</small>' : '');
  $links = array($all_batches_link);

  $stages = array('New', 'Scanned', 'Problem', 'Reviewed', 'Done');
  foreach ($stages as $stage) {
    $tid = islandora_digital_workflow_get_tid_by_name($stage, $content_types_vocab);
    $node_count = islandora_digital_workflow_get_count_of_nodes_by_stage($tid);
    $links[] = l($stage, '/islandora/islandora_digital_workflow/batches/' . $tid) .
        ($node_count ? ' <small class="disabled_text">- ' . $node_count . ' batch' . ($node_count == 1 ? '' : 'es'). '</small>' : '');
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

  // TODO:  display a table of batches that need other details like missing
  // workflow_sequence_id values (the value would be NULL -- which is possible
  // when deleting a sequence).
  $batch_records_no_workflow_sequence = islandora_digital_workflow_find_batch_records_no_workflow_sequence();
  if (count($batch_records_no_workflow_sequence)) {
    $tables[] = theme('islandora_digital_workflow_batch_records_table', array(
      'batch_records' => $batch_records_no_workflow_sequence,
      'table_title' => 'Batch records with no workflow sequence',
      'table_description' => 'Please edit the following batch records to associate with a workflow sequence:',
    ));
  }
  // make links to edit those batches like: islandora/islandora_digital_workflow/edit_batch/test12

  return $tables;
}
