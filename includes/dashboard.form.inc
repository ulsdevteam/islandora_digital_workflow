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
  // Needed for collapsible fieldsets
  drupal_add_js('misc/collapse.js');
  // For some unknown reason, the admin form does not need the drupal collapse library
  // explicitly loaded like this, but it doesn't work here unless this library is included.
  drupal_add_library('system', 'drupal.collapse');

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
  $all_batches_link = l(t('All Batches'), '/islandora/islandora_digital_workflow/batches/all') .
          ($node_count ? ' <small class="small_lt_text">- ' . $node_count . ' batch' . ($node_count == 1 ? '' : 'es'). '</small>' : '');
  $links = array('batches' => array($all_batches_link));

  $stages = array('New', 'Scanned', 'Problem', 'Reviewed', 'Done');
  foreach ($stages as $stage) {
    $tid = islandora_digital_workflow_get_tid_by_name($stage, $content_types_vocab);
    $node_count = islandora_digital_workflow_get_count_of_nodes_by_stage($tid);
    $links['batches'][] = l($stage, '/islandora/islandora_digital_workflow/batches/' . $tid) .
        ($node_count ? ' <small class="small_lt_text">- ' . $node_count . ' batch' . ($node_count == 1 ? '' : 'es'). '</small>' : '');
  }

  // Now add some utility links that may be permission-based...
  if (user_access(ISLANDORA_DIGITAL_WORKFLOW_RUN_REPORTS)) {
    $links['utility'][] = l('Reports', '/islandora/islandora_digital_workflow/reports') . '<br><small class="small_lt_text">Run reports on batches and items.</small><br><br>';
  }
  if (user_access(ISLANDORA_DIGITAL_WORKFLOW_USE_UTILITIES)) {
    $links['utility'][] = l('Utilities', '/islandora/islandora_digital_workflow/process_form') . '<br><small class="small_lt_text">Perform functions on a set of batches and items.</small><br><br>';
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


  $staging_root = variable_get('islandora_digital_workflow_ingest_staging_path', '');
  $staging_folders = islandora_digital_workflow_find_folders_files($staging_root);
  if (count($staging_folders)) {
    $tables[] = theme('islandora_digital_workflow_files_table', array(
      'folders' => $staging_folders,
      'root' => $staging_root,
      'action_link' => '/islandora/islandora_digital_workflow/sync_staging_files/',
      'table_title' => 'Items moved to the "Staging" folder location',
      'table_description' => 'Please resolve by renaming or replacing the files in the folders listed below:',
    ));
  }
  // batches or items that have been moved into the "problems" folder
  $problems_root = variable_get('islandora_digital_workflow_ingest_problems_path', '');
  $problems_folders = islandora_digital_workflow_find_folders_files($problems_root);
  if (count($problems_folders)) {
    $tables[] = theme('islandora_digital_workflow_files_table', array(
      'folders' => $problems_folders,
      'root' => $problems_root,
      'action_link' => '',
      'table_title' => 'Items moved to the "Problems" folder location',
      'table_description' => 'Please resolve by renaming or replacing the files in the folders listed below:',
    ));
  }

  return $tables;
}
