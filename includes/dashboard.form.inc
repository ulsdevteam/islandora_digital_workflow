<?php

/**
 * @file
 * Form to display the user dashboard with links to what the user can do.
 */

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

  $dashboard_data = array();
  $tasks_tables = islandora_digital_workflow_get_tasks_tables();
  $links = islandora_digital_workflow_get_dashboard_links();
  $action_links = islandora_digital_workflow_get_dashboard_action_links();

  return $lookup_form_markup .
      theme('islandora_digital_workflow_dashboard', array(
          'dashboard_data' => $dashboard_data,
          'links' => $links,
          'action_links' => $action_links)) .
      theme('islandora_digital_workflow_dashboard_tasks', array(
          'tasks_tables' => $tasks_tables));
}

/**
 * Gets links that should appear on the dashboard -- including:
 *   - links the Drupal view for each of the various stages.
 *   - Reports
 *   - Utilities
 *   - Administration
 *
 * @return array
 *   An array to the links to Drupal views.
 */
function islandora_digital_workflow_get_dashboard_links() {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/taxonomy-utilities');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/node-utilities');
  
  // first link is "All" stages
  // remaining links are all based on the stage taxonomy terms  
  $content_types_vocab = taxonomy_vocabulary_machine_name_load(IDW_STAGE_TAXONOMY_MACHINE_NAME);
  $node_count = islandora_digital_workflow_get_count_of_nodes_by_stage();
  $all_batches_link = l(t('All Batches'), '/islandora/islandora_digital_workflow/batches/all') .
          ($node_count ? ' <span class="small_lt_text">- ' . $node_count . ' batch' . ($node_count == 1 ? '' : 'es'). '</span>' : '');
  $links = array('batches' => array($all_batches_link));

  $stages = array('New', 'Scanned', 'Problem', 'Reviewed', 'Done');
  foreach ($stages as $stage) {
    $tid = islandora_digital_workflow_get_tid_by_name($stage, $content_types_vocab);
    $node_count = islandora_digital_workflow_get_count_of_nodes_by_stage($tid);
    $links['batches'][] = l($stage, '/islandora/islandora_digital_workflow/batches/' . $tid) .
        ($node_count ? ' <span class="small_lt_text">- ' . $node_count . ' batch' . ($node_count == 1 ? '' : 'es'). '</span>' : '');
  }

  // Now add some utility links that may be permission-based...
  if (user_access(ISLANDORA_DIGITAL_WORKFLOW_RUN_REPORTS)) {
    $links['report'][] = l('Reports', '/islandora/islandora_digital_workflow/reports') . '<br><span class="small_lt_text">Run reports on batches and items.</span><br><br>';
  }
  if (user_access(ISLANDORA_DIGITAL_WORKFLOW_USE_UTILITIES)) {
    $links['utility'][] = l('Utilities', '/islandora/islandora_digital_workflow/utilities') . '<br><span class="small_lt_text">Perform functions on a set of batches and items.</span><br><br>';
  }
  if (user_access(ISLANDORA_DIGITAL_WORKFLOW_MANAGE)) {
    $links['admin'][] = l('Islandora Digital Workflow Settings', '/admin/islandora/islandora_digital_workflow') . '<br><span class="small_lt_text">Configure options and settings for the Islandora Digital Workflow module.</span><br><br>';
  }
  if (user_access('administer permissions')) {
    $links['admin'][] = l(t('User Permissions'), '/admin/people/permissions') . '<br><span class="small_lt_text">Manage user permissions.</span><br><br>';
  }
  
  return $links;  
}

function islandora_digital_workflow_get_dashboard_action_links() {
  $links = array();

  if (user_access('create child collection')) {
    $top_level_collection_uri = rtrim(variable_get('islandora_digital_workflow_top_level_collection', '/islandora/object/islandora:root'), "/");
    $links[] = l('Create Collection', $top_level_collection_uri . '/manage/overview/ingest');
  }
  $links[] = l('Create Batch', '/islandora/islandora_digital_workflow/create_batch');
  return $links;
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

  $batch_records_no_items = islandora_digital_workflow_find_batch_records_no_items();
  if (count($batch_records_no_items)) {
    $tables[] = theme('islandora_digital_workflow_batch_records_table', array(
      'batch_records' => $batch_records_no_items,
      'table_title' => 'Batch records with no items',
      'table_description' => 'Please complete or delete the following batch records:',
    ));
  }

  $batch_names = islandora_digital_workflow_get_batch_record_names();
  $staging_root = variable_get('islandora_digital_workflow_ingest_staging_path', '');
  $staging_folders = islandora_digital_workflow_find_folders_files($staging_root);
  $bad_staging_folder_names = islandora_digital_workflow_filter_out_non_batch_folders($staging_folders, $batch_names);

  if (count($staging_folders)) {
    $tables[] = theme('islandora_digital_workflow_files_table', array(
      'folders' => $staging_folders,
      'root' => $staging_root,
      'action_link' => (user_access(ISLANDORA_DIGITAL_WORKFLOW_SYNC_STAGING) ? '/islandora/islandora_digital_workflow/sync_staging_files/' : ''),
      'show_message_status' => TRUE,
      'table_title' => 'Items moved to the "Staging" folder location',
      'table_description' => 'Resolved by using the Sync operation for each of ' .
          'the folders listed below, but <b>be sure that the folder is ready to be ' .
          'moved back</b> over to the working directory.',
    ));
  }
  
  // $batch_host_folders = islandora_digital_workflow_batch_path
  // batches or items that have been moved into the "problems" folder
  $problems_root = variable_get('islandora_digital_workflow_ingest_problems_path', '');
  $problems_folders = islandora_digital_workflow_find_folders_files($problems_root);
  $bad_problem_folder_names = islandora_digital_workflow_filter_out_non_batch_folders($problems_folders, $batch_names);
  if (count($problems_folders)) {
    $tables[] = theme('islandora_digital_workflow_files_table', array(
      'folders' => $problems_folders,
      'root' => $problems_root,
      'action_link' => '',
      'show_message_status' => TRUE,
      'table_title' => 'Items moved to the "Problems" folder location',
      'table_description' => '<p class="small_font">Aside from needing to satisfy ' .
        'all of the actions in the workflow sequence, problems must be cleared ' .
        'before the item or the batch can be ingested.  To clear a problem, place ' .
        'the repaired files in the <b>Incoming Staging Directory</b> location;  ' .
        'these files can then be imported back into the system which will resolve ' .
        'the specific item\'s problem record.</p>Please resolve the folders listed below:',
    ));
  }

  if (count($bad_staging_folder_names) > 0) {
    drupal_set_message('At least one folder name in the configured Staging path "' .
        $staging_root . '" did not match a known batch name and should be ' .
        'inspected manually to determine whether or not the files are needed.  ' .
        'These folder names were: <ul><li>' .
        implode("</li><li>", $bad_staging_folder_names) . '</ul><br>', 'warning');
  }
  if (count($bad_problem_folder_names) > 0) {
    drupal_set_message('At least one folder name in the configured Problems path "' .
        $problems_root . '" did not match a known batch name and should be ' .
        'inspected manually to determine whether or not the files are needed.  ' .
        'These folder names were: <ul><li>' .
        implode("</li><li>", $bad_problem_folder_names) . '</ul><br>', 'warning');
  }
  return $tables;
}
