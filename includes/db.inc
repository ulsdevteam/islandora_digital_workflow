<?php

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
  $query = db_select('islandora_digital_workflow_batch', 'b')
      ->fields('b')
      ->condition('b.ingest_namespace', NULL);
  $results = $query->execute();
  $rows = $results->fetchAll();
  return $rows;
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
  foreach ($rows as $index => $row) {
    $row->how_long_ago = islandora_digital_workflow_timeago_from_timestamp($row->timestamp);
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
function islandora_digital_workflow_update_batch_transaction($transaction_id, $action_id, $timestamp) {
    return db_update('islandora_digital_workflow_transactions')
      ->fields(array('action_id' => $action_id, 'timestamp' => $timestamp))
      ->condition('transaction_id', $transaction_id)
      ->execute();
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
  $query->join('islandora_digital_workflow_actions', 'a', 'a.action_id = bt.action_id');
  $query->fields('a', array('description'));
  $query->condition('b.nid', $nid)
      ->orderBy('bt.timestamp', 'ASC');

  $access_edit_transactions = user_access(ISLANDORA_DIGITAL_WORKFLOW_EDIT_DELETE_TRANSACTIONS);
  $results = $query->execute();
  $rows = $results->fetchAll();
  foreach ($rows as $index => $row) {
    $rows[$index]->how_long_ago = islandora_digital_workflow_timeago_from_timestamp($row->timestamp);
    $rows[$index]->glyph_class = 'transaction_action_' . (($row->description) ? strtolower(str_replace(" ", "_", $row->description)) : 'spacer');
    $rows[$index]->admin_links = ($access_edit_transactions) ? TRUE : FALSE;
    $rows[$index]->nid = $nid;
  }
  return $rows;
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
function islandora_digital_workflow_get_batch_item_transactions($batch_item_id, $nid) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');

  $query = db_select('islandora_digital_workflow_transactions', 'bt');
  $query->fields('bt', array('timestamp', 'transaction_id'));
  $query->join('islandora_digital_workflow_actions', 'a', 'a.action_id = bt.action_id');
  $query->fields('a', array('description'));
  $query->condition('bt.batch_item_id', $batch_item_id)
      ->orderBy('bt.timestamp', 'ASC');

  $access_edit_transactions = user_access(ISLANDORA_DIGITAL_WORKFLOW_EDIT_DELETE_TRANSACTIONS);
  $results = $query->execute();
  $rows = $results->fetchAll();
  foreach ($rows as $index => $row) {
    $rows[$index]->how_long_ago = islandora_digital_workflow_timeago_from_timestamp($row->timestamp);
    $rows[$index]->glyph_class = 'transaction_action_' . (($row->description) ? strtolower(str_replace(" ", "_", $row->description)) : 'spacer');
    $rows[$index]->admin_links = ($access_edit_transactions) ? TRUE : FALSE;
    $rows[$index]->nid = $nid;
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
      $rows[$index]->filename_basename = $filename_pathinfo['basename'];
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
  $query = db_query('SELECT bi.identifier, bi.title, bi.filename, 
    GROUP_CONCAT(DISTINCT a.description) as `transaction_actions`,
    GROUP_CONCAT(DISTINCT a.action_id) as `transaction_action_ids`,
    bi.batch_item_id
FROM islandora_digital_workflow_batch b
JOIN islandora_digital_workflow_batch_items bi ON b.batch_id = bi.batch_id
LEFT JOIN islandora_digital_workflow_transactions bt ON bi.batch_item_id = bt.batch_item_id
LEFT JOIN islandora_digital_workflow_actions a ON a.action_id = bt.action_id
WHERE b.nid = ' . $nid . '
GROUP BY bi.batch_item_id
ORDER BY bi.batch_item_id ASC, bt.`timestamp` DESC');
  $rows = $query->fetchAll();
  // Loop through and calculate values for filename_basename by using the
  // pathinfo ~ basename element.
  foreach ($rows as $index => $row) {
    $filename_pathinfo = pathinfo($row->filename);
    $rows[$index]->filename_basename = $filename_pathinfo['basename'];
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
    $rows[$index]->filename_basename = $filename_pathinfo['basename'];
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
 * @param string $tablename
 *   The name of the MySQL table to inspect.
 * @return array
 *   'primary_key_field' string the name of the primary key for the table
 *   'numeric_fields' array names of numeric fields
 *   'num_numeric_fields' array names of non-numeric fields
 *   'all_fields' arry names of all fields
 */
function islandora_digital_workflow_get_tablefield_stats($tablename) {
  // TODO:  this is still not used.  Search the project later and see if this
  // can be removed.
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
      ->condition('ba.action_id', 99, '>');
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
 * @return mixed
 *   Either returns an array of actions record objects or NULL if it cannot be found.
 */
function islandora_digital_workflow_get_batch_items_actions($model = '') {
  // If no model is provided, display the sequence_action for the default
  // batch_item record.
  $query = db_select('islandora_digital_workflow_actions', 'ba')
      ->fields('ba')
      ->condition('ba.action_id', 100, '<');
  $query->join('islandora_digital_workflow_sequence_actions', 'sa', 'sa.action_id = ba.action_id');
  $query->orderBy('sa.order');
  $query->join('islandora_digital_workflow_sequence', 's', 's.workflow_sequence_id = sa.workflow_sequence_id');
  $query->fields('s', array('name'));
  $query->join('islandora_digital_workflow_model_sequence', 'm', 'm.workflow_sequence_id = s.workflow_sequence_id');
  $query->condition('m.islandora_model', $model);

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
  $results = $query->execute();
  $rows = $results->fetchAll();
  $return_array = array();
  foreach ($rows as $i => $row) {
    $return_array[$row->workflow_sequence_id] = array(
        'workflow_sequence_id' => $row->workflow_sequence_id,
        'name' => $row->name,
        'description' => $row->description,
    );
  }
  return $return_array;
}

function islandora_digital_workflow_get_item_actions() {
  $rows = db_query('SELECT * FROM islandora_digital_workflow_actions WHERE action_id < 100')->fetchAll();
  $return_array = array();
  foreach ($rows as $i => $row_obj) {
    $return_array[$row_obj->action_id] = array(
        'action_id' => $row_obj->action_id,
        'name' => $row_obj->name,
        'description' => $row_obj->description,
      );
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
  $query->addField('a', 'description', 'action_description');
  $query->join('islandora_digital_workflow_sequence', 's', 's.workflow_sequence_id = sa.workflow_sequence_id');
  $query->addField('s', 'name', 'sequence_name');
  $query->addField('s', 'description', 'sequence_description');
  $results = $query->execute();
  $rows = $results->fetchAll();
  $return_array = array();
  foreach ($rows as $i => $row) {
    $return_array[$row->workflow_sequence_id][] = array(
        'sequence_name' => $row->sequence_name,
        'sequence_description' => $row->sequence_description,
        'order' => $row->order,
        'action_id' => $row->action_id,
        'action_name' => $row->action_name,
        'action_description' => $row->action_description,
        'is_required' => $row->is_required,
    );
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
 */
function islandora_digital_workflow_insert_transactions_record($action_id, $batch_id = NULL, $batch_item_id = NULL) {
  return db_insert('islandora_digital_workflow_transactions')
      ->fields(array(
        'action_id' => $action_id,
        'batch_id' => $batch_id,
        'batch_item_id' => $batch_item_id
      ))
      ->execute();
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

function islandora_digital_workflow_get_required_batch_actions($batch_id = NULL, $nid = NULL) {
  $query = db_select('islandora_digital_workflow_batch', 'b');
  $query->fields('b', array('batch_id', 'workflow_sequence_id'));
  $query->join('islandora_digital_workflow_sequence', 'ws', 'b.workflow_sequence_id = ws.workflow_sequence_id');
  $query->fields('ws', array('name'));
  $query->join('islandora_digital_workflow_sequence_actions', 'wsa', 'ws.workflow_sequence_id = wsa.workflow_sequence_id');
  $query->fields('wsa', array('is_required'));
  $query->addField('wsa', 'order', 'action_order');
  $query->join('islandora_digital_workflow_actions', 'wa', 'wsa.action_id = wa.action_id');
  $query->fields('wa', array('description', 'action_id'));
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
  foreach ($rows as $row) {
    $return_rows[$row->action_order] = array(
        'batch_id' => $row->batch_id,
        'batch_action_description' => $row->description,
        'is_required' => $row->is_required,
        'sequence_name' => $row->name,
        'action_id' => $row->action_id,
        'batch_action_description' => $row->description,
    );
  }
  return $return_rows;
}