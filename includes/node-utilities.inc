<?php

/**
 * @file
 * Set of functions related to node operations related to batch and batch_item 
 * records.
 */

/**
 * This will update the batch_field_defaults field for the workflow node object
 * based on the underlying batch record for display purposes only.
 *
 * If this is called with an existing $node, it will be updated -- else a new
 * node object is created with type="workflow_batch" and field_batch_name to
 * match the form value for "batch name".
 *
 * @param object $node
 *   Standard drupal node object.
 */
function islandora_digital_workflow_sync_node_batch_record_info($node, $form_state, $csv_filename = '', $update_node = TRUE) {
  global $system_creating_nodes;
  $system_creating_nodes = TRUE;
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');

  if (!$csv_filename && (isset($_SESSION['uploaded_csv_filename']))) {
    $csv_filename = $_SESSION['uploaded_csv_filename'];
  }
  $batch_name = (is_object($node)) ? $node->title :
      $form_state['values']['batch_name'];
  $batch_record = islandora_digital_workflow_get_batch_record_by_name($batch_name);
  $node = (is_object($node)) ? $node : (($batch_record['nid'] > 0) ? node_load($batch_record['nid']) : NULL);
  if ($csv_filename) {
    // parse the csv file to get the identifiers and set pass these values
    // along to be added to the node->field_identifiers value
    $csv_array = islandora_digital_workflow_csv_file_as_array($csv_filename);
  }
  else {
    $csv_array = array();
  }
  if ($update_node) {
    $node = islandora_digital_workflow_make_or_update_node($form_state, $csv_array, $node);

    $batch_record = islandora_digital_workflow_create_batch_record_by_form($form_state, $node, $csv_filename);
    // Load the batch record related to the node's title value which should
    // be synced with the batch record's batch_name field.
    $node = (is_object($node)) ? $node : (($batch_record['nid'] > 0) ? node_load($batch_record['nid']) : NULL);
    // Sync the CCK field for Batch Defaults with the latest values
    if (is_array($batch_record) && is_object($node)) {
      $new_value = theme('islandora_digital_workflow_batch_defaults', array('islandora_digital_workflow_batch' => $batch_record));
      $node->field_batch_defaults[LANGUAGE_NONE][0]['value'] = $new_value;
      // Make sure that the format is Full HTML here so it doesn't strip any HTML tags.
      $node->field_batch_defaults[LANGUAGE_NONE][0]['format'] = 'full_html';
      field_attach_update('node', $node);
    }
    if (isset($_SESSION['uploaded_csv_filename'])) {
      @unlink($_SESSION['uploaded_csv_filename']);
      unset($_SESSION['uploaded_csv_filename']);
    }
    if (file_exists($csv_filename)) {
      @unlink($csv_filename);
    }
  }
  return $batch_record;
}

/**
 * This will set the stage (field_stage) for the related node.
 *
 * @param integer $nid
 *   nid value for a Drupal node object.
 * @param string $state_name
 *   The name of the state value like "New", "Problem"
 */
function islandora_digital_workflow_set_node_stage($nid, $state_name = '', $stage_tid = FALSE) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/taxonomy-utilities');
  $node = node_load($nid);
  if (is_object($node)) {
    $stages_vocab = taxonomy_vocabulary_machine_name_load(IDW_STAGE_TAXONOMY_MACHINE_NAME);
    if (!$stage_tid) {
      $stage_tid = islandora_digital_workflow_get_tid_by_name($state_name, $stages_vocab);
    }
    if ($stage_tid) {
      $node->field_stage[$node->language][0] = array('tid' => $stage_tid);
      node_save($node);
    }
  }
}

/**
 * Loads the batch record that corresponds to a given drupal $node.
 *
 * @param object $node
 *   Standard drupal node object.
 * @return mixed
 *   either an associative array of batch record values or NULL
 */
function islandora_digital_workflow_load_batch_record_for_node($node, $form_state = array()) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
  $batch_name = @$node->field_batch_name[$node->language][0]['value'];
  return islandora_digital_workflow_get_batch_record_by_name($batch_name);
}

/**
 * This should be able to update an existing node or create a new node and populate
 * the CCK fields with the form_state and csv_array values.
 *
 * @param array $user
 *   A Drupal user.
 * @param array $form_state
 *   An array representing a Drupal form.
 * @param array $csv_array
 *   An array of CSV values extracted from the CSV uploaded file.
 * @param object $node
 *   Drupal node.
 * @return object
 *   Drupal node.
 */
function islandora_digital_workflow_make_or_update_node($form_state, $csv_array, $node = NULL) {
  global $user;
  module_load_include('inc', 'islandora_digital_workflow', 'includes/taxonomy-utilities');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');

  if (is_null($node)) {
    $node = new stdClass();
    $node->title = $form_state['values']['batch_name'];
    $node->type = "workflow_batch";
    // Sets some defaults. Invokes hook_prepare() and hook_node_prepare().
    node_object_prepare($node);
    // Or e.g. 'en' if locale is enabled.
    $node->language = LANGUAGE_NONE;
    $node->uid = $user->uid;
    // Status is 1 or 0; published or not.
    $node->status = 1;
    // Promote is 1 or 0; promoted to front page or not.
    $node->promote = 0;
    // Comment is 0, 1, 2; 0 = disabled, 1 = read only, or 2 = read/write.
    $node->comment = 0;

    // Set any specific CCK field values for the node object
    $node->field_batch_name[$node->language][0]['value'] = $form_state['values']['batch_name'];
    $node->field_content_type[$node->language][] = array('tid' => $form_state['values']['cmodel_select']);
  }
  
  $stages_vocab = taxonomy_vocabulary_machine_name_load(IDW_STAGE_TAXONOMY_MACHINE_NAME);
  $stage_new_tid = islandora_digital_workflow_get_tid_by_name('New', $stages_vocab);

  $node_has_new_stage_already = FALSE;
  $node_has_new_stage_already = (isset($node->field_stage) && (array_key_exists($node->language, $node->field_stage) &&
      array_key_exists(0, $node->field_stage[$node->language]) &&
      array_key_exists('tid', $node->field_stage[$node->language][0]) &&
      $node->field_stage[$node->language][0]['tid'] == $stage_new_tid)) ? TRUE : FALSE;
  if ($stage_new_tid && !$node_has_new_stage_already) {
    $node->field_stage[$node->language][] = array('tid' => $stage_new_tid);
  }

  if (array_key_exists('values', $form_state)) {
    $content_types_vocab = taxonomy_vocabulary_machine_name_load(IDW_CONTENT_TYPE_TAXONOMY_MACHINE_NAME);
    // For the Content Types cck field, the batch_record value is storing the term value
    $node->field_islandora_model[$node->language][0]['tid'] = $form_state['values']['cmodel_select'];

    // For the batch_priorities_tid cck field, the batch_record value has tid value
    $node->field_priority[$node->language][]['tid'] = $form_state['values']['batch_priority'];

    $node->field_pid_count[$node->language][]['value'] = $form_state['values']['object_count'];
    foreach ($form_state['values']['collections_select'] as $collection_pid) {
      $node->field_collections_mapping[$node->language][]['value'] = $collection_pid;
    }
    if (variable_get('islandora_digital_workflow_use_isMemberOfSite', 0)) {
      foreach ($form_state['values']['sites_select'] as $site_pid) {
        $node->field_sites_mapping[$node->language][]['value'] = $site_pid;
      }
    }
  }
  // Since islandora_digital_workflow_csv_file_as_array returned all headings in
  // lowercase, look for "identifier"
  $csv_values_arr = islandora_digital_workflow_get_csv_values_for_fieldname('identifier', $csv_array);
  if (array_key_exists('identifier', $csv_array) && (count($csv_array) > 0)) {
    $node->field_identifiers[$node->language][0]['value'] = implode("\n", $csv_values_arr['identifier']);
  }
  else {
    $node->field_identifiers[$node->language][0]['value'] = '';
  }

  $node = node_submit($node); // Prepare node for saving
  node_save($node);
  return $node;
}

/**
 * Get the count of workflow_batch nodes that have field_stage_tid value set
 * to $stage_tid.
 *
 * @param string $field_name
 *   The name of the cck field pointing to the taxonomy term
 * @param string $tid
 *   The taxonomy term's tid value
 * for terms such as "New" or "Problem".
 * @return integer
 *   The number of nodes that match the given $stage_tid value.
 */
function islandora_digital_workflow_get_count_of_nodes_by_tid($field_name = '', $tid = 0) {
  $join_table = ($field_name) ? 'field_data_' . $field_name : 'field_data_field_stage';
  $sql = "SELECT count(*) as `count` FROM {node} n " .
      "INNER JOIN {" . $join_table . "} s ON n.nid=s.entity_id " .
      "WHERE (n.type = 'workflow_batch') AND (n.status = 1)" .
      ($tid ? " AND s." . $field_name . "_tid = " . $tid : "") .
      " AND s.delta = 0";
  $result = db_query($sql);
  $results = $result->fetchAll();
  $first_row = array_pop($results);
  $count = is_object($first_row) ? $first_row->count : 0;
  return $count;
}