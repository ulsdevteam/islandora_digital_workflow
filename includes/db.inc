<?php

/**
 * @file
 * Unit of database related functions.
 */

/**
 * Gets a SINGLE batch record for a `batch_name` value.
 *
 * @param string $batch_name
 *   The value of the `batch_name` field.
 * @return mixed
 *   Either returns the batch record object or NULL if it cannot be found.
 */
function islandora_digital_workflow_get_batch_record_by_name($batch_name) {
  if ($batch_name) {
    $query = db_select('islandora_digital_workflow_batch', 'b')
        ->fields('b')
        ->condition('b.batch_name', $batch_name)
        ->range(0,1);
    $results = $query->execute();
    $row = $results->fetchAssoc();
    return $row;
  }
  else {
    return NULL;
  }
}

/**
 * Gets all the names of the batch records.
 *
 * @return array
 *   An array of batch record batch_name values.
 */
function islandora_digital_workflow_get_batch_record_names() {
  $query = db_select('islandora_digital_workflow_batch', 'b')
      ->fields('b', array('batch_name'));
  $results = $query->execute();
  $rows = $results->fetchAll();
  $return_array = array();
  foreach ($rows as $row) {
    $return_array[$row->batch_name] = $row->batch_name;
  }
  return $return_array;
}

/**
 * Returns all batch records.
 *
 * @return mixed
 *   Either returns an array of batch record objects or NULL if it cannot be found.
 */
function islandora_digital_workflow_get_all_batch_records() {
  $query = db_select('islandora_digital_workflow_batch', 'b')
      ->fields('b');
  $results = $query->execute();
  $rows = $results->fetchAll();
  return $rows;
}

/**
 * Gets a SINGLE batch record for a `batch_name` value.
 *
 * @param string $nid
 *   Drupal node's nid property.
 * @return mixed
 *   Either returns the batch record object or NULL if it cannot be found.
 */
function islandora_digital_workflow_get_batch_record_by_nid($nid) {
  if ($nid) {
    $query = db_select('islandora_digital_workflow_batch', 'b')
        ->fields('b')
        ->condition('b.nid', $nid)
        ->range(0,1);
    $results = $query->execute();
    $row = $results->fetchAssoc();
    return $row;
  }
  else {
    return NULL;
  }
}

/**
 * Gets a SINGLE batch record for a `batch_id` value.
 *
 * @param string $batch_id
 *   Optional - the `batch_id` value for a batch record.
 * @param string $batch_id
 *   Optional - the `batch_item_id` value for any batch_items record related to
 * the batch in question.
 * @return mixed
 *   Either returns the batch record object or NULL if it cannot be found.
 */
function islandora_digital_workflow_get_batch_record_by_batch_id($batch_id = NULL, $batch_item_id = NULL) {
  if (!is_null($batch_id)) {
    $query = db_select('islandora_digital_workflow_batch', 'b')
        ->fields('b')
        ->condition('b.batch_id', $batch_id)
        ->range(0,1);
    $results = $query->execute();
    $row = $results->fetchAssoc();
    return $row;
  }
  elseif (!is_null($batch_item_id)) {
    $query = db_select('islandora_digital_workflow_batch', 'b')
        ->fields('b');
    $query->join('islandora_digital_workflow_batch_items', 'bi', 'b.batch_id = bi.batch_id');
    $query->condition('bi.batch_item_id', $batch_item_id)
        ->range(0,1);
    $results = $query->execute();
    $row = $results->fetchAssoc();
    return $row;
  }
  else {
    return NULL;
  }
}

/**
 * This will inspect the transaction object and use the appropriate query
 * to obtain the batch record that relates to it.
 *
 * @param object $transaction
 *   A transaction record object.
 * @return array
 *   The batch_record as an array -- same format as is returned
 * islandora_digital_workflow_get_batch_record_by_batch_id.
 */
function islandora_digital_workflow_get_batch_record_by_transaction($transaction) {
  if (!is_null($transaction->batch_id)) {
    return islandora_digital_workflow_get_batch_record_by_batch_id($transaction->batch_id);
  }
  else {
    $batch_item_record_arr = islandora_digital_workflow_get_batch_item_record($transaction->batch_item_id);
    if (is_array($batch_item_record_arr)) {
      $batch_item_record_obj = array_pop($batch_item_record_arr);
      return islandora_digital_workflow_get_batch_record_by_batch_id($batch_item_record_obj->batch_id);
    }
  }
  return array();
}

/**
 * Gets the batch_item records identifier values related to a given batch.
 *
 * @param integer $batch_id
 *   batch_id for which to get the related batch_item records' identifier values.
 *
 * @return array
 *   An associative array of results.
 */
function islandora_digital_workflow_get_existing_identifiers($batch_id) {
  if ($batch_id) {
    $query = db_select('islandora_digital_workflow_batch', 'b');
    $query->join('islandora_digital_workflow_batch_items', 'bi', 'b.batch_id = bi.batch_id');
    $query->fields('bi', array('batch_item_id','identifier'));
    $query->condition('b.batch_id', $batch_id)->execute();
    $results = $query->execute();
    $rows = $results->fetchAll();
    $return_array = array();
    foreach ($rows as $item_identifier_object) {
      $return_array[$item_identifier_object->batch_item_id] = $item_identifier_object->identifier;
    }
    return $return_array;
  }
  else {
    return NULL;
  }
}

/**
 * This will update a batch record related to a given $batch_id.
 *
 * @param integer $batch_id
 *   batch_id for which to update a batch record.
 * @param array $field_values
 *   associative array of values to set on the batch record.
 * @return integer
 *   The number of records that have been updated (expect 1 or 0).
 */
function islandora_digital_workflow_update_batch($batch_id, $field_values) {
    return db_update('islandora_digital_workflow_batch')
      ->fields($field_values)
      ->condition('batch_id', $batch_id)
      ->execute();
}

/**
 * Returns all batch records that match part of the batch_name field.
 *
 * @param string $batch_name
 *   The value of the `batch_name` field.
 * @return mixed
 *   Either returns an array of batch record objects or NULL if it cannot be found.
 */
function islandora_digital_workflow_find_batch_records_by_name($batch_name) {
  if ($batch_name) {
    $query = db_select('islandora_digital_workflow_batch', 'b')
        ->fields('b')
        ->condition('b.batch_name', '%' . db_like($batch_name) . '%', 'LIKE');
    $results = $query->execute();
    $rows = $results->fetchAll();
    return $rows;
  }
  else {
    return NULL;
  }
}

/**
 * Returns all batch records that match part of the batch_description field.
 *
 * @param string $searchterm
 *   The value of the `batch_name` field.
 * @return mixed
 *   Either returns an array of batch record objects or NULL if it cannot be found.
 */
function islandora_digital_workflow_find_batch_records_by_description($searchterm) {
  if ($searchterm) {
    $query = db_select('islandora_digital_workflow_batch', 'b')
        ->fields('b')
        ->condition('b.batch_description', '%' . db_like($searchterm) . '%', 'LIKE');
    $results = $query->execute();
    $rows = $results->fetchAll();
    return $rows;
  }
  else {
    return NULL;
  }
}

/**
 * Returns all batch records that have a partial match for the `identifier` field
 * in the islandora_digital_workflow_batch_items table.
 *
 * @param string $identifier
 *   The value of the islandora_digital_workflow_batch_items.`identifier` field.
 * @return mixed
 *   Either returns an array of batch record objects or NULL if it cannot be found.
 */
function islandora_digital_workflow_get_batch_record_by_items_identifier($identifier) {
  if ($identifier) {
    $query = db_select('islandora_digital_workflow_batch', 'b');
    $query->join('islandora_digital_workflow_batch_items', 'bi', 'b.batch_id = bi.batch_id');
    $query->fields('b')
        ->condition('bi.identifier', '%' . db_like($identifier) . '%', 'LIKE')
        ->groupBy('b.batch_name');
    $query->addExpression('GROUP_CONCAT(identifier SEPARATOR \', \')', 'identifiers');
    $results = $query->execute();
    $rows = $results->fetchAll();
    return $rows;
  }
  else {
    return NULL;
  }
}

/**
 * Returns all batch records that have a partial match for the `uploaded_batch_items_file` field
 * in the islandora_digital_workflow_batch table.
 *
 * @param string $identifier
 *   The value of the islandora_digital_workflow_batch.`uploaded_batch_items_file` field.
 * @return mixed
 *   Either returns an array of batch record objects or NULL if it cannot be found.
 */
function islandora_digital_workflow_search_batch_csv_by_identifier($identifier) {
  if ($identifier) {
    $query = db_select('islandora_digital_workflow_batch', 'b');
    $query->fields('b')
        ->condition('b.uploaded_batch_items_file', '%' . db_like($identifier) . '%', 'LIKE')
        ->groupBy('b.batch_name');
    $results = $query->execute();
    $rows = $results->fetchAll();
    return $rows;
  }
  else {
    return NULL;
  }
}

/**
 * Dynamic search of the submitted webform data based on configuration.
 *
 * To preserve any instances of a possible search match, but that wasn't in
 * one of the searched fields.  This uses the $highlight_hash to temporarily 
 * replace any instances with a unique string.  This value is passed back in
 * the results array, but the code that highlights the search result records 
 * will do a string replace AFTER HIGHLIGHTING to put the original searchterm
 * value back for each instance of this hash value.
 *
 * @param string $searchterm
 *   The string value that the user is looking up.
 * @param string $highlight_hash
 *   The unique string that will be used to replace a searchterm match in any
 * non-search field data.
 * @return array
 *   An array of webform submitted data that matched a field value for the
 * $searchterm.
 */
function islandora_digital_workflow_search_digitization_requests($searchterm, $highlight_hash = '') {
  if ($searchterm) {
    // Loop through all of the fields that are configured for each of the
    // "digitization request" webforms and union the queries together with each
    // one having a specific value for their configured condition such as:
    //   wc.form_key IN ('staff_name', 'description')
    module_load_include('inc', 'islandora_digitization_requests', 'includes/db');
    module_load_include('inc', 'islandora_digital_workflow', 'includes/create_digitization_request.form');
    $webforms = islandora_digitization_requests_get_webforms();
    $digitization_request_webforms = islandora_digitization_requests_get_digitization_request_webforms($webforms);
    $allrows = array();
    foreach ($digitization_request_webforms as $nid => $webform_title) {
      $search_fields = islandora_digital_workflow_searchable_fields($nid, TRUE);
      if (count($search_fields) > 0) {
        $components = islandora_digitization_requests_webform_components($nid);
        $query = db_select('webform_submitted_data', 'wd')
            ->fields('wd');
        $query->join('webform_submissions', 'ws', 'wd.sid = ws.sid AND wd.nid = ws.nid');
        $query->fields('ws', array('uid','submitted', 'remote_addr'));
        $query->addJoin('LEFT OUTER', 'users', NULL, 'ws.uid = users.uid');
        $query->addField('users', 'name', 'user_name');
        $query->join('webform_component', 'wc', 'wd.cid = wc.cid AND wd.nid = wc.nid');
        $query->fields('wc', array('form_key'));
        $query->join('node', 'n', 'n.nid = wc.nid');
        $query->addField('n', 'title', 'webform_title');
        $query->condition('wd.nid', $nid);
        $query->condition('wc.form_key', $search_fields, 'IN');
        $query->condition('wd.data', '%' . db_like($searchterm) . '%', 'LIKE');
        $query->orderBy('ws.submitted', 'DESC');
        $results = $query->execute();
        $rows = $results->fetchAll();
        foreach ($rows as $idx => $row) {
          $submission_tooltip = islandora_digital_workflow_get_submission_tooltip($row->sid, $searchterm, $search_fields, $highlight_hash);
          $row->submission_tooltip = $submission_tooltip;
          $row->batch_id = 'ws_' . $nid . '_' . $row->sid;
          $allrows[] = $row;
          $rows[$idx]->submission_tooltip = $submission_tooltip;
          $rows[$idx]->batch_id = $row->batch_id;
        }
      }
    }
    return $allrows;
  }
  else {
    return NULL;
  }
}

/**
 * Helper function that returns the HTML for a tooltip based on a webform
 * submission.
 *
 * @param integer $sid
 *   The submission identifier.
 * @param string $searchterm
 *   The search text.
 * @param array $search_fields
 *   A set of fields that were searched - based on configuration.
 * @param string $highlight_hash
 *   The unique string that will be used to replace a searchterm match in any
 * non-search field data.
 * @return type
 */
function islandora_digital_workflow_get_submission_tooltip($sid, $searchterm, $search_fields, $highlight_hash) {
  $submission_arr = islandora_digitization_requests_get_webform_submission($sid, TRUE);
  $output = array();
  foreach ($submission_arr as $cid => $submission_field_arr) {
    $submission_field_arr = reset($submission_field_arr);
    // Protect the data value if this field is not one of the searchable ones
    $searched_this_field = (!(array_search($submission_field_arr['form_key'], $search_fields) === FALSE));
    if (!$searched_this_field) {
      $submission_field_arr['data'] = str_replace($searchterm, $highlight_hash, $submission_field_arr['data']);
    }
    $data_value = nl2br($submission_field_arr['data']);
    $data_truncated_ellipsis = (strlen($data_value) > 225) ? ' ... ' : '';
    $output[] = ($searched_this_field) ?
        '<b>' . $submission_field_arr['component_name'] . ' </b> = "' . substr($data_value, 0, 225) . $data_truncated_ellipsis . '"' :
        '<span class="disabled_text"><b>' . $submission_field_arr['component_name'] . ' </b> = "' . substr($data_value, 0, 225) . $data_truncated_ellipsis . '"</span>';
  }
  return implode("<br>", $output);
}

/**
 * Returns all batches that have a different number for object_count than the
 * actual number of items in the islandora_digital_workflow_batch_items table.
 *
 * @return  array
 *   Either returns an array of values related to counting the items for batch
 * records where the object_count value is not equal to the number of records
 * related to the same batch's islandora_digital_workflow_batch_items table.
 */
function islandora_digital_workflow_find_batch_items_count_mismatch() {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/taxonomy_utilities');
  // Since this needs to use a subquery, the SQL is MUCH easier to write out
  $sql = 'SELECT jbi.items_count, b.object_count, b.*, t.weight, t.name as `priority` ' .
         'FROM islandora_digital_workflow_batch b ' .
         'LEFT OUTER JOIN taxonomy_term_data t ON (b.batch_priority_tid = t.tid) ' .
         'LEFT OUTER JOIN ' .
         '(SELECT count(*) as `items_count`, batch_id ' .
         ' FROM islandora_digital_workflow_batch_items GROUP BY batch_id) as jbi ' .
         ' ON (jbi.batch_id = b.batch_id) ' .
         'HAVING jbi.items_count <> b.object_count ' .
         'ORDER BY `weight`';
  $results = db_query($sql);
  $rows = $results->fetchAll();

  $batch_records = array();
  foreach ($rows as $row) {
    $row->priority = islandora_digital_workflow_get_name_of_tid($row->batch_priority_tid);
    $batch_records[$row->batch_id] = $row;
  }
  return $batch_records;
}

/**
 * Returns the set of batch records that have no batch_items related to them.
 *
 * @return array
 *   Array of stdObject rows.
 */
function islandora_digital_workflow_find_batch_records_no_items() {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/taxonomy_utilities');
  $query = db_select('islandora_digital_workflow_batch', 'b')
      ->fields('b');
  $query->addJoin('LEFT OUTER', 'islandora_digital_workflow_batch_items', 'bi', 'b.batch_id = bi.batch_id');
  $query->fields('bi');
  $query->addJoin('LEFT OUTER', 'taxonomy_term_data', 't', 'b.batch_priority_tid = t.tid');
  $query->addField('t', 'weight');
  $query->addField('t', 'name', 'priority');
  $query->orderBy('weight');

  $query->groupBy('b.batch_id');

  $results = $query->execute();
  $rows = $results->fetchAll();
  
  $no_items = array();
  // Check that the value of the batch_item_id is NULL
  foreach ($rows as $row) {
    $row->priority = islandora_digital_workflow_get_name_of_tid($row->batch_priority_tid);
    if (is_null($row->batch_item_id)) {
      $no_items[] = $row;
    }
  }
  return $no_items;
}

/**
 * Returns the set of batch records that are requested batches with a due date.
 *
 * @return array
 *   Array of stdObject rows.
 */
function islandora_digital_workflow_find_batch_records_is_batch_request() {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/taxonomy_utilities');
  $query = db_select('islandora_digital_workflow_batch', 'b')
      ->fields('b')
      ->condition('b.is_batch_request', 1)
      ->condition('b.progress', 'Completed', '<>');
  $query->addJoin('LEFT OUTER', 'taxonomy_term_data', 't', 'b.batch_priority_tid = t.tid');
  $query->addField('t', 'weight');
  $query->addField('t', 'name', 'priority');
  $query->orderBy('b.batch_request_due_date');

  $query->groupBy('b.batch_id');

  $results = $query->execute();
  $rows = $results->fetchAll();

  $batch_request_rows = array();
  // Check that the value of the batch_item_id is NULL
  foreach ($rows as $row) {
    $row->priority = islandora_digital_workflow_get_name_of_tid($row->batch_priority_tid);
    $row->how_long_from_now = islandora_digital_workflow_timeago_from_timestamp(date('c', $row->batch_request_due_date));
    $batch_request_rows[] = $row;
  }
  return $batch_request_rows;
}

/**
 * Gets an array of batch_item_identifiers and previous transaction_id values.
 *
 * For mapping the items to transactions when replacing (recreating) the
 * batch_items records.
 *
 * @param integer $batch_id
 *   The batch_id value for matching batch record
 * @return array
 *   An associative array where key is old transaction record's transaction_id
 * value and the value is an array of values for batch item's identifier and timestamp
 * for the previous batch_item -- transaction records.
 */
function islandora_digital_workflow_get_batch_item_ids_to_transaction_ids($batch_id) {
  $query = db_select('islandora_digital_workflow_batch', 'b');
  $query->join('islandora_digital_workflow_batch_items', 'bi', 'b.batch_id = bi.batch_id');
  $query->fields('bi', array('batch_item_id','identifier'));
  $query->join('islandora_digital_workflow_transactions', 't', 't.batch_item_id = bi.batch_item_id');
  $query->fields('t', array('transaction_id', 'timestamp'));
  $query->condition('b.batch_id', $batch_id)->execute();
  $results = $query->execute();
  $rows = $results->fetchAll();
  $return_rows = array();
  foreach ($rows as $row) {
    $return_rows[$row->identifier][] = array(
        'timestamp' => $row->timestamp,
        'transaction_id' => $row->transaction_id);
  }
  return $return_rows;
}

/**
 * This will get all of the transactions records related to a given $transaction_id.
 *
 * @param integer $transaction_id
 *   transaction_id representing a islandora_digital_workflow_transactions record.
 * @return array
 *   Either returns an array of batch record objects or NULL if it cannot be found.
 */
function islandora_digital_workflow_get_batch_transaction($transaction_id) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  $query = db_select('islandora_digital_workflow_transactions', 'bt');
  $query->fields('bt', array('timestamp', 'transaction_id', 'batch_id', 'batch_item_id'));
  $query->join('islandora_digital_workflow_actions', 'a', 'a.action_id = bt.action_id');
  $query->fields('a', array('description', 'action_id'));
  $query->condition('bt.transaction_id', $transaction_id);

  $results = $query->execute();
  $rows = $results->fetchAll();
  if (count($rows) > 0) {
    foreach ($rows as $index => $row) {
      $row->how_long_ago = islandora_digital_workflow_timeago_from_timestamp($row->timestamp);
    }
  }
  else {
    $row = NULL;
  }
  return $row;
}

/**
 * This will delete a transaction record related to a given $transaction_id.
 *
 * @param integer $transaction_id
 *   transaction_id representing a islandora_digital_workflow_transactions record.
 * @return array
 *   The number of records that have been updated (expect 1 or 0).
 */
function islandora_digital_workflow_delete_batch_transaction($transaction_id) {
  return db_delete('islandora_digital_workflow_transactions')
    ->condition('transaction_id', $transaction_id)
    ->execute();
}

/**
 * This will update a transaction record related to a given $transaction_id.
 *
 * Additionally, this code will call islandora_digital_workflow_set_problem_record
 *
 * @param integer $transaction_id
 *   transaction_id representing a islandora_digital_workflow_transactions record.
 * @param integer $action_id
 *   action_id for which to update this transaction.
 * @param integer $timestamp
 *   timestamp value to set for the record (taken from previous instance of record
 * that is being updated).
 * @return integer
 *   The number of records that have been updated (expect 1 or 0).
 */
function islandora_digital_workflow_update_batch_transaction($transaction_id, $action_id, $timestamp, $problem_notes = '') {
  global $user;
  $updated = db_update('islandora_digital_workflow_transactions')
    ->fields(array('action_id' => $action_id, 'timestamp' => $timestamp, 'drupal_uid' => $user->uid))
    ->condition('transaction_id', $transaction_id)
    ->execute();
  // check to see if this specific change will trigger any transaction Stage changes
  islandora_digital_workflow_check_trigger_stage_change($transaction_id);

  return $updated;
  if ($action_id == IDW_ACTION_PROBLEM) {
    islandora_digital_workflow_set_problem_record($transaction_id, $user->uid, $problem_notes);
  }
}

/**
 *
 * (see: islandora_digital_workflow_insert_transactions_record)
 *
 * @param integer $transaction_id
 *   The transaction_id of the transaction that was just added (see:
 * islandora_digital_workflow_insert_transactions_record).
 * @param type $transaction_values
 *   The relevant transaction values in an array of action_id, batch_id,
 * batch_item_id.
 * @return type
 *   No return value.  `return` call only used to exit the procedure.
 */
function islandora_digital_workflow_check_trigger_stage_change($transaction_id = NULL, $transaction_values = NULL) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/node_utilities');
  if (!(is_null($transaction_id) && is_array($transaction_values))) {
    $transaction_values = islandora_digital_workflow_get_batch_transaction($transaction_id);
  }
  $transaction_values = (array) $transaction_values;
  // Given the transaction record, we can see what the action_id is and look that 
  // up to see whether or not it should trigger a "Stage" change - then to look
  // further to know whether or not this change alone or "all batch items" have to 
  // have this action in order to fire the trigger.
  $action_record = islandora_digital_workflow_get_action($transaction_values['action_id']);

  // Also, in order to set a stage_taxonomy_tid, we will need the node nid value 
  // from the batch_record.
  if (is_null($transaction_values['batch_id'])) {
    $batch_item_record = islandora_digital_workflow_get_batch_item($transaction_values['batch_item_id']);
    $batch_record = islandora_digital_workflow_get_batch_record_by_batch_id($batch_item_record->batch_id);
  }
  else {
    $batch_item_record = NULL;
    $batch_record = islandora_digital_workflow_get_batch_record_by_batch_id($transaction_values['batch_id']);
  }

  if ((!isset($action_record['stage_taxonomy_tid']) || is_null($action_record['add_term_name'])) &&
    (!isset($action_record['stage_taxonomy_tid_rm']) || is_null($action_record['rm_term_name']))) {
    return;
  }
  $affects = (($action_record['is_batch_action'] || $action_record['is_triggered_by_single_item']) ?
      '"@action_description" has been added' :
      'All Items in the batch have "@action_description"');
  $message_args = array(
      '@action_description' => $action_record['action_description'],
      '@stage' => $action_record['add_term_name'],
      '@rm_stage' => $action_record['rm_term_name'],
      '@batch_item_identifier' => (($batch_item_record &&
        is_object($batch_item_record->identifier)) ?
          $batch_item_record->identifier : ''),
      '@batch_name' => $batch_record['batch_name'],);
  $message_string = 
    (($batch_item_record) ? ' for the item @batch_item_identifier' . $batch_item_record->identifier : '') .
    (($batch_record) ? ' on the batch "@batch_name"' : '');
  if ($action_record['is_batch_action'] == 1 || $action_record['is_triggered_by_single_item']) {
    if (islandora_digital_workflow_set_node_stage($batch_record['nid'], '', $action_record['stage_taxonomy_tid'])) {
      drupal_set_message(t($affects . ' which triggers the "@stage stage" for ' .
          $message_string, $message_args));
    }
  }
  else {
    // Load $items with the $batch_record->nid value
    $items = islandora_digital_workflow_get_batch_items_by_nid($batch_record['nid']);
    $all_items_have_action = TRUE;
    foreach ($items as $item_obj) {
      $transaction_action_ids = explode(',', $item_obj->transaction_action_ids);
      $all_items_have_action &= (!(array_search($transaction_values['action_id'], $transaction_action_ids) === FALSE));
    }
    if ($all_items_have_action) {
      if (islandora_digital_workflow_set_node_stage($batch_record['nid'], '', $action_record['stage_taxonomy_tid'])) {
        drupal_set_message(t($affects . ' which triggers the "@stage stage" for ' .
            $message_string, $message_args));
      }
    }
  }
  if ($action_record['is_batch_action'] == 1 || $action_record['is_triggered_by_single_item_rm']) {
    if (islandora_digital_workflow_set_node_stage($batch_record['nid'], '', $action_record['stage_taxonomy_tid_rm'], TRUE)) {
      drupal_set_message(t($affects . ' which triggers the "@rm_stage stage" for ' .
          $message_string, $message_args));
    }
  }
  else {
    // Load $items with the $batch_record->nid value
    $items = islandora_digital_workflow_get_batch_items_by_nid($batch_record['nid']);
    $all_items_have_action = TRUE;
    foreach ($items as $item_obj) {
      $transaction_action_ids = explode(',', $item_obj->transaction_action_ids);
      $all_items_have_action &= (!(array_search($transaction_values['action_id'], $transaction_action_ids) === FALSE));
    }
    if ($all_items_have_action) {
      if (islandora_digital_workflow_set_node_stage($batch_record['nid'], '', $action_record['stage_taxonomy_tid_rm'], TRUE)) {
        drupal_set_message(t($affects . ' which triggers the "@stage stage" for ' .
            $message_string, $message_args));
      }
    }
  }
}

/**
 * Set the problem record related to the given transaction.
 *
 * This will check whether or not the supplied action is IDW_ACTION_PROBLEM....
 * and will also insert the data to the node (so that the
 * node for the entire batch is switched to "Problem" as well as to insert
 * an individual islandora_digital_workflow_problem_items record.
 *
 * @global object $user
 *   Drupal user object
 * @param integer $transaction_id
 *   Identifier for a record in the islandora_digital_workflow_transactions table.
 * @param NULL|integer $alternative_uid
 *   An alternate user uid value to use for this transaction.
 * @param string $problem_notes
 *   The notes that the user entered to submit the problem with the item.
 */
function islandora_digital_workflow_set_problem_record($transaction_id, $alternative_uid = NULL, $problem_notes = '') {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  global $user;
  $transaction = islandora_digital_workflow_get_batch_transaction($transaction_id);
  if (is_object($transaction)) {
    $batch_record = islandora_digital_workflow_get_batch_record_by_transaction($transaction);
    db_query("INSERT INTO islandora_digital_workflow_problem_items (" .
        "`transaction_id`, `problem_notes`, `problem_timestamp`, `drupal_uid`) VALUES (" .
        $transaction_id . ", '" . addslashes($problem_notes) . "', now(), " .
        (!is_null($alternative_uid) ? $alternative_uid : $user->uid) . ")");
    // Move the files related to this item over to the configured Problems path.
    islandora_digital_workflow_move_batch_item_file($transaction->batch_item_id, variable_get('islandora_digital_workflow_ingest_problems_path', ''), $problem_notes);
  }
}

/**
 * This will get all of the transactions records related to a given Drupal node's nid.
 *
 * @param integer $nid
 *   $nid value from a Drupal node object.
 * @return array
 *   Either returns an array of batch record objects or NULL if it cannot be found.
 */
function islandora_digital_workflow_get_batch_transactions_by_nid($nid) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');

  $query = db_select('islandora_digital_workflow_batch', 'b');
  $query->join('islandora_digital_workflow_transactions', 'bt', 'b.batch_id = bt.batch_id');
  $query->fields('bt', array('timestamp', 'transaction_id'));
  $query->addJoin('LEFT OUTER', 'users', NULL, 'bt.drupal_uid = users.uid');
  $query->fields('users', array('uid', 'name'));
  $query->join('islandora_digital_workflow_actions', 'a', 'a.action_id = bt.action_id');
  $query->fields('a', array('description'));
  $query->addField('a', 'name', 'action_name');
  $query->condition('b.nid', $nid)
      ->orderBy('bt.timestamp', 'ASC');

  $access_edit_transactions = user_access(ISLANDORA_DIGITAL_WORKFLOW_EDIT_DELETE_TRANSACTIONS);
  $results = $query->execute();
  $rows = $results->fetchAll();
  foreach ($rows as $index => $row) {
    $rows[$index]->how_long_ago = islandora_digital_workflow_timeago_from_timestamp($row->timestamp);
    $rows[$index]->glyph_class = islandora_digital_workflow_glyph_class($row->action_name);
    $rows[$index]->user_name = ($row->uid) ? l($row->name, 'user/' . $row->uid, array('attributes'=>array(
      'title' => 'link opens in separate tab',
      'class' => array('link_open_new_tab_tiny'),
      'target' => '_blank'))) : 'system';
    $rows[$index]->admin_links = ($access_edit_transactions) ? TRUE : FALSE;
    $rows[$index]->nid = $nid;
  }
  return $rows;
}

/**
 * This will run a SQL query to find the most recent timestamp value for a
 * transaction that happened on a batch or batch_item.
 *
 * @param integer $batch_item_id
 *   $batch_item_id value.
 * @param string $batch_id
 *   The `batch_id` value for a batch record.
 * @return \stdClass
 *   A single transaction for the batch_item or batch that has properties for
 * how_long_ago and max_timestamp.
 */
function islandora_digital_workflow_get_max_timestamp_for_batch_item_transactions($batch_item_id, $batch_id) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');

  $sql = 'SELECT MAX(bt.timestamp) AS `max_timestamp` ' .
         'FROM islandora_digital_workflow_transactions bt ' .
         'LEFT OUTER JOIN islandora_digital_workflow_problem_items p ON bt.transaction_id = p.transaction_id ' .
         'WHERE (p.problem_resolved_timestamp IS NULL AND ' .
         ' (bt.batch_item_id = ' . $batch_item_id . ' OR ' .
         ' bt.batch_id = ' . $batch_id . '))';

  $results = db_query($sql);
  $rows = $results->fetchAll();
  $return_object = new stdClass();
  foreach ($rows as $index => $row) {
    $return_object->how_long_ago = islandora_digital_workflow_timeago_from_timestamp($row->max_timestamp);
    $return_object->max_timestamp = $row->max_timestamp;
  }
  return $return_object;
}

/**
 * This will get all of the transactions records related to a given Drupal batch_item.
 *
 * @param integer $batch_item_id
 *   $batch_item_id value.
 * @param integer $nid
 *   $nid value from a Drupal node object.
 * @return array
 *   Either returns an array of batch record objects or NULL if it cannot be found.
 */
function islandora_digital_workflow_get_batch_item_transactions($batch_item_id, $nid, $batch_id) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  $sql = 'SELECT bt.timestamp AS `timestamp`, bt.transaction_id AS transaction_id, ' .
         'users.uid AS uid, users.name AS name, a.description AS description, ' .
         'a.action_id AS action_id, a.name AS action_name, a.is_batch_action AS is_batch_action, p.problem_notes ' .
         'FROM islandora_digital_workflow_transactions bt ' .
         'LEFT OUTER JOIN islandora_digital_workflow_problem_items p ON bt.transaction_id = p.transaction_id ' .
         'LEFT OUTER JOIN users users ON bt.drupal_uid = users.uid ' .
         'INNER JOIN islandora_digital_workflow_actions a ON a.action_id = bt.action_id ' .
         'WHERE  (bt.batch_item_id = ' . (0 + $batch_item_id) . ' AND p.problem_resolved_timestamp IS NULL) ' .
         'UNION SELECT bt.timestamp AS `timestamp`, bt.transaction_id AS transaction_id, ' .
         'users.uid AS uid, users.name AS name, a.description AS description, ' .
         'a.action_id AS action_id, a.name AS action_name, a.is_batch_action AS is_batch_action, p.problem_notes ' .
         'FROM islandora_digital_workflow_transactions bt ' .
         'LEFT OUTER JOIN islandora_digital_workflow_problem_items p ON bt.transaction_id = p.transaction_id ' .
         'LEFT OUTER JOIN users users ON bt.drupal_uid = users.uid ' .
         'INNER JOIN islandora_digital_workflow_actions a ON a.action_id = bt.action_id ' .
         'WHERE  (bt.batch_id = ' . (0 + $batch_id) . ' AND p.problem_resolved_timestamp IS NULL) ' .
         'ORDER BY `timestamp` ASC';

  $results = db_query($sql);
  $access_edit_transactions = user_access(ISLANDORA_DIGITAL_WORKFLOW_EDIT_DELETE_TRANSACTIONS);
  $rows = $results->fetchAll();
  foreach ($rows as $index => $row) {
    $rows[$index]->how_long_ago = islandora_digital_workflow_timeago_from_timestamp($row->timestamp);
    $rows[$index]->user_name = ($row->uid) ? l($row->name, 'user/' . $row->uid, array('attributes'=>array(
      'title' => 'link opens in separate tab',
      'class' => array('link_open_new_tab_tiny'),
      'target' => '_blank'))) : 'system';
    $rows[$index]->glyph_class = islandora_digital_workflow_glyph_class($row->action_name);
    $rows[$index]->admin_links = ($access_edit_transactions) ? TRUE : FALSE;
    $rows[$index]->nid = $nid;
    $rows[$index]->is_batch_action = $row->is_batch_action;
    $rows[$index]->transaction_id = $row->transaction_id;
    $rows[$index]->problem_notes = $row->problem_notes;
  }
  return $rows;
}

/**
 * This will get all of the transactions records related to a given Drupal batch_item.
 *
 * @param integer $batch_item_id
 *   $batch_item_id value matching an individual batch item.
 * @param type $batch_id
 *   The $batch_id value matching a batch.
 * @param boolean $unresolved
 *   Whether or not to restrict the query to problems that have been resolved.
 * @return array
 *   Either returns an array of batch record objects or NULL if it cannot be found.
 */
function islandora_digital_workflow_get_batch_item_problems($batch_item_id, $batch_id, $unresolved = FALSE) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');

  $sql = 'SELECT p.problem_notes, p.problem_timestamp, p.problem_resolved_timestamp, ' .
         'users.uid AS uid, users.name AS user_name, bt.batch_id, bt.batch_item_id, bi.identifier, bi.title ' .
         'FROM islandora_digital_workflow_problem_items p ' .
         'JOIN islandora_digital_workflow_transactions bt ON p.transaction_id = bt.transaction_id ' .
         'LEFT OUTER JOIN islandora_digital_workflow_batch_items bi ON bt.batch_item_id = bi.batch_item_id ' .
         'LEFT OUTER JOIN users users ON bt.drupal_uid = users.uid ' .
         'WHERE ((bt.batch_item_id = ' . $batch_item_id . ' OR bt.batch_id = ' . $batch_id . ') AND ' . ($unresolved ? '' : 'NOT ') . 'p.problem_resolved_timestamp IS NULL) ' .
         'ORDER BY p.problem_timestamp DESC';

  $results = db_query($sql);
  $rows = $results->fetchAll();
  foreach ($rows as $index => $row) {
    $rows[$index]->problem_how_long_ago = islandora_digital_workflow_timeago_from_timestamp($row->problem_timestamp);
    $rows[$index]->problem_resolved_how_long_ago = (($unresolved) ? '' : islandora_digital_workflow_timeago_from_timestamp($row->problem_resolved_timestamp));
    $rows[$index]->user_name = ($row->uid) ? l($row->user_name, 'user/' . $row->uid, array('attributes'=>array(
      'title' => 'link opens in separate tab',
      'class' => array('link_open_new_tab_tiny'),
      'target' => '_blank'))) : 'system';
    $rows[$index]->problem_notes = $row->problem_notes;
  }
  return $rows;
}

/**
 * Returns all batch records that are related to a node's nid value (through
 * the relationship between islandora_digital_workflow_batch and
 * islandora_digital_workflow_batch_items.
 *
 * @param integer $nid
 *   The value of the islandora_digital_workflow_batch_items.`identifier` field.
 * @return mixed
 *   Either returns an array of batch record objects or NULL if it cannot be found.
 */
function islandora_digital_workflow_get_batch_files_by_nid($nid) {
  if ($nid) {
    $query = db_select('islandora_digital_workflow_batch', 'b');
    $query->join('islandora_digital_workflow_batch_items', 'bi', 'b.batch_id = bi.batch_id');
    $query->fields('bi')
        ->condition('b.nid', $nid);
    $results = $query->execute();
    $rows = $results->fetchAll();
    foreach ($rows as $index => $row) {
      $filename_pathinfo = pathinfo($row->filename);
      $rows[$index]->master_filename_basename = $filename_pathinfo['basename'];
      $rows[$index]->filesize = (file_exists($row->filename)) ? filesize($row->filename) : NULL;
    }
    return $rows;
  }
  else {
    return NULL;
  }
}

/**
 * This will get all of the batch_items records and their transactions related to
 * a given Drupal node's nid.
 *
 * @param integer $nid
 *   $nid value from a Drupal node object.
 * @return array
 *   Either returns an array of batch record objects or NULL if it cannot be found.
 */
function islandora_digital_workflow_get_batch_items_by_nid($nid) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');

  $rows = array();
  if ($nid) {
    $required_batch_actions = islandora_digital_workflow_get_required_batch_actions(NULL, $nid);
    // Due to the nature of this query and how stubborn the db_select is with creating
    // a LEFT JOIN query that uses GROUP BY and GROUP_CONCAT to display the needed
    // field values, this is much MUCH MUCH easier to write out as a SQL statement.
    $query = db_query('SELECT bi.identifier, bi.title, bi.islandora_model, bi.filename,
      \'\' as `ready_for`,
      (SELECT description FROM islandora_digital_workflow_actions wfa
       JOIN islandora_digital_workflow_transactions wft ON (wft.action_id = wfa.action_id)
       WHERE wft.transaction_id = MAX(bt.transaction_id)) as `last_action`,
      (MAX(bt.timestamp)) as `when`,
      GROUP_CONCAT(DISTINCT a.name) as `transaction_actions`,
      GROUP_CONCAT(DISTINCT a.action_id) as `transaction_action_ids`,
      bi.progress, bi.batch_item_id
  FROM islandora_digital_workflow_batch b
  JOIN islandora_digital_workflow_batch_items bi ON b.batch_id = bi.batch_id
  LEFT JOIN islandora_digital_workflow_transactions bt ON (bi.batch_item_id = bt.batch_item_id OR bi.batch_id = bt.batch_id)
  LEFT JOIN islandora_digital_workflow_actions a ON a.action_id = bt.action_id
  WHERE b.nid = ' . $nid . '
  GROUP BY bi.batch_item_id
  ORDER BY bi.batch_item_id ASC, bt.`timestamp` DESC');
    $rows = $query->fetchAll();
    // Loop through and calculate values for master_filename_basename by using the
    // pathinfo ~ basename element.
    foreach ($rows as $index => $row) {
      $filename_pathinfo = pathinfo($row->filename);
      $rows[$index]->master_filename_basename = $filename_pathinfo['basename'];
      // Need to put the action_ids together with the transaction descriptions...
      // - it is easier on the server than running subsequent queries to get these.
      $actions = explode(",", $row->transaction_actions);
      $action_ids = explode(",", $row->transaction_action_ids);
      $transactions = $transaction_action_ids_array = $transaction_actions_array = array();
      foreach ($action_ids as $k => $v) {
        $transaction_action_ids_array[$v] = $v;
        $transaction_actions_array[$v] = $actions[$k];
      }
      // Also, put these in order - and insert an empty transaction for any that
      // are not set for the item.
      $i = 1;
      foreach ($required_batch_actions as $required_batch_action) {
        $required_action_id = $required_batch_action['action_id'];
        $exploded_key = (!array_search($required_action_id, $transaction_action_ids_array) === FALSE) ? array_search($required_action_id, $transaction_action_ids_array) : FALSE;
        $transactions[$i] = (!$exploded_key === FALSE) ?
          $transaction_actions_array[$exploded_key] : '';
        $i++;
      }

      $rows[$index]->transactions = $transactions;
    }
    islandora_digital_workflow_calculate_ready_for($rows, $nid);
  }
  return $rows;
}

/**
 * Returns the "Ready For" step for the items.
 *
 * This will inspect the rows of item objects and look up that item's batch to get
 * that batch's "workflow sequence" and it will compare the required steps against
 * the completed steps for that item.  The first uncompleted step is returned.
 *
 * @param array $rows
 *   The rows of items objects.  VARIABLE parameter passed by reference.
 * @param integer $nid
 *   Needed to look up the batch record.
 */
function islandora_digital_workflow_calculate_ready_for(&$rows, $nid) {
  // Need to load the batch_record in order to see which sequence is being used
  // so that the required actions for that sequence can be compared against
  // each items' current transaction_action_ids.
  $batch_record = islandora_digital_workflow_get_batch_record_by_nid($nid);
  $actions = islandora_digital_workflow_get_batch_items_actions($batch_record['workflow_sequence_id']);
  foreach ($rows as $index => $row) {
    $items_action_ids = explode(",", $row->transaction_action_ids);
    $next = '';
    $next_action_id = -1;
    foreach ($actions as $action_id => $action_description) {
      if ($action_id && !$next && (array_search($action_id, $items_action_ids) === FALSE)) {
        $next = $action_description;
        $next_action_id = $action_id;
      }
    }

    if ($next_action_id <> -1) {
      $next_action = islandora_digital_workflow_get_action($next_action_id);
      $rows[$index]->ready_for = '<a title="' . $next_action['action_description'] .
          '" href="/node/' . $nid . '/add_transaction/' . $row->batch_item_id .
          '/' . $next_action_id . '"><div class="add_20">&nbsp;</div></a><div class="' .
          islandora_digital_workflow_glyph_class($next_action['action_name']) . '">&nbsp;</div>';
    }
    else {
      $rows[$index]->ready_for = '';
    }
  }
}

/**
 * This returns the batch item record for $batch_item_id.
 *
 * @return array
 *   Either returns an array of a single batch item record objects.
 */
function islandora_digital_workflow_get_batch_item_record($batch_item_id, $batch_id = NULL) {
  $query = db_select('islandora_digital_workflow_batch_items', 'bi')
      ->fields('bi')
      ->condition('bi.batch_item_id', $batch_item_id);
  if (!is_null($batch_id)) {
    $query->condition('bi.batch_id', $batch_id);
  }
  $results = $query->execute();
  $rows = $results->fetchAll();
  foreach ($rows as $index => $row) {
    $filename_pathinfo = pathinfo($row->filename);
    $rows[$index]->master_filename_basename = $filename_pathinfo['basename'];
    $rows[$index]->batch_id = $row->batch_id;
  }
  return $rows;
}

/**
 * This returns the drush log entries for a given $batch_item_id.
 *
 * @return array
 *   An array of drush log records.
 */
function islandora_digital_workflow_get_drush_log_for_batch_item($batch_item_id) {
  $query = db_select('islandora_digital_workflow_drush_log', 'd')
      ->fields('d')
      ->condition('d.batch_item_id', $batch_item_id);
  $query->join('users', 'u', 'u.uid = d.drupal_uid');
  $query->fields('u', array('name'));
  $results = $query->execute();
  $rows = $results->fetchAll();
  foreach ($rows as $id => $row) {
    $rows[$id]->user_link = l($row->name, 'user/' . $row->drupal_uid . '/edit');
  }
  return $rows;
}

/**
 * Updates an existing batch_item record.
 *
 * @param integer $batch_item_id
 * @param array $update_values
 *   The array of values for the update where the key is the fieldname and the
 * value is the value to use for updating.
 * @param array $original_values
 *   The array of values that represent the record's previous values.
 */
function islandora_digital_workflow_update_batch_item_record($batch_item_id, $update_values, $original_values) {
  db_query("SET NAMES utf8");
  // Get the batch_record related to this item -- needed below in a couple places.
  $batch_item_record_arr = islandora_digital_workflow_get_batch_item_record($batch_item_id);
  if (is_array($batch_item_record_arr)) {
    $batch_item_record_obj = array_pop($batch_item_record_arr);
    $batch_record = islandora_digital_workflow_get_batch_record_by_batch_id($batch_item_record_obj->batch_id);
  }
  else {
    $batch_record = array();
  }

  $results = db_update('islandora_digital_workflow_batch_items')
      ->fields(array(
        'title' => $update_values['title'],
        'identifier' => $update_values['identifier'],
        'filename' => $update_values['filename'],
        'assigned_pid' => (array_key_exists('assigned_pid', $update_values) ? $update_values['assigned_pid'] : ''),
        'islandora_model' => (array_key_exists('islandora_model', $update_values) ? $update_values['islandora_model'] : ''),
        'type_of_resource' => (array_key_exists('type_of_resource', $update_values) ? $update_values['type_of_resource'] : ''),
        'mods' => $update_values['mods'],
        'marc' => (array_key_exists('marc', $update_values) ? $update_values['marc'] : ''),
      ))
      ->condition('batch_item_id', $batch_item_id)
      ->execute();

  // Need to get the item's batch record in order to derive the various file paths
  // for the rename operations.  This will be needed if the item's identifier
  // changes (below) and to pass to the calculate function.
  $current_delivery_item_path = islandora_digital_workflow_batch_path($batch_record, 'delivery') .
        '/' . $original_values->identifier;
  $current_problems_item_path = islandora_digital_workflow_batch_path($batch_record, 'problems') .
        '/' . $original_values->identifier;
  $current_working_item_path = islandora_digital_workflow_batch_path($batch_record, 'working') .
        '/' . $original_values->identifier;
  // If the identifier changes, these variables will be set in that conditional.
  $update_delivery_item_path = $update_problems_item_path = $update_working_item_path = '';

  if ($results) {
    // Compare the filename value to the previous filename value to see
    // if the underlying file needs to be renamed.  Do this FIRST because the item
    // folder name can also change when the item is being updated.
    if ($update_values['filename'] <> $original_values->filename) {
      if (file_exists($update_values['filename'])) {
        drupal_set_message(t('A file already existed with the updated filename, so nothing needed to be renamed.'), 'message_info');
      }
      if (file_exists($original_values->filename)) {
        $renamed_file = rename($original_values->filename, $update_values['filename']);
        if ($renamed_file) {
          drupal_set_message(t('The underlying file has been renamed to match the value that was supplied.'), 'status');
        }
        else {
          drupal_set_message(t('There was a problem renaming the underlying file from "') .
              $original_values->filename . '" to "' . $update_values['filename'] . t('".  It could be a file permissions issue.'), 'error');
        }
      }
    }

    // Compare the underlying MARC file to the value that is passed by $update_values --
    // and overwrite that file if needed.
    if (array_key_exists('marc', $update_values) && ($update_values['marc'] <> $original_values->marc)) {
      $calculated_marc_filename = islandora_digital_workflow_filename_sanitize($original_values->filename, '.xml');
      $bytes_written = ($calculated_marc_filename) ?
          file_put_contents($calculated_marc_filename, $update_values['marc']) : 0;
      if ($bytes_written <> strlen($update_values['marc'])) {
        drupal_set_message(t('There was a problem saving the updated MARC value to the file system at "') .
          $calculated_marc_filename . t('".  It may be a file permissions issue.'), 'error');
      }
      islandora_digital_workflow_insert_transactions_record(IDW_ACTION_ADD_MARC_RECORD, NULL, $batch_item_id);
    }
    // Compare the underlying MODS file to the value that is passed by $update_values --
    // and overwrite that file if needed.
    if ($update_values['mods'] <> $original_values->mods) {
      $calculated_mods_filename = islandora_digital_workflow_filename_sanitize($original_values->filename, '.xml');
      $bytes_written = ($calculated_mods_filename) ?
          file_put_contents($calculated_mods_filename, $update_values['mods']) : 0;
      if ($bytes_written <> strlen($update_values['mods'])) {
        drupal_set_message(t('There was a problem saving the updated MODS value to the file system at "') .
          $calculated_mods_filename . t('".  It may be a file permissions issue.'), 'error');
      }
      islandora_digital_workflow_insert_transactions_record(IDW_ACTION_MODS_RECORD_UPDATED, NULL, $batch_item_id);
    }

    // Compare the item identifier to see if it has changed... if so, the system
    // will need to rename those folders (delivery, (and if they exists) problems
    // and working folder.
    if ($update_values['identifier'] <> $original_values->identifier) {
      $replace_to = '/' . $update_values['identifier'] . '/';
      $replace_from = '/' . $original_values->identifier . '/';
      $update_delivery_item_path = str_replace($replace_from, $replace_to, $current_delivery_item_path);
      $update_problems_item_path = str_replace($replace_from, $replace_to, $current_problems_item_path);
      $update_working_item_path = str_replace($replace_from, $replace_to, $current_working_item_path);
      // Only try to rename those item folders that exist
      if (file_exists($current_delivery_item_path)) {
        if (!rename($current_delivery_item_path, $update_delivery_item_path) ) {
          drupal_set_message(t('There was a problem renaming the "delivery folder" ' .
              'for the current item "@item_ident".  Check file permissions on the ' .
              'paths:<ul><li>@path</li><li>@to_path</li></ul>.', array(
                  '@path' => $current_delivery_item_path,
                  '@to_path' => $update_delivery_item_path,
                  '@item_ident' => $original_values->identifier)), 'error');
        }
      }
      if (file_exists($current_problems_item_path)) {
        if (!rename($current_problems_item_path, $update_problems_item_path) ) {
          drupal_set_message(t('There was a problem renaming the "problems folder" ' .
              'for the current item "@item_ident".  Check file permissions on the ' .
              'paths:<ul><li>@path</li><li>@to_path</li></ul>.', array(
                  '@path' => $current_problems_item_path,
                  '@to_path' => $update_problems_item_path,
                  '@item_ident' => $original_values->identifier)), 'error');
        }
      }
      if (file_exists($current_working_item_path)) {
        if (!rename($current_working_item_path, $update_working_item_path) ) {
          drupal_set_message(t('There was a problem renaming the "working folder" ' .
              'for the current item "@item_ident".  Check file permissions on the ' .
              'paths:<ul><li>@path</li><li>@to_path</li></ul>.', array(
                  '@path' => $current_working_item_path,
                  '@to_path' => $update_working_item_path,
                  '@item_ident' => $original_values->identifier)), 'error');
        }
      }
    }
  }

  return $results;
}

/**
 * Calculates the progress and file count for the given batch item.
 *
 * @param integer $batch_item_id
 *   The batch_item_id of the item for which to calculate progress,
 * @param array $batch_record
 *   The batch record array that the item belongs to.
 */
function islandora_digital_workflow_calculate_item_progress_filedetails($batch_item_id, $batch_record)  {
  if (!is_null($batch_item_id)) {
    $item_record_transactions = islandora_digital_workflow_get_batch_item_transactions($batch_item_id, $batch_record['nid'], $batch_record['batch_id']);
    $required_batch_actions = islandora_digital_workflow_get_required_batch_actions($batch_record['batch_id']);
    $item_record_results = islandora_digital_workflow_get_batch_item_record($batch_item_id);
    $batch_item_obj = array_pop($item_record_results);
    // Start with the easiest assumption... the item is new:
    $progress = 'New';

    // Are there any actions for this item?  If so, then consider it "In Progress".
    if (is_array($item_record_transactions) && count($item_record_transactions) > 1) {
      $progress = 'In Progress';
    }
    // Now, if it is not new, we must see if it is ready for ingest:
    $item_ready_for_ingest = islandora_digital_workflow_item_ingest_prerequisites_all_done($item_record_transactions, $required_batch_actions);
    $progress = ($item_ready_for_ingest) ? 'Prepared' : $progress;

    // After it is prepared, it could be ingested now.
    $item_ingested = islandora_digital_workflow_item_ingested($batch_item_obj, $batch_record, $item_record_transactions);
    $progress = ($item_ingested) ? 'Ingested' : $progress;

    // Now if it is ready for ingest, we should check to see whether or not it has
    // been published.
    $item_done = FALSE;
    foreach ($item_record_transactions as $item_transaction_object) {
      $item_done |= FALSE; //($item_transaction_object->action_id  == IDW_PUBLISH_OBJECT);
    }

    // There may be the action_id that the object is ingested / done, but we may want to
    // check whether or not the islandora object behind that item ACTUALLY does exist.
    if ($item_done) {
      $namespace = islandora_digital_workflow_get_ingest_namespace($batch_record);
      $pid = ($batch_item_obj->assigned_pid <> '') ? $batch_item_obj->assigned_pid : $namespace . $batch_item_obj->identifier;
      $islandora_object = islandora_object_load($pid);
      $item_done |= (is_object($islandora_object));
    }
    $progress = ($item_done) ? 'Completed' : $progress;
    if ($progress <> $batch_item_obj->progress) {
      $set_fields = array('progress' => $progress);
      db_update('islandora_digital_workflow_batch_items')
        ->condition('batch_item_id', $batch_item_id)
        ->fields($set_fields)
        ->execute();
    }
    islandora_digital_workflow_recalc_batch_files($batch_record, $output_contents, $output_footer, $batch_item_id);
    islandora_digital_workflow_update_batch_record_progress($batch_record);
  }
}

/**
 * This will analyze a mysql table to return stats on the fields.
 *
 * This will perform mysql analysis of the underlying table to return the
 * fieldname for primary key, which fields are numeric, which are not, and
 * finally all field names (both numeric and non-numeric fields).
 *
 * @todo this is still not used.  Search the project later and see if this
 *       can be removed.
 * 
 * @param string $tablename
 *   The name of the MySQL table to inspect.
 * @return array
 *   'primary_key_field' string the name of the primary key for the table
 *   'numeric_fields' array names of numeric fields
 *   'num_numeric_fields' array names of non-numeric fields
 *   'all_fields' arry names of all fields
 */
function islandora_digital_workflow_get_tablefield_stats($tablename) {
  $query = db_select($tablename, 'b')
      ->fields('b')
      ->range(0,1);
  $results = $query->execute();
  $result = $results->fetchAssoc();
  $primary_key_field = '';
  $fields = $integer_fieldnames = $non_integer_fieldnames = array();

  $fields = array_keys($result);
  if (count($fields) < 1) {
    return array(NULL, array(), array());
  }

  foreach ($fields as $field) {
    $results = db_query("SHOW FIELDS FROM `" . $tablename . "` WHERE Field = '" . $field . "'");

    $row = $results->fetchAssoc();
    if (!$primary_key_field && $row['Key'] == 'PRI') {
      $primary_key_field = $field;
    }
    if (substr($row['Type'], 0, 4) == "int(" || substr($row['Type'], 0, 7) == 'TINYINT') {
      $integer_fieldnames[] = $field;
    } else {
      $non_integer_fieldnames[] = $field;
    }
  }
  return array(
      'primary_key_field' => $primary_key_field,
      'numeric_fields' => $integer_fieldnames,
      'num_numeric_fields' => $non_integer_fieldnames,
      'all_fields' => $fields
    );
}

/**
 * Returns all batch actions that pertain to batch records rather than
 * batch_items records.
 *
 * @return mixed
 *   Either returns an array of actions record objects or NULL if it cannot be found.
 */
function islandora_digital_workflow_get_batch_actions() {
  $query = db_select('islandora_digital_workflow_actions', 'ba')
      ->fields('ba')
      ->condition('ba.is_batch_action', 1);
  $results = $query->execute();
  $rows = $results->fetchAll();
  $options = array('' => 'Select an action');
  foreach ($rows as $i => $row) {
    $options[$row->action_id] = $row->description;
  }
  return $options;
}

/**
 * Returns all batch actions that pertain to batch_items records rather than
 * batch records.
 *
 * @param integer $workflow_sequence_id
 *   The workflow sequence id value for the batch in question.
 * @param boolean $description_only
 *   Optional - when set to FALSE, the returned values will be an array that
 * contains all field data for the actions, else the return value is just
 * the descriptions for each action.
 * @return mixed
 *   Either returns an array of actions record objects or NULL if it cannot be
 * found.  The return array structure depends on the optional $description_only
 * parameter -- when FALSE, the return for each action_id contains an array
 * of the entire sequence record.
 */
function islandora_digital_workflow_get_batch_items_actions($workflow_sequence_id = 0, $description_only = TRUE) {
  // If no model is provided, display the sequence_action for the default
  // batch_item record.
  $query = db_select('islandora_digital_workflow_actions', 'ba')
      ->fields('ba')
      ->condition('ba.is_batch_action', 0);
  $query->join('islandora_digital_workflow_sequence_actions', 'sa', 'sa.action_id = ba.action_id');
  $query->addField('sa', 'order', 'sequence_order');
  $query->orderBy('sa.order');
  $query->join('islandora_digital_workflow_sequence', 's', 's.workflow_sequence_id = sa.workflow_sequence_id');
  $query->fields('s', array('name'));
  $query->join('islandora_digital_workflow_model_sequence', 'm', 'm.workflow_sequence_id = s.workflow_sequence_id');
  if ($workflow_sequence_id) {
    $query->condition('sa.workflow_sequence_id', $workflow_sequence_id);
  }
  $query->groupBy('sa.action_id');

  $results = $query->execute();
  $rows = $results->fetchAll();

  $options = array('' => 'Select an action');
  foreach ($rows as $i => $row) {
    $options[$row->action_id] = (($description_only) ? $row->description :
        (array)$row);
  }
  return $options;
}

/**
 * Returns the list of workflow sequence name values.
 *
 * @return array
 *   An array of workflow sequence records' name values.
 */
function islandora_digital_workflow_get_workflow_sequences() {
  $query = db_select('islandora_digital_workflow_sequence', 's')
      ->fields('s')
      ->orderBy('s.workflow_sequence_id');
  $query->addExpression('(SELECT MAX(timestamp) FROM islandora_digital_workflow_sequence_actions WHERE workflow_sequence_id = s.workflow_sequence_id)', 'max_timestamp');

  $results = $query->execute();

  $rows = $results->fetchAll();
  $return_array = array();
  foreach ($rows as $i => $row) {
    $row->how_long_ago = islandora_digital_workflow_timeago_from_timestamp($row->max_timestamp);
    $return_array[$row->workflow_sequence_id] = (array) $row;
  }

  return $return_array;
}

/**
 * To get a set of workflow item-level actions as options for some forms.
 *
 *  @todo rewrite to use a db_select -- the LEFT OUTER JOIN can be accomplished using:
 *        $query->addJoin('LEFT', 'taxonomy_term_data', 'td', 'stage_taxonomy_tid = td.tid');
 * 
 * @param boolean $display_reserved_actions
 *   whether or not to return only the Reserved actions.
 * @return array
 *   the set of options for possible workflow actions.
 */
function islandora_digital_workflow_get_item_action_options($display_reserved_actions = TRUE) {
  // This has to LEFT OUTER JOIN to the taxonomy_term_data table TWICE so that
  // it can get the term name for the "Add Stage" as well as for the "Remove Stage".
  $query = db_query('SELECT a.*, td_add.name as `add_term_name`, td_rm.name as `rm_term_name` ' .
          'FROM islandora_digital_workflow_actions a ' .
          'LEFT OUTER JOIN taxonomy_term_data td_add ON td_add.tid = stage_taxonomy_tid ' .
          'LEFT OUTER JOIN taxonomy_term_data td_rm ON td_rm.tid = stage_taxonomy_tid_rm ' .
          'WHERE a.is_batch_action = 0' .
      (!$display_reserved_actions ? ' AND a.description NOT LIKE \'Reserved %\'' : '') );
  $rows = $query->fetchAll();
  $return_array = array();
  foreach ($rows as $i => $row_obj) {
    $row_obj->is_batch_action = 0;
    $row_obj->action_name = $row_obj->name;
    $row_obj->action_description = $row_obj->description;
    $return_array[$row_obj->action_id] = (array) $row_obj;
  }
  return $return_array;
}

/**
 * To get a set of workflow batch actions as options for some forms.
 *
 * @todo rewrite to use a db_select -- the LEFT OUTER JOIN can be accomplished using:
 *       $query->addJoin('LEFT', 'taxonomy_term_data', 'td', 'stage_taxonomy_tid = td.tid');
 *
 * @param boolean $display_reserved_actions
 *
 * @return array
 *   the set of options for possible workflow actions.
 */
function islandora_digital_workflow_get_batch_action_options($display_reserved_actions = TRUE) {
  $query = db_query('SELECT a.*, td_add.name as `add_term_name`, td_rm.name as `rm_term_name` ' .
          'FROM islandora_digital_workflow_actions a ' .
          'LEFT OUTER JOIN taxonomy_term_data td_add ON td_add.tid = a.stage_taxonomy_tid ' .
          'LEFT OUTER JOIN taxonomy_term_data td_rm ON td_rm.tid = a.stage_taxonomy_tid_rm ' .
          'WHERE a.is_batch_action = 1' .
      (!$display_reserved_actions ? ' AND a.description NOT LIKE \'Reserved %\'' : '') );
  $rows = $query->fetchAll();
  $return_array = array();
  foreach ($rows as $i => $row_obj) {
    $row_obj->action_name = $row_obj->name;
    $row_obj->action_description = $row_obj->description;
    $row_obj->is_batch_action = 1;
    $return_array[$row_obj->action_id] = (array) $row_obj;
  }
  return $return_array;
}

/**
 * Returns the list of workflow sequence name values.
 *
 * These values are grouped by workflow_sequence_id and action_id - and ordered
 * by each sequence's order field.
 *
 * @return array
 *   An array of workflow sequence records' name values.
 */
function islandora_digital_workflow_get_workflow_sequence_actions() {
  $query = db_select('islandora_digital_workflow_sequence_actions', 'sa')
      ->fields('sa')
      ->orderBy('sa.workflow_sequence_id')
      ->orderBy('sa.order');

  $query->join('islandora_digital_workflow_actions', 'a', 'a.action_id = sa.action_id');
  $query->addField('a', 'action_id', 'action_id');
  $query->addField('a', 'name', 'action_name');
  $query->addField('a', 'is_batch_action', 'is_batch_action');
  $query->addField('a', 'description', 'action_description');
  $query->addField('a', 'stage_taxonomy_tid', 'stage_taxonomy_tid');
  $query->addField('a', 'is_triggered_by_single_item', 'is_triggered_by_single_item');
  $query->join('islandora_digital_workflow_sequence', 's', 's.workflow_sequence_id = sa.workflow_sequence_id');
  $query->addField('s', 'name', 'sequence_name');
  $query->addField('s', 'is_mixed', 'is_mixed');
  $query->addField('s', 'description', 'sequence_description');
  $query->addJoin('LEFT', 'taxonomy_term_data', 'td', 'a.stage_taxonomy_tid = td.tid');
  $query->addField('td', 'name', 'add_term_name');

  $results = $query->execute();
  $rows = $results->fetchAll();
  $return_array = array();
  foreach ($rows as $i => $row) {
    $return_array[$row->workflow_sequence_id][] = (array) $row;
  }
  return $return_array;
}

/**
 * Returns all model to workflow sequences mappings.
 *
 * @return array
 *   An array of workflow sequence records' name values.
 */
function islandora_digital_workflow_get_workflow_model_sequences() {
  $query = db_select('islandora_digital_workflow_model_sequence', 'ms')
      ->fields('ms')
      ->orderBy('ms.workflow_sequence_id');
  $results = $query->execute();
  $rows = $results->fetchAll();
  $return_array = array();
  foreach ($rows as $i => $row) {
    $return_array[$row->workflow_sequence_id][$row->islandora_model] = $row->islandora_model;
  }
  return $return_array;
}

/**
 * Returns the matching islandora_digital_workflow_batch_items batch_item_id value.
 *
 * @param string $identifier
 * $use__batch_id string 
 *   Optional value of a batch_id to search within.
 * $like_identifier boolean
 *   Optional parameter to use a LIKE '%identifier%' MySQL condition, else
 * the condition is an exact match.
 * @return stdObject
 *   The batch_item record object.
 */
function islandora_digital_workflow_get_batch_item_id_by_identifier($identifier, $use__batch_id = NULL, $like_identifier = TRUE) {
  // If this has been passed a $pid value, just use whatever comes after the colon.
  if (strstr($identifier, ':')) {
    @list($namespace, $identifier) = explode(":", $identifier);
  }
  if ($use__batch_id) {
    $query = db_select('islandora_digital_workflow_batch_items', 'bi')
      ->fields('bi', array('batch_item_id'))
      ->condition('batch_id', $use__batch_id);
  }
  else {
    $query = db_select('islandora_digital_workflow_batch_items', 'bi')
          ->fields('bi', array('batch_item_id'));
  }
  if ($like_identifier) {
    $query->condition('identifier', '%' . db_like($identifier), 'LIKE');
  }
  else {
    $query->condition('identifier', $identifier);
  }
  $results = $query->execute();
  $row = $results->fetchAssoc();
  return (is_array($row)) ? array_pop($row) : $row;
}

/**
 * This will return the batch_item_id values of records that have a given
 * `assigned_pid` value.
 *
 * @param string $assigned_pid
 *   The `assigned_pid` value of an islandora_digital_workflow_batch_item record.
 * This should be distinct based on assumptions, but there could be more than
 * one record with the same final `assigned_pid` value, and this should return
 * all of the references because this is only called from the "datastream ingested"
 * hook which is only trying to set a transaction for that item/s.
 * @return integer
 *   The batch_item_id value of the record that matches the "assigned_pid" value
 * of a record.
 */
function islandora_digital_workflow_get_batch_item_ids_by_assigned_pid($assigned_pid) {
  // Take the $assigned_pid value and strip off the namespace to get the identifier.
  if (strstr($assigned_pid, ":")) {
    @list($namespace, $identifier) = explode(":", $assigned_pid);
  }
  else {
    $identifier = $assigned_pid;
  }
  $query = db_select('islandora_digital_workflow_batch_items', 'bi')
    ->fields('bi', array('batch_item_id'))
    ->condition('identifier', $identifier);
  $results = $query->execute();
  $rows = $results->fetchAll();
  $batch_item_ids = array();
  foreach ($rows as $row) {
    $batch_item_ids[] = $row->batch_item_id;
  }
  return $batch_item_ids;
}

/**
 * Insert a record to islandora_digital_workflow_batch_items.
 *
 * @param array $fields
 *   Associative array of fieldnames and their values.
 * @return integer
 */
function islandora_digital_workflow_insert_batch_item($fields) {
  db_query("SET NAMES utf8");
  return db_insert('islandora_digital_workflow_batch_items')
      ->fields($fields)
      ->execute();
}

/**
 * Inserts an action transaction for a batch_item record by identifier.
 *
 * This code needs to handle several special actions as well.
 *   1. For any actions that are configured to trigger a stage change, it
 * will call islandora_digital_workflow_check_trigger_stage_change.
 *   2. When the action is a "Problem", this will need to set the problem record
 * by calling islandora_digital_workflow_set_problem_record.
 *   3. When the action is "Scanned" (IDW_ACTION_SCAN), the code needs to copy and
 * possibly delete the delivery file.
 *   4. When the action is "Publish the item" (IDW_PUBLISH_OBJECT), the code
 * needs to:
 *      a. remove any relationship to "_review" collections
 *      b. add relalationship/s to collection/s and site/s.
 *
 * @param type $action_id
 *   id value correponding to the action_id to insert.
 * @param integer $batch_id
 *   batch_id value corresponding to the batch for the transaction.
 * @param integer $batch_item_id
 *   batch_item_id value corresponding to the batch_item for the transaction.
 * @param integer $alternative_uid
 *   when provided, this will override the current-logged-in user's uid value.
 */
function islandora_digital_workflow_insert_transactions_record($action_id, $batch_id = NULL, $batch_item_id = NULL, $alternative_uid = NULL, $problem_notes = '') {
  // Just in case the calling code does not include the utilities.inc, do it again.
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  global $user;
  if (!is_null($batch_id) || !is_null($batch_item_id)) {
    $transaction_id = db_insert('islandora_digital_workflow_transactions')
        ->fields(array(
          'action_id' => $action_id,
          'batch_id' => $batch_id,
          'batch_item_id' => $batch_item_id,
          'drupal_uid' => ($alternative_uid) ? $alternative_uid : $user->uid,
        ))
        ->execute();
    // check to see if this specific change will trigger any transaction Stage changes
    islandora_digital_workflow_check_trigger_stage_change(NULL, array(
        'action_id' => $action_id,
        'batch_id' => $batch_id,
        'batch_item_id' => $batch_item_id,
      ));
  
    if ($action_id == IDW_ACTION_PROBLEM) {
      islandora_digital_workflow_set_problem_record($transaction_id, ($alternative_uid) ? $alternative_uid : $user->uid, $problem_notes);
    } elseif ($action_id == IDW_ACTION_SCAN && !is_null($batch_item_id)) {
      islandora_digital_workflow_copy_delivery_files($batch_item_id, FALSE, NULL);
    } elseif ($action_id == IDW_PUBLISH_OBJECT) {
      islandora_digital_workflow_publish_object($batch_item_id);
    }

    if (!is_null($batch_item_id)) {
      $item_record_results = islandora_digital_workflow_get_batch_item_record($batch_item_id);
      $batch_item_obj = array_pop($item_record_results);
      $batch_record = islandora_digital_workflow_get_batch_record_by_batch_id(NULL, $batch_item_id);
      // Before exiting, this can cause the "progress" of an item to change
      islandora_digital_workflow_calculate_item_progress_filedetails($batch_item_id, $batch_record);
    }

    return $transaction_id;
  }
}

/**
 * Removes a transaction record.
 *
 * @param type $action_id
 *   id value correponding to the action_id to deletion.
 * @param integer $batch_id
 *   batch_id value corresponding to the batch of the transaction.
 * @param integer $batch_item_id
 *   batch_item_id value corresponding to the batch_item of the transaction.
 */
function islandora_digital_workflow_remove_transactions_record($action_id, $batch_id = NULL, $batch_item_id = NULL) {
  return db_delete('islandora_digital_workflow_transactions')
      ->condition('action_id', $action_id)
      ->condition('batch_id', $batch_id)
      ->condition('batch_item_id', $batch_item_id)
      ->execute();
}

/**
 * Returns each sequence that relates to a given Islandora model.
 *
 * @param string $model_name
 *   The Islandora model name.
 * @return array
 *   The sequences that apply to the given Islandora model.
 */
function islandora_digital_workflow_get_workflow_sequences_for_model($model_name) {
  $results = db_query('SELECT s.workflow_sequence_id, s.`name` ' .
      'FROM {islandora_digital_workflow_sequence} s ' .
      'JOIN {islandora_digital_workflow_model_sequence} ms ON ' .
      '(ms.workflow_sequence_id = s.workflow_sequence_id) WHERE islandora_model ' .
      '= :islandora_model', array(
          ':islandora_model' => $model_name,
      ))->fetchAll();
  $sequences = array('' => 'Select workflow sequence');
  foreach ($results as $row) {
    $sequences[$row->workflow_sequence_id] = $row->name;
  }
  return $sequences;
}

/**
 * Gets the required batch actions for a batch or node.
 *
 * This can use a batch_id identifier OR a node's nid value to find the actions.
 * 
 * @param integer $batch_id
 *   Batch identifier (matches islandora_digital_workflow_batch.batch_id).
 * @param integer $nid
 *   Node identifier - matches a Drupal node record nid value.
 * @return array
 *   array of actions - related to the batch as specified by nid or batch_id.
 */
function islandora_digital_workflow_get_required_batch_actions($batch_id = NULL, $nid = NULL) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  $query = db_select('islandora_digital_workflow_batch', 'b');
  $query->fields('b', array('batch_id', 'workflow_sequence_id'));
  $query->join('islandora_digital_workflow_sequence', 'ws', 'b.workflow_sequence_id = ws.workflow_sequence_id');
  $query->fields('ws', array('name', 'workflow_sequence_id', 'is_mixed'));
  $query->join('islandora_digital_workflow_sequence_actions', 'wsa', 'ws.workflow_sequence_id = wsa.workflow_sequence_id');
  $query->fields('wsa', array('is_required', 'is_ingest_prerequisite', 'islandora_model'));
  $query->addField('wsa', 'order', 'action_order');
  $query->join('islandora_digital_workflow_actions', 'wa', 'wsa.action_id = wa.action_id');
  $query->fields('wa', array('description', 'action_id'));
  $query->addField('wa', 'name', 'action_name');
  if ($batch_id) {
    $query->condition('b.batch_id', $batch_id);
  }
  elseif ($nid) {
    $query->condition('b.nid', $nid);
  }
  $query->orderBy('wsa.order', 'ASC');
  $results = $query->execute();
  $rows = $results->fetchAll();
  $return_rows = array();
  // To have explicit control over the returned array results...
  foreach ($rows as $row) {
    $return_rows[] = array(
        'batch_id' => $row->batch_id,
        'batch_action_name' => $row->action_name,
        'batch_action_description' => $row->description,
        'is_required' => $row->is_required,
        'is_ingest_prerequisite' => $row->is_ingest_prerequisite,
        'islandora_model' => $row->islandora_model,
        'sequence_name' => $row->name,
        'is_mixed' => $row->is_mixed,
        'workflow_sequence_id' => $row->workflow_sequence_id,
        'action_id' => $row->action_id,
        'class_name' => islandora_digital_workflow_glyph_class($row->action_name),
    );
  }
  return $return_rows;
}

/**
 * Gets an `islandora_digital_workflow_actions` record.
 *
 * @param integer $action_id
 *   The `action_id` (primary key) for an action record.
 * @return array
 *   The action record as an array.
 */
function islandora_digital_workflow_get_action($action_id) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  $query = db_select('islandora_digital_workflow_actions', 'wa');
  $query->fields('wa');
  $query->addField('wa', 'name', 'action_name');
  $query->addField('wa', 'is_system_action', 'is_system_action');
  $query->addField('wa', 'description', 'action_description');

  $query->condition('wa.action_id', $action_id);
  $query->addJoin('LEFT', 'taxonomy_term_data', 'td_add', 'wa.stage_taxonomy_tid = td_add.tid');
  $query->fields('td_add');
  $query->addField('td_add', 'name', 'add_term_name');
  $query->addJoin('LEFT', 'taxonomy_term_data', 'td_rm', 'wa.stage_taxonomy_tid_rm = td_rm.tid');
  $query->addField('td_rm', 'name', 'rm_term_name');
  $results = $query->execute();
  $rows = $results->fetchAll();
  $return = array();
  // To have explicit control over the returned array results...
  foreach ($rows as $row) {
    $return = array(
        'action_id' => $action_id,
        'action_name' => $row->action_name,
        'action_description' => $row->action_description,
        'extra_info' => $row->extra_info,
        'glyph' => $row->glyph,
        'is_batch_action' => $row->is_batch_action,
        'is_system_action' => $row->is_system_action,
        'is_triggered_by_single_item' => $row->is_triggered_by_single_item,
        'stage_taxonomy_tid' => $row->stage_taxonomy_tid,
        'is_triggered_by_single_item_rm' => $row->is_triggered_by_single_item_rm,
        'stage_taxonomy_tid_rm' => $row->stage_taxonomy_tid_rm,
        'add_term_name' => $row->add_term_name,
        'rm_term_name' => $row->rm_term_name,
        'class_name' => islandora_digital_workflow_glyph_class($row->action_name),
    );
  }
  return $return;
}

/**
 * Returns all currently configured workflow actions.
 *
 * @return array
 *   An array of all workflow actions.
 */
function islandora_digital_workflow_get_all_actions() {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  $query = db_select('islandora_digital_workflow_actions', 'wa');
  $query->fields('wa');
  $query->addField('wa', 'description', 'action_description');
  $query->addField('wa', 'name', 'action_name');
  $results = $query->execute();
  $rows = $results->fetchAll();
  $return = array();
  // To have explicit control over the returned array results...
  foreach ($rows as $row) {
    $return[$row->action_id] = array(
        // be careful, in other places where the actions join to the taxonomy
        // table, the fields "name" and "description" can point to the taxonomy
        // terms table.
        'name' => $row->name,
        'description' => $row->description,
        'action_name' => $row->action_name,
        'action_description' => $row->action_description,
        'glyph' => $row->glyph,
        'is_batch_action' => $row->is_batch_action,
        'class_name' => islandora_digital_workflow_glyph_class($row->action_name),
    );
  }
  return $return;
}

/**
 * This will return a single batch_item record.
 *
 * @param integer $batch_item_id
 *   The batch_item_id identifier (matches
 * islandora_digital_workflow_batch_item.batch_item_id).
 * @return array
 *   The array item is a stdObject object.
 */
function islandora_digital_workflow_get_batch_item($batch_item_id) {
  $query = db_select('islandora_digital_workflow_batch_items', 'bi');
  $query->fields('bi');
  $query->condition('bi.batch_item_id', $batch_item_id);
  $results = $query->execute();
  $rows = $results->fetchAll();
  $return = array();
  foreach ($rows as $row) {
    $return = $row;
  }
  return $return;
}

/**
 * This returns the next available "action_id" value -- for creating new
 * actions.
 *
 * @return integer
 *   The number to use for the next islandora_digital_workflow_action record.
 */
function islandora_digital_workflow_get_free_action_id() {
  // Run a query to get the maximum value of action_id that is under the range
  // limit defined by "item_action" vs. "batch_action".
  $sql = 'SELECT (MAX(action_id) + 1) as `next_action_id` FROM islandora_digital_workflow_actions';
  $results = db_query($sql);
  $rows = $results->fetchAll();
  $rows = array_pop($rows);
  return $rows->next_action_id;
}

/**
 * This will update the islandora_digital_workflow_batch_items record with
 * the values in the $item_data array.
 *
 * @param array $batch_record
 *   Only used for the output to display the batch record from which the item came.
 * @param array $item_data
 *   This key-value pair (with the key being each item's identifier) is used to
 * set field data on the islandora_digital_workflow_batch_items table for the item.
 * @return string
 *   HTML markup from the item updating.
 */
function islandora_digital_workflow_update_item_file_data($batch_record, $item_data) {
  $batch_record = (array) $batch_record;
  $markup_arr = array();
  foreach ($item_data as $item_pid => $identifer_data) {
    @list($item_namespace, $item_identifier) = explode(":", $item_pid);
    $total_file_size = 0;
    $files = array();
    foreach ($identifer_data as $item_file) {
      $total_file_size += $item_file['size'];
      $files[] = $item_file['file'];
    }
    $total_file_count = count($identifer_data);
    $set_fields = array(
        'file_count' => $total_file_count,
        'file_size' => $total_file_size);
    db_update('islandora_digital_workflow_batch_items')
      ->condition('batch_id', $batch_record['batch_id'])
      ->condition('identifier', $item_identifier)
      ->fields($set_fields)
      ->execute();
    $markup_arr[] = '<b><span style="color:#888">' . $batch_record['batch_name'] . '/</span>' . $item_identifier . '</b><br>';
    $markup_arr[] = '<pre class="small_lt_text">';
    $markup_arr[] = "       <b>Size</b>: " . $total_file_size . " bytes\n";
    $markup_arr[] = "      <b>Count</b>: " . $total_file_count . "\n";
    $markup_arr[] = "      <b>Files</b>: " . implode(", ", ($files));
    $markup_arr[] = "</pre>";
  }
  return implode("\n", $markup_arr);
}

/**
 * Inserts the record into the forena_repositories table.
 */
function islandora_digital_workflow_create_islandora_digital_workflow_sql_configuration() {
  $private_path = rtrim(variable_get('file_private_path', ''), '/') . '/';
  if (!$private_path) {
    drupal_set_message(t('The Drupal | Media | File System "Private file system ' .
        'path" is not configured.  This must be configured and the folder must' .
        'be writable by Drupal and not accessible over the web.'), 'error');
    return FALSE;
  }
  $config_array = array(
    'source' => 'user',
    'data provider' => 'FrxDrupal',
    'database' => 'default',
    'access callback' => 'user_access',
    'user callback' => 'forena_current_user_id',
    'path' => $private_path . 'islandora_digital_workflow_sql',
    'debug' => 0
  );
  // Make the value serialized like:  //  a:7:{s:6:"source";s:4:"user";s:13:"...
  $config_str = serialize($config_array);
  db_insert('forena_repositories')
    ->fields(array(
        'repository' => 'islandora_digital_workflow_sql',
        'title' => 'Islandora Workflow Reports',
        'enabled' => 1,
        'config' => $config_str,
    ))
    ->execute();
}

/**
 * To set the problem_resolved_timestamp for the problem records related
 * to a given batch_id.
 *
 * @param integer $batch_id
 *   The batch_id of the batch being sync'd from the delivery directory.
 */
function islandora_digital_workflow_unset_problems_records($batch_id) {
  $sql = 'UPDATE islandora_digital_workflow_problem_items 
   SET problem_resolved_timestamp = now()
   WHERE transaction_id IN 
    (SELECT transaction_id 
     FROM islandora_digital_workflow_transactions 
     WHERE batch_id = ' . $batch_id . '
     UNION 
     SELECT transaction_id 
     FROM islandora_digital_workflow_transactions 
     WHERE batch_item_id IN 
     (SELECT batch_item_id 
      FROM islandora_digital_workflow_batch_items 
      WHERE batch_id = ' . $batch_id . '
     )
    )';
  db_query($sql);
}

/**
 * This will run a query to find the batch_item_id values for a given set of
 * identifiers.
 *
 * @param type $identifiers
 *   Array of identifiers for which to return the batch_item_id values.
 * @return type
 *   Array of batch_item_id values.
 */
function islandora_digital_workflow_get_batch_item_ids($identifiers) {
  $query = db_select('islandora_digital_workflow_batch_items', 'bi')
      ->fields('bi', array('batch_item_id'))
      ->condition('bi.identifier', $identifiers, 'IN');
  $results = $query->execute();
  $rows = $results->fetchAll();
  $return_array = array();
  foreach ($rows as $i => $row) {
    $return_array[] = $row->batch_item_id;
  }
  return $return_array;
}

/**
 * Helper function to make a "processing set" that holds the mode of operation
 * and the batch_item_id values (see includes/utilities.inc function
 * `islandora_digital_workflow_make_process_set_records`).
 *
 * @param string $mode
 *   The select box value for the intended processing mode corresponding to the
 * "Process Identifiers" form (includes/process.form.inc).
 * @param array $item_ids
 *   An array of batch_item_id values that correspond to the identifiers needed
 * for affecting items with the selected mode.
 * @return integer
 *   The identifier value of the created `islandora_digital_workflow_process_set`
 * record.
 */
function islandora_digital_workflow_make_process_set_records($mode, $item_ids) {
  if (count($item_ids) > 0) {
    $process_set_id = db_insert('islandora_digital_workflow_process_set')
        ->fields(array(
          'set_mode' => $mode,
        ))
        ->execute();
    // Insert the islandora_digital_workflow_process_set_items records.
    foreach ($item_ids as $item_id) {
      db_insert('islandora_digital_workflow_process_set_items')
          ->fields(array(
            'process_set_id' => $process_set_id,
            'batch_item_id' => $item_id,
          ))
          ->execute();
    }

    // Delete old process_set and islandora_digital_workflow_process_set_items
    // records (3 days = 259200 seconds)
    $query = db_select('islandora_digital_workflow_process_set', 'p')
        ->fields('p', array('process_set_id'))
        ->condition('p.timestamp', 'NOW() - 259200', '<');
    $results = $query->execute();
    $rows = $results->fetchAssoc();
    foreach ($rows as $del_process_set_id) {
      db_delete('islandora_digital_workflow_process_set_items')
        ->condition('process_set_id', $del_process_set_id)
        ->execute();
      db_delete('islandora_digital_workflow_process_set')
        ->condition('process_set_id', $del_process_set_id)
        ->execute();
    }
    return $process_set_id;
  }
}

/**
 * This will run a query to find and return the item_ids affected by a given
 * $process_set_id value.
 *
 * @param ingeger $process_set_id
 *   The identifier value of an `islandora_digital_workflow_process_set` record.
 * This variable is passed by reference.
 * @param type $mode
 *   The select box value for the intended processing mode corresponding to the
 * "Process Identifiers" form (includes/process.form.inc).  This variable is passed
 * by reference.
 * @param array $item_identifiers
 *   Array of batch_item_id values.
 */
function islandora_digital_workflow_get_process_set_records($process_set_id, &$mode, &$item_identifiers) {
  $query = db_select('islandora_digital_workflow_process_set', 'p')
      ->fields('p', array('set_mode'))
      ->condition('p.process_set_id', $process_set_id)
      ->range(0,1);

  $results = $query->execute();
  $row = $results->fetchAssoc();
  $mode = $row['set_mode'];

  $item_identifiers = array();
  $query = db_select('islandora_digital_workflow_batch_items', 'bi');
  $query->join('islandora_digital_workflow_process_set_items', 'pi', 'pi.batch_item_id = bi.batch_item_id');
  $query->fields('bi', array('identifier'));
  $query->condition('pi.process_set_id', $process_set_id)->execute();
  $results = $query->execute();
  $rows = $results->fetchAll();
  $return_array = array();
  foreach ($rows as $row) {
    $item_identifiers[] = $row->identifier;
  }
}

/**
 * Helper function to return the tablename for a given primary key fieldname
 *
 * @param string $identifier_fieldname
 *   The name of the primary key field.
 * @return string
 *   The name of the table if found, else an empty string.
 */
function islandora_digital_workflow_get_tablename_for_debug_fieldname($identifier_fieldname) {
  $mapping = array('drush_log_id' => 'islandora_digital_workflow_drush_log');
  return ((array_key_exists($identifier_fieldname, $mapping)) ? $mapping[$identifier_fieldname] : '');
}

/**
 * This could bring back any one record from a table given the primary key value, 
 * that primary key fieldname, and the tablename.
 *
 * @param integer $id_value
 *   The value for the primary key field.
 * @param string $id_fieldname
 *   The name of the primary key field for the table
 * @param string $table_name
 *   The name of the table from which the debug record is coming.
 * @return array
 *   The record as an array.
 */
function islandora_digital_workflow_get_debug_record($id_value, $id_fieldname, $table_name) {
  if ($table_name) {
    $query = db_select($table_name, 't')
        ->fields('t')
        ->condition('t.' . $id_fieldname, $id_value)
        ->range(0,1);
    $results = $query->execute();
    $row = $results->fetchAssoc();
    return $row;
  }
  else {
    return NULL;
  }
}

/**
 * This will delete a batch_item record related to a given $batch_item_id.
 *
 * @param integer $batch_item_id
 *   batch_item_id representing a islandora_digital_workflow_batch_items record.
 * @return array
 *   The number of records that have been updated (expect 1 or 0).
 */
function islandora_digital_workflow_delete_batch_item($batch_item_id) {
  // First, get the current item record if it exists and try to purge the files
  // that it was using.
  $batch_item_record_arr = islandora_digital_workflow_get_batch_item_record($batch_item_id);
  if (is_array($batch_item_record_arr)) {
    $batch_item_record_obj = array_pop($batch_item_record_arr);
    $item_path = '';
    if ($batch_item_record_obj->filename) {
      if (variable_get('islandora_digital_workflow_delete_delivery_files', 0)) {
        $pathinfo = pathinfo($batch_item_record_obj->filename);
        // ONLY do this if the path is an "item subfolder" and not higher - such as
        // one of the configured "Incoming Delivery Directory" or "Ingest Working
        // Directory" paths (would be very bad).
        if (array_key_exists('dirname', $pathinfo) && strstr($pathinfo['dirname'], $batch_item_record_obj->identifier) ) {
          $item_path = $pathinfo['dirname'];
        }
      }
      if (file_exists($batch_item_record_obj->filename)) {
        @unlink($batch_item_record_obj->filename);
        drupal_set_message(t('deleted file "' . $batch_item_record_obj->filename . '"'), 'error');
      }
    }
    else {
      $batch_record = islandora_digital_workflow_get_batch_item_record($batch_item_id);
      $batch_path = islandora_digital_workflow_batch_path($batch_record);
      $item_path = $batch_path . '/' . $batch_item_record_obj->identifier;
    }
    if ($item_path) {
      module_load_include('inc', 'islandora_digital_workflow', 'includes/file_utilities');
      $deleted = islandora_digital_workflow_deleteDirectory($item_path);
    }
  }

  // SQL statement to delete the islandora_digital_workflow_problem_items records
  // because they are related to islandora_digital_workflow_transactions records.
  $sql = "DELETE FROM islandora_digital_workflow_problem_items WHERE transaction_id " .
         "IN (SELECT transaction_id FROM islandora_digital_workflow_transactions " .
         " WHERE batch_item_id = " . $batch_item_id . ")";
  db_query($sql);

  db_delete('islandora_digital_workflow_transactions')
    ->condition('batch_item_id', $batch_item_id)
    ->execute();
  db_delete('islandora_digital_workflow_process_set_items')
    ->condition('batch_item_id', $batch_item_id)
    ->execute();
  db_delete('islandora_digital_workflow_drush_log')
    ->condition('batch_item_id', $batch_item_id)
    ->execute();

  // Finally the batch_items records can be deleted.
  return db_delete('islandora_digital_workflow_batch_items')
    ->condition('batch_item_id', $batch_item_id)
    ->execute();
}

/**
 * Will delete the  islandora_digital_workflow_batch record with matching
 * $batch_id value.
 *
 * @param integer $batch_id
 * @return boolean
 *   Success on deleting a batch with given $batch_id value.
 */
function islandora_digital_workflow_delete_batch_record($batch_id) {
  if ($batch_id) {
    return db_delete('islandora_digital_workflow_batch')
      ->condition('batch_id', $batch_id)
      ->execute();
  }
}

/**
 * Helper function to delete the workflow_batch nodes and any related
 * islandora_digital_workflow_batch records.
 *
 * @return boolean
 *   Whether or not there was any problem deleting the nodes that are
 * "workflow_batch" type.
 */
function islandora_digital_workflow_delete_workflow_batch_nodes() {
  $nids = db_select('node', 'n')
      ->fields('n', array('nid'))
      ->fields('n', array('type'))
      ->condition('n.type', 'workflow_batch')
      ->execute()
      ->fetchCol(); // returns an indexed array

  $delete_success = TRUE;
  foreach ($nids as $nid) {
    $batch_record = islandora_digital_workflow_get_batch_record_by_nid($nid);
    if (is_array($batch_record)) {
      islandora_digital_workflow_delete_batch_record($batch_record['batch_id']);
    }
    $delete_success &= node_delete($nid);
  }
  return $delete_success;
}

/**
 * Will look up the batch set for the given batch_external_id ... will match an
 * islandora PID value when prefixed with the ingest namespace variable.
 *
 * @param string $pid
 * @return integer - value of the set
 */
function islandora_digital_workflow_get_set($pid) {
  $result = 0;
  $results = array();
  $q = db_select('islandora_batch_queue', 'q');
  $items = $q->fields('q', array('sid'))
    ->condition('q.id', $pid)
    ->execute();
  while ($item = $items->fetchAssoc()) {
    $result = $item['sid'];
  }

  return $result;
}

/**
 * This will set the batch record's progress based on the completedness of all
 * of the items of the batch.
 *
 * @param array $batch_record
 *   The batch_record array related to the batch in question.
 */
function islandora_digital_workflow_update_batch_record_progress($batch_record = array()) {
  // must get all items related to the batch -- and loop through to set the batch
  // value for progress based on all items having that progress.
  $batch_items = islandora_digital_workflow_get_batch_items_by_nid($batch_record['nid']);
  // Go in the reverse order of possible progress values.
  if (!($set_batch_progress_to = islandora_digital_workflow_batch_items_all_same_progress($batch_items, t('Completed'), array()))) {
    if (!($set_batch_progress_to = islandora_digital_workflow_batch_items_all_same_progress($batch_items, t('Prepared'), array(t('Completed'))))) {
      if (!($set_batch_progress_to = islandora_digital_workflow_batch_items_all_same_progress($batch_items, t('In Progress'), array(t('Prepared'), t('Completed'))))) {
        $set_batch_progress_to = t('New');
      }
    }
  }
  // may need to set the progress if it is not the same as the current value stored
  // in the batch_record.
  if ($batch_record['progress'] <> $set_batch_progress_to) {
    islandora_digital_workflow_update_batch($batch_record['batch_id'], array(
        'progress' => $set_batch_progress_to));
  }
}

/**
 * Helper function to determine whether or not all of the items are at the same
 * "progress" step.
 *
 * @param array $batch_items
 *   Set of batch_item objects.
 * @param array $check_for_progress
 *   A progress value to check all items if they have passed that progress.
 * @param array $check_for_progress_array
 *   A set of progress values that would allow the code to consider that the
 * lowest value was obtained.  For example, if there are three items and two are
 * "Completed" and one was "Prepared", the two checks of the items for the
 * "Prepared" stage should also consider it a match if the progress value was
 * "Completed" - in other words, "Prepared" was actually already achieved for
 * that item.
 * @return string
 *   Will return the status of the items ONLY when all items have the same status,
 * else this will return an empty string.
 */
function islandora_digital_workflow_batch_items_all_same_progress($batch_items, $check_for_progress, $check_for_progress_array = array()) {
  // Start off with a TRUE -- will be FALSE when there are no items in the array.
  $all_same = (is_array($batch_items) && (count($batch_items) > 0));
  $check_for_progress_array[] = $check_for_progress;
  foreach ($batch_items as $item_obj) {
    $all_same &= (!(array_search($item_obj->progress, $check_for_progress_array) === FALSE));
  }
  return (!$all_same) ? '' : $check_for_progress;
}

/**
 * This will return the batch record that is related to a webforms submission.
*
 * @param integer $sid
 *   Webform submission identifier value.
 * @return mixed
 *   Either returns the batch record object or NULL if it cannot be found.
 */
function islandora_digital_workflow_get_batch_of_webformsubmission($sid) {
  if ($sid) {
    $query = db_select('islandora_digital_workflow_batch', 'b');
    $query->fields('b')
        ->condition('b.webform_submission_sid', $sid);
    $results = $query->execute();
    $rows = $results->fetchAll();
    return $rows;
  }
  else {
    return NULL;
  }
}

/**
 * This will get the values for a given submission by the submission identifier.
 * This joins the submission table to the node table in order to get the title
 * of that kind of submission (ie: "Scan Request")
 *
 * @param integer $sid
 *   Webform submission identifier value.
 * @return mixed
 *   Either returns the batch record object or NULL if it cannot be found.
 */
function islandora_digital_workflow_get_webformsubmission($sid) {
  if ($sid) {
    $query = db_select('webform_submissions', 'ws');
    $query->fields('ws');
    $query->join('node', 'n', 'n.nid = ws.nid');
    $query->fields('n', array('title'))
        ->condition('ws.sid', $sid);
    $results = $query->execute();
    $rows = $results->fetchAll();
    $row = array_pop($rows);
    return $row;
  }
  else {
    return NULL;
  }
}

/**
 * Finds the webform_component.extra value for a given webform component.
 *
 * @param integer $cid
 *   Webform component identifier.
 * @param string $type
 *   Optional type of component to filter upon.
 * @return mixed
 *   Returns the extra value if there is a matching record, else returns NULL.
 */
function islandora_digital_workflow_get_webform_componentdef_extra($cid, $type = '') {
  $query = db_select('webform_component', 'wc')
      ->fields('wc');
  if ($type) {
    $query->condition('wc.type', 'select');
  }
  $query->condition('wc.cid', $cid);
  $results = $query->execute();
  $rows = $results->fetchAll();
  if (count($rows) > 0) {
    $row = array_pop($rows);
    $extra = unserialize($row->extra);
    return $extra;
  }
  else {
    return NULL;
  }
}

/**
 * A function that will check a given workflow sequence for the "Ingest item
 * into repository" and "Publish the item" actions - and add them if they do
 * not exist.
 *
 * @param integer $workflow_sequence_id
 *   The identifier for a given workflow sequence.
 */
function islandora_digital_workflow_workflow_autofix_sequence($workflow_sequence_id) {
  $batch_item_sequence_actions = islandora_digital_workflow_get_batch_items_actions($workflow_sequence_id, FALSE);
  $found_user_ingest_action = $found_system_ingest_action = $found_publish_action = FALSE;
  $publish_action_sequence_order = 0;
  foreach ($batch_item_sequence_actions as $action_id => $sequence_action_arr) {
    if ($action_id == IDW_PUBLISH_OBJECT) {
      $publish_action_sequence_order = $sequence_action_arr['sequence_order'];
    }
    $found_user_ingest_action |= ($action_id == IDW_ACTION_USER_INGEST);
    $found_system_ingest_action |= ($action_id == IDW_ACTION_SYSTEM_INGESTED);
    $found_publish_action |= ($action_id == IDW_PUBLISH_OBJECT);
  }
  // IDW_ACTION_USER_INGEST : User queues the item for ingest by the system.
  if (!$found_system_ingest_action) {
    $fields = array(
        'workflow_sequence_id' => $workflow_sequence_id,
        'action_id' => IDW_ACTION_USER_INGEST,
        '`order`' => ($publish_action_sequence_order - 2),
        'is_required' => 1,
    );
    $inserted = db_insert('islandora_digital_workflow_sequence_actions')
      ->fields($fields)
      ->execute();
    drupal_set_message(t('The "User queues the item for ingest by the system" action was added automatically.'));
  }
  // IDW_ACTION_SYSTEM_INGESTED : System has ingested item into repository.
  if (!$found_system_ingest_action) {
    $fields = array(
        'workflow_sequence_id' => $workflow_sequence_id,
        'action_id' => IDW_ACTION_SYSTEM_INGESTED,
        '`order`' => ($publish_action_sequence_order - 1),
        'is_required' => 1,
    );
    $inserted = db_insert('islandora_digital_workflow_sequence_actions')
      ->fields($fields)
      ->execute();
    drupal_set_message(t('The "System has ingested item into repository" action was added automatically.'));
  }
  // IDW_PUBLISH_OBJECT : Publish the item.
  if (!$found_publish_action) {
    $fields = array(
        'workflow_sequence_id' => $workflow_sequence_id,
        'action_id' => IDW_PUBLISH_OBJECT,
        '`order`' => 10,
        'is_required' => 1,
    );
    $inserted = db_insert('islandora_digital_workflow_sequence_actions')
      ->fields($fields)
      ->execute();
    drupal_set_message(t('The "Publish the item" action was added automatically.'));
  }
}