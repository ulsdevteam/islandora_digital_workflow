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
 *   The `batch_id` value for a batch record.
 * @return mixed
 *   Either returns the batch record object or NULL if it cannot be found.
 */
function islandora_digital_workflow_get_batch_record_by_batch_id($batch_id) {
  if ($batch_id) {
    $query = db_select('islandora_digital_workflow_batch', 'b')
        ->fields('b')
        ->condition('b.batch_id', $batch_id)
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
 * Returns all batch records that have a partial match for the `uploaded_csv_file` field
 * in the islandora_digital_workflow_batch table.
 *
 * @param string $identifier
 *   The value of the islandora_digital_workflow_batch.`uploaded_csv_file` field.
 * @return mixed
 *   Either returns an array of batch record objects or NULL if it cannot be found.
 */
function islandora_digital_workflow_search_batch_csv_by_identifier($identifier) {
  if ($identifier) {
    $query = db_select('islandora_digital_workflow_batch', 'b');
    $query->fields('b')
        ->condition('b.uploaded_csv_file', '%' . db_like($identifier) . '%', 'LIKE')
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
 * This returns an array of batch record objects that have no value set for
 * `ingest_namespace`.
 *
 * These records represent "stub records" and only exist if the user has not completed
 * the second page of a new batch to provide all of the details.  Stub records are
 * created with only the name, the model type, and a description value.
 *
 * @return array
 *   Either returns an array of batch record objects or NULL if it cannot be found.
 */
function islandora_digital_workflow_find_batch_records_no_details() {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/taxonomy-utilities');
  $query = db_select('islandora_digital_workflow_batch', 'b')
      ->fields('b')
      ->condition('b.ingest_namespace', NULL);
  $query->addJoin('LEFT OUTER', 'taxonomy_term_data', 't', 'b.batch_priority_tid = t.tid');
  $query->addField('t', 'weight');
  $query->addField('t', 'name', 'priority');
  $query->orderBy('weight');
  $results = $query->execute();
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
  module_load_include('inc', 'islandora_digital_workflow', 'includes/taxonomy-utilities');
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
  module_load_include('inc', 'islandora_digital_workflow', 'includes/taxonomy-utilities');
  $query = db_select('islandora_digital_workflow_batch', 'b')
      ->fields('b')
      ->condition('b.is_batch_request', 1);
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

function islandora_digital_workflow_check_trigger_stage_change($transaction_id = NULL, $transaction_values = NULL) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/node-utilities');
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

  if (!isset($action_record['stage_taxonomy_tid']) || is_null($action_record['term_name'])) {
    return;
  }

  $affects = ($action_record['is_triggered_by_single_item'] ?
      '"@action_description" has been added' :
      'All Items in the batch have "@action_description"');
  $message_args = array(
      '@action_description' => $action_record['action_description'],
      '@stage' => $action_record['term_name'],
      '@batch_item_identifier' => (($batch_item_record &&
        is_object($batch_item_record->identifier)) ?
          $batch_item_record->identifier : ''),
      '@batch_name' => $batch_record['batch_name'],);
  $message_string = 
    (($batch_item_record) ? ' for the item @batch_item_identifier' . $batch_item_record->identifier : '') .
    (($batch_record) ? ' on the batch "@batch_name"' : '');
  if ($action_record['is_batch_action'] == 1 || $action_record['is_triggered_by_single_item']) {
    islandora_digital_workflow_set_node_stage($batch_record['nid'], '', $action_record['stage_taxonomy_tid']);
    drupal_set_message(t($affects . ' which triggers the "@stage stage" for ' .
        $message_string, $message_args));
  }
  else {
    // the load the $items with the $batch_record->nid value
    $items = islandora_digital_workflow_get_batch_items_by_nid($batch_record['nid']);
    $all_items_have_action = TRUE;
    foreach ($items as $item_obj) {
      $transaction_action_ids = explode(',', $item_obj->transaction_action_ids);
      $all_items_have_action &= (!(array_search($transaction_values['action_id'], $transaction_action_ids) === FALSE));
    }
    if ($all_items_have_action) {
      islandora_digital_workflow_set_node_stage($batch_record['nid'], '', $action_record['stage_taxonomy_tid']);
      drupal_set_message(t($affects . ' which triggers the "@stage stage" for ' .
          $message_string, $message_args));
    }
    // else, the Stage does not get set here because all items did not have that action.
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
 *
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
    $rows[$index]->user_name = ($row->uid) ? l($row->name, 'user/' . $row->uid, array('attributes' => array('target' => '_blank'))) : 'system';
    $rows[$index]->admin_links = ($access_edit_transactions) ? TRUE : FALSE;
    $rows[$index]->nid = $nid;
  }
  return $rows;
}

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
         'WHERE  (bt.batch_item_id = ' . $batch_item_id . ' AND p.problem_resolved_timestamp IS NULL) ' .
         'UNION SELECT bt.timestamp AS `timestamp`, bt.transaction_id AS transaction_id, ' .
         'users.uid AS uid, users.name AS name, a.description AS description, ' .
         'a.action_id AS action_id, a.name AS action_name, a.is_batch_action AS is_batch_action, p.problem_notes ' .
         'FROM islandora_digital_workflow_transactions bt ' .
         'LEFT OUTER JOIN islandora_digital_workflow_problem_items p ON bt.transaction_id = p.transaction_id ' .
         'LEFT OUTER JOIN users users ON bt.drupal_uid = users.uid ' .
         'INNER JOIN islandora_digital_workflow_actions a ON a.action_id = bt.action_id ' .
         'WHERE  (bt.batch_id = ' . $batch_id . ' AND p.problem_resolved_timestamp IS NULL) ' .
         'ORDER BY `timestamp` ASC';

  $results = db_query($sql);
  $access_edit_transactions = user_access(ISLANDORA_DIGITAL_WORKFLOW_EDIT_DELETE_TRANSACTIONS);
  $rows = $results->fetchAll();
  foreach ($rows as $index => $row) {
    $rows[$index]->how_long_ago = islandora_digital_workflow_timeago_from_timestamp($row->timestamp);
    $rows[$index]->user_name = ($row->uid) ? l($row->name, 'user/' . $row->uid, array('attributes' => array('target' => '_blank'))) : 'system';
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
    $rows[$index]->user_name = ($row->uid) ? l($row->user_name, 'user/' . $row->uid, array('attributes' => array('target' => '_blank'))) : 'system';
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
    bi.batch_item_id
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
  return $rows;
}

/**
 * This returns the batch item record for $batch_item_id.
 *
 * @return array
 *   Either returns an array of a single batch item record objects.
 */
function islandora_digital_workflow_get_batch_item_record($batch_item_id) {
  $query = db_select('islandora_digital_workflow_batch_items', 'bi')
      ->fields('bi')
      ->condition('bi.batch_item_id', $batch_item_id);
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
  $results = db_update('islandora_digital_workflow_batch_items')
      ->fields(array(
        'title' => $update_values['title'],
        'identifier' => $update_values['identifier'],
        'filename' => $update_values['filename'],
        'assigned_pid' => $update_values['assigned_pid'],
        'islandora_model' => $update_values['islandora_model'],
        'type_of_resource' => $update_values['type_of_resource'],
        'mods' => $update_values['mods'],
        'marc' => (array_key_exists('marc', $update_values) ? $update_values['marc'] : ''),
      ))
      ->condition('batch_item_id', $batch_item_id)
      ->execute();

  if ($results) {
    // Compare the filename value to the previous filename value to see
    // if the underlying file needs to be renamed.
    if ($update_values['filename'] <> $original_values->filename) {
      if (file_exists($update_values['filename'])) {
        drupal_set_message('A file already existed with the updated filename, so nothing needed to be renamed.', 'status');
      }
      if (file_exists($original_values->filename)) {
        $renamed_file = rename($original_values->filename, $update_values['filename']);
        if ($renamed_file) {
          drupal_set_message('The underlying file has been renamed to match the value that was supplied.', 'status');
        }
        else {
          drupal_set_message('There was a problem renaming the underlying file from "' .
              $original_values->filename . '" to "' . $update_values['filename'] . '".  It could be a file permissions issue.', 'error');
        }
      }
    }

    // Compare the underlying MARC file to the value that is passed by $update_values --
    // and overwrite that file if needed.
    if (array_key_exists('marc', $update_values) && ($update_values['marc'] <> $original_values->marc)) {
      $orig_filename_pathinfo = pathinfo($original_values->filename);
      $calculated_marc_filename = (array_key_exists('extension', $orig_filename_pathinfo)) ?
          str_replace('.' . $orig_filename_pathinfo['extension'], '.xml', $original_values->filename) :
          $original_values->filename . '.xml';
      $bytes_written = ($calculated_marc_filename) ?
          file_put_contents($calculated_marc_filename, $update_values['marc']) : 0;
      if ($bytes_written <> strlen($update_values['marc'])) {
        drupal_set_message('There was a problem saving the updated MARC value to the file system at "' .
          $calculated_marc_filename . '".  It may be a file permissions issue.', 'error');
      }
      islandora_digital_workflow_insert_transactions_record(IDW_ACTION_ADD_MARC_RECORD, NULL, $batch_item_id);
    }
    // Compare the underlying MODS file to the value that is passed by $update_values --
    // and overwrite that file if needed.
    if ($update_values['mods'] <> $original_values->mods) {
      $orig_filename_pathinfo = pathinfo($original_values->filename);
      $calculated_mods_filename = (array_key_exists('extension', $orig_filename_pathinfo)) ?
          str_replace('.' . $orig_filename_pathinfo['extension'], '.xml', $original_values->filename) :
          $original_values->filename . '.xml';
      $bytes_written = ($calculated_mods_filename) ?
          file_put_contents($calculated_mods_filename, $update_values['mods']) : 0;
      if ($bytes_written <> strlen($update_values['mods'])) {
        drupal_set_message('There was a problem saving the updated MODS value to the file system at "' .
          $calculated_mods_filename . '".  It may be a file permissions issue.', 'error');
      }
      islandora_digital_workflow_insert_transactions_record(IDW_ACTION_ADD_MODS_RECORD, NULL, $batch_item_id);
    }
  }
  return $results;
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
 * @return mixed
 *   Either returns an array of actions record objects or NULL if it cannot be found.
 */
function islandora_digital_workflow_get_batch_items_actions($workflow_sequence_id = 0) {
  // If no model is provided, display the sequence_action for the default
  // batch_item record.
  $query = db_select('islandora_digital_workflow_actions', 'ba')
      ->fields('ba')
      ->condition('ba.is_batch_action', 0);
  $query->join('islandora_digital_workflow_sequence_actions', 'sa', 'sa.action_id = ba.action_id');
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
    $options[$row->action_id] = $row->description;
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
  $query = db_query('SELECT a.*, td.name as `term_name` FROM islandora_digital_workflow_actions a ' .
          'LEFT OUTER JOIN taxonomy_term_data td ON td.tid = stage_taxonomy_tid ' .
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
  $query = db_query('SELECT a.*, td.name as `term_name` FROM islandora_digital_workflow_actions a ' .
          'LEFT OUTER JOIN taxonomy_term_data td ON td.tid = a.stage_taxonomy_tid ' .
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
  $query->addField('td', 'name', 'term_name');

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
 * Returns the matching islandora_digital_workflow_batch_items record.
 *
 * @param string $identifier
 * @return stdObject
 *   The batch_item record object.
 */
function islandora_digital_workflow_get_batch_item_record_by_identifier($identifier, $use__batch_id = NULL) {
  if ($use__batch_id) {
    $results = db_select('islandora_digital_workflow_batch_items', 'bi')
          ->fields('bi', array('batch_item_id'))
          ->condition('batch_id', $use__batch_id)
          ->condition('identifier', '%' . db_like($identifier), 'LIKE')
          ->execute();
  }
  else {
    $results = db_select('islandora_digital_workflow_batch_items', 'bi')
          ->fields('bi', array('batch_item_id'))
          ->condition('identifier', '%' . db_like($identifier), 'LIKE')
          ->execute();
  }
  $row = $results->fetchAssoc();
  return (is_array($row)) ? array_pop($row) : $row;
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
  $query->addField('wa', 'description', 'action_description');

  $query->condition('wa.action_id', $action_id);
  $query->addJoin('LEFT', 'taxonomy_term_data', 'td', 'wa.stage_taxonomy_tid = td.tid');
  $query->fields('td');
  $results = $query->execute();
  $rows = $results->fetchAll();
  $return = array();
  // To have explicit control over the returned array results...
  foreach ($rows as $row) {
    $return = array(
        'action_id' => $action_id,
        'action_name' => $row->action_name,
        'action_description' => $row->action_description,
        'glyph' => $row->glyph,
        'is_batch_action' => $row->is_batch_action,
        'is_triggered_by_single_item' => $row->is_triggered_by_single_item,
        'stage_taxonomy_tid' => $row->stage_taxonomy_tid,
        'term_name' => $row->name,
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
   foreach ($item_data as $item_identifier => $identifer_data) {
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
       ->condition('identifier', $item_identifier)
       ->fields($set_fields)
       ->execute();
      $markup_arr[] = '<b><span style="color:#888">' . $batch_record->batch_name . '/</span>' . $item_identifier . '</b><br>';
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
  // make the value serialized like:  //  a:7:{s:6:"source";s:4:"user";s:13:"...
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
 *   The batch_id of the batch being sync'd from the delivery directory,
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

function islandora_digital_workflow_get_process_set_records($process_set_id, &$mode, &$items) {
  $query = db_select('islandora_digital_workflow_process_set', 'p')
      ->fields('p', array('set_mode'))
      ->condition('p.process_set_id', $process_set_id)
      ->range(0,1);

  $results = $query->execute();
  $row = $results->fetchAssoc();
  $mode = $row['set_mode'];

  $items = array();
  $query = db_select('islandora_digital_workflow_batch_items', 'bi');
  $query->join('islandora_digital_workflow_process_set_items', 'pi', 'pi.batch_item_id = bi.batch_item_id');
  $query->fields('bi', array('identifier'));
  $query->condition('pi.process_set_id', $process_set_id)->execute();
  $results = $query->execute();
  $rows = $results->fetchAll();
  $return_array = array();
  foreach ($rows as $row) {
    $items[] = $row->identifier;
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
