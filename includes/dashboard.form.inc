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
  module_load_include('inc', 'islandora_digital_workflow', 'includes/taxonomy_utilities');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/node_utilities');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  
  // first link is "All" stages
  // remaining links are all based on the stage taxonomy terms  
  $stages_vocab = taxonomy_vocabulary_machine_name_load(IDW_STAGE_TAXONOMY_MACHINE_NAME);
  $priorities_vocab = taxonomy_vocabulary_machine_name_load(IDW_BATCH_PRIORITY_TAXONOMY_MACHINE_NAME);
  $content_type_vocab = taxonomy_vocabulary_machine_name_load(IDW_CONTENT_TYPE_TAXONOMY_MACHINE_NAME);
  $accessible_models = islandora_digital_workflow_get_cmodels_having_sequences();
  $priority_terms = islandora_digital_workflow_sort_array_objects_by_weight(islandora_digital_workflow_get_all_terms_of_vocab($priorities_vocab));
  $stages_terms = islandora_digital_workflow_sort_array_objects_by_weight(islandora_digital_workflow_get_all_terms_of_vocab($stages_vocab));

  $node_count = islandora_digital_workflow_get_count_of_nodes_by_tid();
  $all_batches_link = l(t('All Batches'), '/islandora/islandora_digital_workflow/batches/all') .
          ($node_count ? ' <span class="small_lt_text">- ' . $node_count . ' batch' . ($node_count == 1 ? '' : 'es'). '</span>' : '');
  $links = array('batch_stages' => array($all_batches_link));

  foreach ($stages_terms as $tid => $stage_obj) {
    $node_count = islandora_digital_workflow_get_count_of_nodes_by_tid('field_stage', $tid);
    $link_att = array('attributes' => array('title' => $stage_obj->description));
    $links['batch_stages'][] = l($stage_obj->name, '/islandora/islandora_digital_workflow/batches/' . $tid, $link_att) .
        ($node_count ? ' <span class="small_lt_text">- ' . $node_count . ' batch' . ($node_count == 1 ? '' : 'es'). '</span>' : '');
  }

  $priorities = array('Highest', 'High', 'Regular', 'Low', 'Lowest');
  foreach ($priority_terms as $tid => $priority_obj) {
    $node_count = islandora_digital_workflow_get_count_of_nodes_by_tid('field_priority', $tid);
    $links['batch_priorities'][] = l($priority_obj->name, 'taxonomy/term/' . $tid) .
        ($node_count ? ' <span class="small_lt_text">- ' . $node_count . ' batch' . ($node_count == 1 ? '' : 'es'). '</span>' : '');
  }

  foreach ($accessible_models as $accessible_model => $pid_label) {
    $tid = islandora_digital_workflow_get_tid_by_name($accessible_model, $content_type_vocab);
    $node_count = islandora_digital_workflow_get_count_of_nodes_by_tid('field_islandora_model', $tid);
    $links['batch_content_types'][] = l($pid_label['label'], 'taxonomy/term/' . $tid) .
        ($node_count ? ' <span class="small_lt_text">- ' . $node_count . ' batch' . ($node_count == 1 ? '' : 'es'). '</span>' : '');
  }

  // Now add some utility links that may be permission-based...
  if (user_access(ISLANDORA_DIGITAL_WORKFLOW_RUN_REPORTS)) {
    $links['report'][] = l('Reports', '/islandora/islandora_digital_workflow/reports') .
        '<br><span class="small_lt_text">Run reports on batches and items.</span>' .
        (user_access('administer forena reports') ? '<br>' .
           l(t('Administer reports'), '/admin/structure/forena') .
            '<br><span class="small_lt_text">Forena reports administration</span>' : '') . '<br><br>';
  }
  if (user_access(ISLANDORA_DIGITAL_WORKFLOW_USE_UTILITIES)) {
    $links['utility'][] = l('Utilities', '/islandora/islandora_digital_workflow/utilities') .
        '<br><span class="small_lt_text">Perform functions on a set of batches ' .
        'and items.</span><br><br>';
  }
  if (user_access(ISLANDORA_DIGITAL_WORKFLOW_MANAGE)) {
    $links['admin'][] = l('Islandora Digital Workflow Settings', '/admin/islandora/islandora_digital_workflow') .
        '<br><span class="small_lt_text">Configure options and settings for ' .
        'the Islandora Digital Workflow module.</span><br><br>';
  }
  if (user_access(ISLANDORA_DIGITAL_WORKFLOW_MANAGE_SEQUENCES)) {
    $links['admin'][] = l('Workflow Actions', '/admin/islandora/islandora_digital_workflow/workflow_sequence_actions') .
        '<br><span class="small_lt_text">Configure the specific actions that can ' .
        'be used as steps in a workflow sequence.</span><br><br>';
    $links['admin'][] = l('Workflow Sequences', '/admin/islandora/islandora_digital_workflow/workflow_sequences') .
        '<br><span class="small_lt_text">Configure the actions required and ' .
        'the model-associations for various workflow sequences.</span><br><br>';
  }
  if (user_access('administer permissions')) {
    $links['admin'][] = l(t('User Permissions'), '/admin/people/permissions') .
        '<br><span class="small_lt_text">Manage user permissions.</span><br><br>';
  }
  
  return $links;  
}

/**
 * This simply returns the "action_links" for the dashboard page.
 *
 * @return array
 *   Each item is a rendered link.
 */
function islandora_digital_workflow_get_dashboard_action_links() {
  $links = array();

  // First link should be to the "Create Batch" form.
  if (user_access(ISLANDORA_DIGITAL_WORKFLOW_CREATE_NEW_BATCH)) {
    $links[] = l('Create Batch', '/islandora/islandora_digital_workflow/create_batch');
  }

  if (module_exists('islandora_digitization_requests')) {
    // Look up whether or not there are any "Digitization Request" Webforms.
    // and create an entry in $links[].
    if (user_access(ISLANDORA_DIGITAL_WORKFLOW_CREATE_DIGITIZATION_REQUEST)) {
      // There could be more than one type of "Digitization request" WebForm
       $links[] = l('Submit a Digitization Request', '/islandora/islandora_digital_workflow/create_digitization_request');
    }
  }

  // Add a link if the user has permission to create collections.
  if (user_access('create child collection')) {
    $top_level_collection_uri = rtrim(variable_get('islandora_digital_workflow_top_level_collection', '/islandora/object/islandora:root'), "/");
    $links[] = l('Create Collection', $top_level_collection_uri . '/manage/overview/ingest');
  }
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
  // Runs a query to get any batches that have no items yet.
  if (user_access(ISLANDORA_DIGITAL_WORKFLOW_UPDATE_ITEMS)) {
    $batch_records_no_items = islandora_digital_workflow_find_batch_records_no_items();
    if (count($batch_records_no_items)) {
      $tables[] = theme('islandora_digital_workflow_batch_records_table', array(
        'batch_records' => $batch_records_no_items,
        'table_title' => 'Batch records with no items',
        'table_description' => 'Please complete or delete the following batch records:',
      ));
    }
  }

  // View batches that report a different number of items than "How many physical objects".
  if (user_access(ISLANDORA_DIGITAL_WORKFLOW_UPDATE_ITEMS)) {
    $batch_records_mismatch = islandora_digital_workflow_find_batch_items_count_mismatch();
    if (count($batch_records_mismatch)) {
      $tables[] = theme('islandora_digital_workflow_batch_records_table', array(
        'batch_records' => $batch_records_mismatch,
        'table_title' => 'Number of items records does not equal the intended "How many physical objects"',
        'table_description' => 'Please resolve the number of objects or items for the following batches:',
      ));
    }
  }

  // View any batches that have "Request by" date values
  if (user_access(ISLANDORA_DIGITAL_WORKFLOW_UPDATE_ITEMS)) {
    $batch_records_is_batch_request = islandora_digital_workflow_find_batch_records_is_batch_request();
    if (count($batch_records_is_batch_request)) {
      $tables[] = theme('islandora_digital_workflow_batch_records_table', array(
        'batch_records' => $batch_records_is_batch_request,
        'table_title' => 'Requested Batches',
        'table_description' => 'The following batches all have been requested.',
      ));
    }
  }

  // View Problem and Staging files related to batches
  if (user_access(ISLANDORA_DIGITAL_WORKFLOW_DASHBOARD_VIEW_PROBLEMS)) {
    $batch_names = islandora_digital_workflow_get_batch_record_names();
    $delivery_root = variable_get('islandora_digital_workflow_ingest_delivery_path', '');
    $delivery_folders = islandora_digital_workflow_find_folders_files($delivery_root);
    $bad_delivery_folder_names = islandora_digital_workflow_filter_out_non_batch_folders($delivery_folders, $batch_names);
    if (count($delivery_folders)) {
      $tables[] = theme('islandora_digital_workflow_files_table', array(
        'folders' => $delivery_folders,
        'root' => $delivery_root,
        'action_link' => (user_access(ISLANDORA_DIGITAL_WORKFLOW_SYNC_DELIVERY) ? '/islandora/islandora_digital_workflow/sync_delivery_files/' : ''),
        'show_message_status' => TRUE,
        'table_title' => 'Items moved to the "Delivery" folder location',
        'table_description' => 'Resolved by using the Sync operation for each of ' .
            'the folders listed below, but <b>be sure that the folder is ready to be ' .
            'moved back</b> over to the working directory.',
      ));
    }

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
          'the repaired files in the <b>Incoming Delivery Directory</b> location;  ' .
          'these files can then be imported back into the system which will resolve ' .
          'the specific item\'s problem record.</p><p><em>Mouse-over any of the ' .
          '"problem_notes.txt" to see what needs to be done to resolve each ' .
          'problem.</em><br>Please resolve the folders listed below:</p>',
      ));
    }

    if (count($bad_delivery_folder_names) > 0) {
      drupal_set_message(t('At least one folder name in the configured Delivery path "' .
          $delivery_root . '" did not match a known batch name and should be ' .
          'inspected manually to determine whether or not the files are needed.  ' .
          'These folder names were: ') . '<ul><li>' .
          implode("</li><li>", $bad_delivery_folder_names) . '</ul><br>', 'warning');
    }
    if (count($bad_problem_folder_names) > 0) {
      drupal_set_message(t('At least one folder name in the configured Problems path "' .
          $problems_root . '" did not match a known batch name and should be ' .
          'inspected manually to determine whether or not the files are needed.  ' .
          'These folder names were: ') . '<ul><li>' .
          implode("</li><li>", $bad_problem_folder_names) . '</ul><br>', 'warning');
    }
  }
  return $tables;
}
