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
function islandora_digital_workflow_sync_node_batch_record_info($node, $form_state, $batch_items_filename = '', $update_node = TRUE) {
  global $system_creating_nodes;
  $system_creating_nodes = TRUE;
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');

  if (!$batch_items_filename && (isset($_SESSION['uploaded_batch_items_filename']))) {
    $batch_items_filename = $_SESSION['uploaded_batch_items_filename'];
  }
  $batch_name = (is_object($node)) ? $node->title :
      $form_state['values']['batch_name'];
  $batch_record = islandora_digital_workflow_get_batch_record_by_name($batch_name);
  $node = (is_object($node)) ? $node : (($batch_record['nid'] > 0) ? node_load($batch_record['nid']) : NULL);
  $marc_and_mods_records = array();
  if ($batch_items_filename) {
    $mimetype = mime_content_type($batch_items_filename);
    switch ($mimetype) {
      // 1. MRC "application/marc".
      case "application/marc":
      case "application/octet-stream":
        $marc_and_mods_records = islandora_marc_utility_mrc_split($batch_items_filename);
        list($file_header, $file_rows) = islandora_digital_workflow_mrc_results_as_array($marc_and_mods_records);
        $arr = array('header' => $file_header, 'rows' => $file_rows);
        break;
      // 2. MARCXML "application/xml".
      case "application/xml":
        $marc_and_mods_records = islandora_marc_utility_marcxml_split($batch_items_filename);
        list($file_header, $file_rows) = islandora_digital_workflow_mrc_results_as_array($marc_and_mods_records);
        $arr = array('header' => $file_header, 'rows' => $file_rows);
        break;

      default:
        // 3. CSV "text/plain" -- all other cases -- attempt as CSV "text/plain".
        // parse the csv file to get the identifiers and set pass these values
        // along to be added to the node->field_identifiers value
        $arr = islandora_digital_workflow_csv_file_as_array($batch_items_filename);
        break;
    }

  }
  else {
    $arr = array();
  }
  if ($update_node) {
    $node = islandora_digital_workflow_make_or_update_node($form_state, $arr, $node);

    $batch_record = islandora_digital_workflow_create_batch_record_by_form($form_state, $node, $batch_items_filename, NULL, $marc_and_mods_records);
    // Load the batch record related to the node's title value which should
    // be synced with the batch record's batch_name field.
    $node = (is_object($node)) ? $node : (($batch_record['nid'] > 0) ? node_load($batch_record['nid']) : NULL);
    // Sync the CCK field for Batch Defaults with the latest values
    if (is_array($batch_record) && is_object($node)) {
      module_load_include('install', 'islandora_digital_workflow', 'islandora_digital_workflow');
      $schema = islandora_digital_workflow_schema();
      $new_value = theme('islandora_digital_workflow_batch_defaults', array(
          'batch_record' => $batch_record,
          'schema' => $schema));
      $node->field_batch_defaults[$node->language][0]['value'] = $new_value;
      // Make sure that the format is Full HTML here so it doesn't strip any HTML tags.
      $node->field_batch_defaults[$node->language][0]['format'] = 'full_html';
      field_attach_update('node', $node);
    }
    if (isset($_SESSION['uploaded_batch_items_filename'])) {
      @unlink($_SESSION['uploaded_batch_items_filename']);
      unset($_SESSION['uploaded_batch_items_filename']);
    }
    if (file_exists($batch_items_filename)) {
      @unlink($batch_items_filename);
    }
  }
  else {
    // This maps any of the node CCK fields to the ['values']['fieldname'] that 
    // an islandora_digital_workflow_batch record would have as if this was being 
    // passed via the Create Batch / Edit Batch form.
    module_load_include('inc', 'islandora_digital_workflow', 'includes/taxonomy_utilities');
    $form_state = array('values' => array(
      // batch_name is a Unique key, but should not allow the node editor to
      // change that value...
      'batch_name' => $batch_record['batch_name'],
      'islandora_model' => islandora_digital_workflow_get_name_of_tid($node->field_islandora_model[$node->language][0]['tid']),
      'batch_priority_tid' => $node->field_priority[$node->language][0]['tid'],
    ));
    $batch_record = islandora_digital_workflow_create_batch_record_by_form($form_state, $node, $batch_items_filename, NULL, $marc_and_mods_records);
  }
  return $batch_record;
}

/**
 * This will set the stage (field_stage) for the related node.
 *
 * @param integer $nid
 *   nid value for a Drupal node object.
 * @param string $stage_name
 *   The name of the state value like "New", "Problem"
 *
 */
function islandora_digital_workflow_set_node_stage($nid, $stage_name = '', $stage_tid = FALSE, $is_removal = FALSE) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/taxonomy_utilities');
  $node = node_load($nid);
  if (is_object($node)) {
    $stages_vocab = taxonomy_vocabulary_machine_name_load(IDW_STAGE_TAXONOMY_MACHINE_NAME);
    if (!$stage_tid) {
      $stage_tid = islandora_digital_workflow_get_tid_by_name($stage_name, $stages_vocab);
    }
    if ($stage_tid) {
      if ($is_removal) {
        drupal_set_message('For node (' . $nid . '), REMOVE the stage behind $stage_tid = ' . $stage_tid);
        if (array_key_exists($node->language, $node->field_stage)) {
          $original_stages = $node->field_stage[$node->language];
          $node->field_stage[$node->language] = array();
          $removal = FALSE;
          foreach ($original_stages as $stage) {
            if ($stage['tid'] <> $stage_tid) {
              $node->field_stage[$node->language][] = array('tid' => $stage['tid']);
            }
            else {
              $removal = TRUE;
            }
          }
          if ($removal) {
            node_save($node);
          }
          return TRUE;
        }
      }
      else {
        // Check also that this stage tid is not already on the node
        $stage_already_exists = FALSE;
        if (array_key_exists($node->language, $node->field_stage)) {
          foreach ($node->field_stage[$node->language] as $tid_arr) {
            $stage_already_exists |= ($tid_arr['tid'] == $stage_tid);
          }
        }
        if (!$stage_already_exists) {
          $node->field_stage[$node->language][] = array('tid' => $stage_tid);
          node_save($node);
          return TRUE;
        }
      }
    }
  }
  return FALSE;
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
 * the CCK fields with the $form_state and $arr values.
 *
 * @param array $user
 *   A Drupal user.
 * @param array $form_state
 *   An array representing a Drupal form.
 * @param array $arr
 *   An array values extracted from the uploaded items file.
 * @param object $node
 *   Drupal node.
 * @return object
 *   Drupal node.
 */
function islandora_digital_workflow_make_or_update_node($form_state, $arr, $node = NULL) {
  global $user;
  module_load_include('inc', 'islandora_digital_workflow', 'includes/taxonomy_utilities');
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
    $node->field_priority[$node->language][0]['tid'] = $form_state['values']['batch_priority'];

    $node->field_pid_count[$node->language][0]['value'] = $form_state['values']['object_count'];
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
  $csv_values_arr = islandora_digital_workflow_get_csv_values_for_fieldname('identifier', $arr);
  if ((array_key_exists('header', $arr)) && (!(array_search('identifier', $arr['header']) === FALSE)) && (count($arr['rows']) > 0)) {
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
  $join_table = 'field_data_' . $field_name;
  $sql = "SELECT count(*) as `count` FROM {node} n " .
      ($field_name ?
      "INNER JOIN {" . $join_table . "} s ON n.nid=s.entity_id " : "") .
      "WHERE (n.type = 'workflow_batch') AND (n.status = 1)" .
      ($tid ? " AND s." . $field_name . "_tid = " . $tid : "");
  $result = db_query($sql);
  $results = $result->fetchAll();
  $first_row = array_pop($results);
  $count = is_object($first_row) ? $first_row->count : 0;
  return $count;
}
