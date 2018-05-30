<?php

/**
 * This array mapping is needed from various places in the code.
 * 
 * Provided here as a function so that key lookup is possible as well as 
 * searching for the "Content type" name given the model name.
 *
 * @return array
 *   The mapping between Islandora models and the human readable names.  The
 * key is human readable and the value is the Islandora model.
 */
function islandora_digital_workflow_content_type_to_islandora_model_mappings() {
  return array(
    'Audio' => 'islandora:sp-audioCModel',
    'Book' => 'islandora:bookCModel',
    'Internet Archive Book' => 'islandora:bookCModel',
    'Disk Image' => 'islandora:sp_disk_image',
    'Finding Aid' => 'islandora:findingAidCModel',
    'Image' => 'islandora:sp_basic_image',
    'Large Image' => 'islandora:sp_large_image_cmodel',
    'Image' => 'islandora:sp_large_image_cmodel',
    'Manuscript' => 'islandora:manuscriptCModel',
    'Newspaper' => 'islandora:newspaperIssueCModel',
    'Newspaper/Serial' => 'islandora:newspaperIssueCModel',
    'Oral History' => 'islandora:oralhistoriesCModel',
    'PDF' => 'islandora:sp_pdf',
    'Video' => 'islandora:sp_videoCModel',
    'Web Archive' => 'islandora:sp_web_archive',
  );
}

/**
 * This will return the islandora model name for a given content type.
 *
 * @param string $content_type
 *   The human-readable (Islandora) content type name.
 * @return string
 *   The Islandora model name for the content type provided.
 */
function islandora_digital_workflow_content_type_to_islandora_model($content_type) {
  $islandora_content_type_mapping = islandora_digital_workflow_content_type_to_islandora_model_mappings();
  return (array_key_exists($content_type, $islandora_content_type_mapping)) ? $islandora_content_type_mapping[$content_type] : '';
}

/**
 * Creates a minimal batch stub record that will be updated in other parts of
 * the code.
 *
 * @global object $user
 *   Drupal user object.
 * @param array $form_state
 *   Drupal representation of form_state as an array.
 * @return mixed
 *   Either redurns the batch record OR NULL if there is not enough info to
 * create the record
 */
function islandora_digital_workflow_create_stub_batch_record($form_state) {
  global $user;
  // TODO: add a validate call instead of checking the values of $form_state here.
  if (is_array($form_state) && isset($form_state['values']['batch_name'])
    && $form_state['values']['batch_name']) {
    $batch_name = $form_state['values']['batch_name'];
    $batch_record = islandora_digital_workflow_get_batch_record_by_name($batch_name);

    $field_values = array(
        'batch_name' => $batch_name,
        'batch_description' => $form_state['values']['batch_description'],
        'default_type_of_resource' => islandora_digital_workflow_get_name_of_tid($form_state['values']['cmodel_select']),
        'is_batch_active' => 0,
        'is_batch_request' => 0,
        'user' => $user->name,
        'lastmod' => time(),
      );

    if ((!is_array($batch_record))) {
      $batch_id = db_insert('islandora_digital_workflow_batch')
      ->fields($field_values)
      ->execute();
    }
    else {
      $batch_id = db_update('islandora_digital_workflow_batch')
      ->fields($field_values)
      ->condition('batch_name', $batch_name)
      ->execute();
    }
    $updated_batch_record = islandora_digital_workflow_get_batch_record_by_name($batch_name);
    return $updated_batch_record;
  }
  else {
    return NULL;
  }
}

/**
 * Given a form submission and related workflow_batch node, this should update
 * the batch record.
 *
 * @global object $user
 *   Drupal user object.
 * @param array $form_state
 *   Drupal representation of form_state as an array.
 * @param object $node
 *   Drupal node object.
 * @param string $csv_filename
 *   The filename that points to the CSV file.
 * @return mixed
 *   Either redurns the batch record OR NULL if there is not enough info to
 * create the record
 */
function islandora_digital_workflow_create_batch_record_by_form($form_state, $node, $csv_filename) {
  global $user;
  // TODO: add a validate call instead of checking the values of $form_state here.
  if (is_array($form_state) && isset($form_state['values']['batch_name'])
    && $form_state['values']['batch_name']) {
    $batch_name = $form_state['values']['batch_name'];
    $batch_record = islandora_digital_workflow_get_batch_record_by_name($batch_name);

    $uploaded_csv_file = ($csv_filename) ? file_get_contents(drupal_realpath($csv_filename)) : FALSE;
    $batch_request_date = strtotime($form_state['values']['batch_request_due_date']['year'] . '/' .
        $form_state['values']['batch_request_due_date']['month'] . '/' .
        $form_state['values']['batch_request_due_date']['day']);

    // Since the form can be modified and potentially may not include some
    // values, check whether or not these form_state fields exist before
    // trying to access their values.  This is the reason for some of these
    // fields to have a logic test for setting the record's value.
    $field_values = array(
        'batch_name' => $batch_name,
        'batch_description' => $form_state['values']['batch_description'],
	'is_batch_active' => $form_state['values']['is_batch_active'],
        'is_batch_request' => $form_state['values']['is_batch_request'],
        'batch_requestor' => $form_state['values']['batch_requestor'],
        'batch_request_due_date' => $batch_request_date,
        'object_count' => $form_state['values']['object_count'],
        'ingest_namespace' => $form_state['values']['ingest_namespace'],
        'default_ead_id' => (array_key_exists('default_ead_id', $form_state['values'])) ?
          $form_state['values']['default_ead_id'] : '',
        'default_depositor' => $form_state['values']['default_depositor'],
	'batch_host' => $form_state['values']['batch_host'],
	'default_genre' => $form_state['values']['default_genre'],
	'batch_default_pub_status' => $form_state['values']['batch_default_pub_status'],
	'batch_default_CR_status' => $form_state['values']['batch_default_CR_status'],
	'batch_default_CR_holder' => $form_state['values']['batch_default_CR_holder'],
        'default_type_of_resource' => islandora_digital_workflow_get_name_of_tid($form_state['values']['cmodel_select']),
        'mapto_collections' => implode(",", $form_state['values']['collections_select']),
        'nid' => $node->nid,
        'user' => $user->name,
        'lastmod' => time(),

        // default_type_of_resource - the text value of the selected Islandora Model
        // 
        // if configured, set the site_id field and value
        //   mapto_site_id_values

        /*
        'batch_default_perm_notes' => '',
        'structural_metadata_treatment' => '',
        'image_editing_treatment' => '',
        'blank_and_missing_treatment' => '',
        'page_edge_treatment' => '',
        'batch_condition_handling' => '',
        */
      );

    // if configured, set the site_id field and value
    //   mapto_site_id_values / sites_select
    if (variable_get('islandora_digital_workflow_use_isMemberOfSite', 0)) {
      $field_values['mapto_site_id_values'] = implode(",", $form_state['values']['sites_select']);
    }

    // Only update the uploaded CSV file if the file was passed in.  In other words
    // updating a batch record should not wipe out the previous value for
    // uploaded_csv_file.
    if ($csv_filename) {
       // replace Microsoft Word version of single  and double quotations marks (“ ” ‘ ’) with  regular quotes (' and ")
      $field_values['uploaded_csv_file'] = islandora_digital_workflow_convert_smart_quotes($uploaded_csv_file);
/*
      $field_values['uploaded_csv_file'] = mb_check_encoding($field_values['uploaded_csv_file'], 'UTF-8') ?
          $field_values['uploaded_csv_file'] : utf8_encode($field_values['uploaded_csv_file']);
 */
    }
    db_query("SET NAMES utf8");
    if ((!is_array($batch_record))) {
      $batch_id = db_insert('islandora_digital_workflow_batch')
      ->fields($field_values)
      ->execute();
    }
    else {
      $batch_id = db_update('islandora_digital_workflow_batch')
      ->fields($field_values)
      ->condition('batch_name', $batch_name)
      ->execute();
    }
    $updated_batch_record = islandora_digital_workflow_get_batch_record_by_name($batch_name);
    islandora_digital_workflow_sync_node_to_items($updated_batch_record, $node, $csv_filename);
    return $updated_batch_record;
  }
  else {
    return NULL;
  }
}

/**
 * Helper function to prevent bad Microsoft UTF8 quote characters from breaking
 * the SQL insert.  Nobody needs those curly quotes.
 *
 * @param string $string
 *   The text that needs to be converted
 * @return string
 *   The converted string value.
 */
function islandora_digital_workflow_convert_smart_quotes($string) {
  return str_replace(
      array(chr(145),
            chr(146),
            chr(147),
            chr(148),
            chr(151)),
      array("'",
            "'",
            '"',
            '"',
            '-'),
      $string);
}

/**
 * Gets the batch_host values from the module configuration.
 *
 * This function will fetch the config value for "Available Host Aliases for Batch ingest"
 * and split the alias_and_value value at the "=" character and then add items to the return
 * associative array with the actual host value as the array keys and the alias is the value.
 *
 * @return array
 *   batch host values as an array where the key is the actual file path and the
 * value is the friendly alias name that has been configured.
 */
function islandora_digital_workflow_get_batch_host_values() {
  $arr = explode("\r\n", trim(variable_get('islandora_digital_workflow_hosts_for_ingest')));
  $return_array = array(variable_get('islandora_digital_workflow_ingest_prepared_path', '/ingest/islandora_ingest') => 'Default Incoming Ingest Path');

  foreach ($arr as $alias_and_value) {
    @list($alias, $value) = explode("=", $alias_and_value, 2);
    $alias = trim($alias);
    if ($alias && $value) {
      $return_array[$value] = $alias;
    }
  }
  return $return_array;
}

/**
 * Helper function to sync the islandora_digital_workflow_batch_items records
 * to a given workflow_batch node or batch_record.
 *
 * @param array $batch_record
 *   The batch record as an array.
 * @param object $node
 *   Drupal node object.
 * @param string $csv_filename
 *   The filename that points to the CSV file.
*/
function islandora_digital_workflow_sync_node_to_items($batch_record, $node, $csv_filename) {
  if (is_array($batch_record) && isset($node->field_identifiers) &&
      array_key_exists('value', $node->field_identifiers[$node->language][0])) {
    // Set a variable for this for readibility on the subsequent function call.
    $keep_created_MODS_files = ($csv_filename <> '');
    $csv_file_sources = islandora_digital_workflow_prepare_csv2mods($csv_filename, $batch_record, $keep_created_MODS_files);
    if (count($csv_file_sources) > 0) {
      // Delete any pre-existing batch_items records for this batch's batch_id.
      if (db_delete('islandora_digital_workflow_batch_items')
          ->condition('batch_id', $batch_record['batch_id'])
          ->execute()) {
        drupal_set_message('Previous batch item record deleted for "' .
            $batch_record['batch_name'] . '".', 'status');
      }
      foreach ($csv_file_sources as $csv_file_source) {
        islandora_digital_workflow_sync_batch_item_record($batch_record, $csv_file_source);
      }
    }
  }
}

/**
 * Helper function to sync the islandora_digital_workflow_batch_items based on
 * the rows of the uploaded CSV file.
 *
 * @param array $batch_record
 *   The batch record as an array.
 * @param string $csv_filename
 *   The filename that points to the CSV file.
 */
function islandora_digital_workflow_sync_batch_item_record($batch_record, $csv_file_source) {
  if (file_exists($csv_file_source['mods_filename'])) {
    $mods_content = file_get_contents($csv_file_source['mods_filename']);
    $mods_content = islandora_digital_workflow_convert_smart_quotes($mods_content); // mb_check_encoding($mods_content, 'UTF-8') ? $mods_content : utf8_encode($mods_content);
    // unlink($csv_file_source['mods_filename']);
  }
  else {
    $mods_content = '';
  }
  $title = islandora_digital_workflow_convert_smart_quotes($csv_file_source['title']); //mb_check_encoding($csv_file_source['title'], 'UTF-8') ? $csv_file_source['title'] : utf8_encode($csv_file_source['title']);

  db_query("SET NAMES utf8");
  // take each mods_filename and put this into the item record
  $batch_item_id = db_insert('islandora_digital_workflow_batch_items')
      ->fields(array(
        'batch_id' => $batch_record['batch_id'],
        'identifier' => $csv_file_source['identifier'],
        'title' => $title,
        'filename' => $csv_file_source['filename'],
        'mods' => $mods_content,
      ))
      ->execute();
}

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
  $query->fields('bt', array('timestamp'));
  $query->join('islandora_digital_workflow_actions', 'a', 'a.action_id = bt.action_id');
  $query->fields('a', array('description'));
  $query->condition('b.nid', $nid)
      ->orderBy('bt.timestamp', 'DESC');

  $results = $query->execute();
  $rows = $results->fetchAll();
  foreach ($rows as $index => $row) {
    $rows[$index]->how_long_ago = islandora_digital_workflow_timeago_from_timestamp($row->timestamp);
  }
  return $rows;
}

/**
 * This will get all of the transactions records related to a given Drupal batch_item.
 *
 * @param integer $nid
 *   $nid value from a Drupal node object.
 * @return array
 *   Either returns an array of batch record objects or NULL if it cannot be found.
 */
function islandora_digital_workflow_get_batch_item_transactions($batch_item_id) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');

  $query = db_select('islandora_digital_workflow_transactions', 'bt');
  $query->fields('bt', array('timestamp'));
  $query->join('islandora_digital_workflow_actions', 'a', 'a.action_id = bt.action_id');
  $query->fields('a', array('description'));
  $query->condition('bt.batch_item_id', $batch_item_id)
      ->orderBy('bt.timestamp', 'DESC');

  $results = $query->execute();
  $rows = $results->fetchAll();
  foreach ($rows as $index => $row) {
    $rows[$index]->how_long_ago = islandora_digital_workflow_timeago_from_timestamp($row->timestamp);
  }
  return $rows;
}

/**
 * Returns all batch records that have a partial match for the `identifier` field
 * in the islandora_digital_workflow_batch_items table.
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

  // Due to the nature of this query and how stubborn the db_select is with creating
  // a LEFT JOIN query that uses GROUP BY and GROUP_CONCAT to display the needed
  // field values, this is much MUCH MUCH easier to write out as a SQL statement.
  $query = db_query('SELECT bi.identifier, bi.title, bi.filename, 
    GROUP_CONCAT(a.description) as `transaction_actions`, bi.batch_item_id
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
    $rows[$index]->transaction_actions = str_replace(",", "<br>", $row->transaction_actions);
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
  $num_updated = db_update('islandora_digital_workflow_batch_items')
      ->fields(array(
        'title' => $update_values['title'],
        'identifier' => $update_values['identifier'],
        'filename' => $update_values['filename'],
        'mods' => $update_values['mods'],
      ))
      ->condition('batch_item_id', $batch_item_id)
      ->execute();

  if ($num_updated) {
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
      $calculated_mods_filename = str_replace('.' . $orig_filename_pathinfo['extension'], '.xml', $original_values->filename);
      $bytes_written = file_put_contents($calculated_mods_filename, $update_values['mods']);
      if ($bytes_written <> strlen($update_values['mods'])) {
        drupal_set_message('There was a problem saving the updated MODS value to the file system at "' .
          $calculated_mods_filename . '".  It may be a file permissions issue.', 'error');
      }
    }
  }
  return $num_updated;
}


/**
 * Helper function for field to xpath mappings used for creating MODS from CSV.
 *
 * This still provides a test as to whether or not a field gets into the initial XML
 * from the sheet - and the headings title that would match for that field.
 *
 * @return array
 *   The key matches a CSV column heading and the value is the xpath mapping for
 * that field.
 */
function islandora_digital_workflow_get_csv_header_xpath_mappings() {
  return array(
    'genre'                     => '/mods:mods/mods:genre',
    'abstract'                  => '/mods:mods/mods:abstract',
    'description'               => '/mods:mods/mods:abstract',
    'type_of_resource'          => '/mods:mods/mods:typeOfResource',
    'gift_of'                   => '/mods:mods/mods:note[@type="donor"]',
    'address'                   => '/mods:mods/mods:note[@type="address"]',
    'subject'                   => '/mods:mods/mods:subject/mods:topic',
    'contributor'               => '/mods:mods/mods:name/mods:namePart[../mods:role/mods:roleTerm[(text()="contributor") and @type="text"]]',
    'creator'                   => '/mods:mods/mods:name/mods:namePart[../mods:role/mods:roleTerm[(text()="creator") and @type="text"]]',
    'depositor'                 => '/mods:mods/mods:name/mods:namePart[../mods:role/mods:roleTerm[(text()="depositor") and @type="text"]]',
    'identifier'                => '/mods:mods/mods:identifier[@type="pitt"]',
    'source_id'                 => '/mods:mods/mods:identifier[@type="source"]',
    'source_identifier'         => '/mods:mods/mods:identifier[@type="source"]',
    'title'                     => '/mods:mods/mods:titleInfo/mods:title',
    'publisher'                 => '/mods:mods/mods:originInfo/mods:publisher',
    'date_digitized'            => '/mods:mods/mods:originInfo/mods:dateCaptured',
    'format'                    => '/mods:mods/mods:physicalDescription/mods:form',
    'subject_name'              => '/mods:mods/mods:subject[@authority="lcsh"]/mods:name',
    'dimension'                 => '/mods:mods/mods:physicalDescription/mods:extent',
    'source_citation'           => '/mods:mods/mods:relatedItem/note[@type="prefercite"]',
    'subject_lcsh'              => '/mods:mods/mods:subject[@authority="lcsh"]/mods:topic',
    'source_collection_id'      => '/mods:mods/mods:relatedItem[@type="host"]/mods:identifier',
    'subject_local'             => '/mods:mods/mods:subject[@authority="local"]/mods:topic',
    'sort_date'                 => '/mods:mods/mods:originInfo/mods:dateOther[@type="sort"]',
    'subject_location'          => '/mods:mods/mods:subject[@authority="lcsh"]/mods:geographic',
    'date'                      => '/mods:mods/mods:originInfo/mods:dateOther[@type="display"]',
    'scale'                     => '/mods:mods/mods:subject/mods:cartographics/mods:scale',
    'source_collection'         => '/mods:mods/mods:relatedItem/mods:titleInfo/mods:title',
    'copyright_status'          => '/mods:mods/mods:accessCondition/copyrightMD:copyright/@copyright.status',
    'source_container'          => '/mods:mods/mods:relatedItem[@type="host"]/mods:note[@type="container"]',
    'source_ownership'          => '/mods:mods/mods:relatedItem[@type="host"]/mods:note[@type="ownership"]',
    'publication_status'        => '/mods:mods/mods:accessCondition/copyrightMD:copyright/@publication.status',
    'pub_place'                 => '/mods:mods/mods:originInfo/mods:place/mods:placeTerm[@type="text"]',
    'source_collection_date'    => '/mods:mods/mods:relatedItem[@type="host"]/mods:originInfo/mods:dateCreated',
    'normalized_date'           => '/mods:mods/mods:originInfo/mods:dateCreated[@encoding="iso8601" @keyDate="yes"]',
    'rights_holder'             => '/mods:mods/mods:accessCondition/copyrightMD:copyright/copyrightMD:rights.holder/copyrightMD:name',
    'normalized_date_qualifier' => '/mods:mods/mods:originInfo/mods:dateCreated[@qualifier="approximate" @encoding="iso8601" @keyDate="yes"]',
    // NEW fields for Oral Histories
    'interviewee'               => '',
    'interviewer'               => '',
    'parent_id'                 => '', // same as parent_identifier
    'parent_identifier'         => '', // same as parent_id
    // These field values are not represented in a MODS file, but will appear
    // in the CSV file.
    'batch' => '',
    'collection' => '',
    'file_name' => '',
    'filename' => '',
    'location' => '',
    // NEW fields that will appear in the CSV file, but not to be transformed to
    // a MODS node.
    'path_to_master_wav' => '',
  );
}

/**
 * Helper function to get a field value/s from any Drupal table.
 *
 * @param string $table_name
 *   Name of the table.
 * @param string $option_name
 *   Optional - name of the field to pull from the table (if not the primary key field).
 * @param string $key_field
 *   Name of the primary key field of the table.
 * @param string $value_field
 *   Additional field name to pull from the table.
 * @param string $sort_field
 *   Optional - name of field to sort by.
 * @param integer $option_value
 *   Optional - which option value to return when selecting more than one field.
 * @return array
 *   Returns an array of objects that represent database rows.
 */
function islandora_digital_workflow_get_mysql_options($table_name, $option_name = '', $key_field, $value_field, $sort_field = '', $option_value = 0) {
  $query = 'SELECT ' . $key_field . ', ' . $value_field . ' FROM ' . $table_name . ($sort_field ? ' ORDER BY ' . $sort_field : '');
  $results = db_query($query);
  $rows = $results->fetchAllAssoc($key_field);

  if ($option_name) {
    array_unshift($rows, array($option_value => 'Select ' . $option_name));
  }
  return $rows;
}

/**
 * Gets any values from Solr for a given query.
 *
 * @param string $solr_query
 *   The Solr query to run.
 * @param string $key_field
 *   The field to return.
 * @param string $value_field
 *   Additional value field to return
 * @param string $sort_field
 *   Optional - field to sort results by.
 * @return array
 *   Returns an array of values where the distinct set is keyed on the $key_field.
 */
function islandora_digital_workflow_get_solr_options($solr_query, $key_field, $value_field, $sort_field = '') {
  $query_processor = new IslandoraSolrQueryProcessor();
  $query_processor->solrQuery = $solr_query;
  if ($sort_field) {
    $query_processor->sort = $sort_field;
  }
  $query_processor->solrStart = 0;
  $query_processor->solrLimit = 40000;
  $query_processor->solrParams = array('fl' => $key_field . (($value_field <> $key_field) ? ',' . $value_field : ''));

  $url = parse_url(variable_get('islandora_solr_url', 'localhost:8080/solr'));
  $solr = new Apache_Solr_Service($url['host'], $url['port'], $url['path'] . '/');
  $solr->setCreateDocuments(FALSE);
  $results = array();
  try {
    $search_results = $solr->search($query_processor->solrQuery, $query_processor->solrStart, $query_processor->solrLimit, $query_processor->solrParams, 'GET');
    $tmp = json_decode($search_results->getRawResponse(), TRUE);
    if ($tmp['response']['numFound'] > 0) {
      foreach ($tmp['response']['docs'] as $rec) {
        $results[$rec[$key_field]] = $rec[$value_field];
      }
    }
  }
  catch (Exception $e) {
  }
  return $results;
}

/**
 * Gets Solr fields for multiple fields and can be limited to a specific number
 * of results.
 *
 * @param string $solr_query
 *   The Solr query to run.
 * @param string $key_field
 *   The field to return.
 * @param string $value_fields_csv
 *   Additional values field to return (multiple fields separated by commas).
 * @param string $sort_field
 *   Optional - field to sort results by.
 * @param integer $limit
 *   Optional - how many results to return.
 * @return array
 *   Returns an array of results based on parameters.
 */
function islandora_digital_workflow_get_solr_values($solr_query, $key_field, $value_fields_csv, $sort_field = '', $limit = 5000) {
  $query_processor = new IslandoraSolrQueryProcessor();
  $query_processor->solrQuery = $solr_query;
  if ($sort_field) {
    $query_processor->sort = $sort_field;
  }
  $query_processor->solrStart = 0;
  $query_processor->solrLimit = $limit;
  $query_processor->solrParams = array('fl' => $key_field . (($value_fields_csv <> $key_field) ? ',' . $value_fields_csv : ''));

  $url = parse_url(variable_get('islandora_solr_url', 'localhost:8080/solr'));
  $solr = new Apache_Solr_Service($url['host'], $url['port'], $url['path'] . '/');
  $solr->setCreateDocuments(FALSE);
  $results = array();
  try {
    $search_results = $solr->search($query_processor->solrQuery, $query_processor->solrStart, $query_processor->solrLimit, $query_processor->solrParams, 'POST');
    $tmp = json_decode($search_results->getRawResponse(), TRUE);
    if ($tmp['response']['numFound'] > 0) {
      foreach ($tmp['response']['docs'] as $rec) {
        $results[$rec[$key_field]] = $rec;
      }
    }
  }
  catch (Exception $e) {
  }
  return $results;
}

/**
 * Helper function to fetch the possible Sites objects from Solr.
 *
 * If no values are found, an array is still returned so that will display in the
 * dropdown box in the Create Batch form.
 *
 * @return array
 *   Returns an array of PID values that point to siteCModel objects from Solr.
 */
function islandora_digital_workflow_get_sites_options() {
  $sites = islandora_digital_workflow_get_solr_options('RELS_EXT_hasModel_uri_ms:*siteCModel', 'PID', 'fgs_label_s');
  if (count($sites) < 1) {
    $sites[0] = 'No sites objects configured.';
  }
  return $sites;
}

/**
 * This will return the ingest path for a given batch by combining the batch's host setting
 * and the batch_name.  If the folder does not exist, it will be created.  It will be
 * named 'batch_{$batch_name}'.
 *
 * In the case when the batch is coming from the ftp to box.com, the system will use
 * the return the Default Ingest Path (configured admin/islandora/workflow) and the files
 * would the downloaded and extracted to this path from "box.com"
 *
 * Note about the folder permissions:
 *   The folder must be writable by apache as well as by other admin members of our department.
 *
 * @param array $batch_values
 *   Array of values from the islandora_digital_workflow_batch table that represents
 * a single batch.
 * @return string
 *   The batch_path for the given batch_values record.
 */
function islandora_digital_workflow_batch_path($batch_values) {
  if (is_object($batch_values)) {
    $batch_values = (array) $batch_values;
  }
  if (!is_array($batch_values) || !array_key_exists('batch_name', $batch_values)) {
    drupal_set_message('The batch path could not be determined.  Either the values are missing or ' .
            'there is no value for the batch "Name" (batch_name field) yet.', 'warning');
    return '';
  }
  $ingest_path = $batch_values['batch_host'];
  if (!$batch_values['batch_host'] || $ingest_path == 'ftp' || (ltrim($ingest_path, 'ftp:') <> $ingest_path)) {
    $ingest_path = variable_get('islandora_digital_workflow_ingest_prepared_path', '/ingest/islandora_ingest');
  } elseif (strstr($ingest_path, ":")) {
    @list($ingest_host, $ingest_path) = explode(":", $ingest_path);
    drupal_set_message('This batch is configured to use files from a different server.  ' .
        'That server needs to be configured as a mounted folder under the /ingest ' .
        'directory and the ' . l('Available Host Alias', '/admin/islandora/islandora_digital_workflow') .
        ' configuration would need to be updated to point to the mounted location.',
        'error');
  }
  $batch_path = rtrim($ingest_path, '/') . '/' . $batch_values['batch_name'];
  if (!file_exists($batch_path)) {
    $call = mkdir($batch_path);
    if (!$call) {
      drupal_set_message('The batch folder "' . $batch_path . '" could not be created.', 'error');
    }
  }

  return (file_exists($batch_path)) ? $batch_path : '';
}

/**
 * Helper function to return the value of the
 * islandora_digital_workflow_ingest_namespace - with the right trailing colon.
 *
 * @return string
 *   The sanitized ingest_namespace value.
 */
function islandora_digital_workflow_get_ingest_namespace() {
  return rtrim(variable_get('islandora_digital_workflow_ingest_namespace', 'islandora'), ':') . ':';
}

/**
 * This will call the islandora_get_content_model_names and reduce the options
 * to the possible models for digital scanning purposes.
 *
 * @return array
 *   List of Islandora ~ fedora models available for digital scanning processes.
 */
function islandora_digital_workflow_get_reduced_cmodels() {
  module_load_include('inc', 'islandora', 'includes/content_model.autocomplete');
  $models = islandora_get_content_model_names();
  
  // If they are set, remove these models:
  $remove_models = array(
    'ir:citationCModel',
    'islandora:sp-audioCModel',
    'islandora:collectionCModel',
    'islandora:compoundCModel',
    'islandora:sp_disk_image',
    'islandora:entityCModel',
    'islandora:eventCModel',
    'islandora:newspaperPageCModel',
    'islandora:oralhistoriesCModel',
    'islandora:organizationCModel',
    'islandora:sp_pdf',
    'islandora:pageCModel',
    'islandora:personCModel',
    'islandora:placeCModel',
    'islandora:sp_videoCModel',
    'islandora:manuscriptPageCModel',
    'ir:thesisCModel',
    'islandora:transformCModel'
  );
  foreach ($remove_models as $entry) {
    unset($models[$entry]);
  }
  
  /* Possible future inclusion for these models:
      islandora:sp-audioCModel
      islandora:compoundCModel
      islandora:bookCModel
      islandora:oralhistoriesCModel
      islandora:sp_videoCModel
      islandora:sp_disk_image
   */

  // For readability purposes, strip out the "islandora:" prefix for each
  foreach ($models as $key => $model) {
    if (substr($model, 0, 10) == 'Islandora ') {
      $models[$key] = substr($model, 10);
    }
  }

  // Sort these models by the readable names.
  asort($models);

  $return_models = array();
  // When used as checkboxes elements in a form, #options array cannot have a 0 key.
  $i = 1;
  foreach ($models as $model) {
    $return_models[$i] = $model;
    $i++;
  }

  return $return_models;
}

/**
 * Returns all content types for the current Islandora deployment.
 *
 * This will also strip off the " Content Model" suffix.
 *
 * @return array
 *   the key for each item is the Islandora content model
 *   the value for each item is the readable name
 */
function islandora_digital_workflow_get_reduced_readable_cmodels() {
  $models = islandora_digital_workflow_get_reduced_cmodels();

  // Loop through the $models and update $models to make it more readable
  $between_parenthesis_regex = '#\((.*?)\)#';
  $return_models = array();
  $suffix = " Content Model";
  foreach ($models as $idx => $model_name_readable_and_model_name) {
    // Get the key name from $all_models -- $index SHOULD match an item, but
    // safer to check.
    preg_match($between_parenthesis_regex, $model_name_readable_and_model_name, $matches);
    if (count($matches) > 0) {
      $model_name = $matches[1];
      $model_name_readable = trim(str_replace('(' . $model_name . ')', '', $model_name_readable_and_model_name));
      // TODO: check user permissions here for this $model_name before adding.
      $return_models[$model_name] = str_replace($suffix, "", $model_name_readable);
    }
  }
  return $return_models;
}

/**
 * This will get the Islandora model name given any content type name.
 *
 * @param array $all_models
 *   Associative array - see islandora_digital_workflow_get_reduced_readable_cmodels()
 * @param string $readable_content_type
 *   The human readable content type name (used for Taxonomy among other things).
 * @return string
 */
function islandora_digital_workflow_get_cmodels_from_readable_name($all_models, $readable_content_type) {
  $readable_content_type = str_replace(" Content Model", "", $readable_content_type);
  if (!(array_search($readable_content_type, $all_models) === FALSE)) {
    return array_search($readable_content_type, $all_models);
  }
  else {
    return '';
  }
}

/**
 * Performs validation on a CSV file to see that it can be used with a batch.
 *
 * @param string $csv_filename
 *   Filename pointing to the CSV file.
 * @return boolean
 *   Whether or not the CSV file is valid for a batch.
 */
function islandora_digital_workflow_validate_csv($csv_filename) {
  $validation_success = TRUE;
  $csv_file_arr = islandora_digital_workflow_csv_file_as_array($csv_filename);
  $file_header = (isset($csv_file_arr['header']) ? $csv_file_arr['header'] : array());
  $file_rows = (isset($csv_file_arr['rows']) ? $csv_file_arr['rows'] : array());

  // Get the normalized_date column index.
  $date_index = array_search('normalized_date', $file_header);

  // Get the filename column index.
  $filename_index = array_search('file_name', $file_header);
  // If no 'file_name' is not found, check the other field name 'filename'.
  if ($filename_index === FALSE) {
    $filename_index = array_search('filename', $file_header);
  }

  foreach ($file_rows as $k => $row) {
    /** DO NOT DO RELATIVE FILE CHECKING ON THE UPLOADED CSV FILES!
    // If there is a filename_index, check that the file exists.
    if (!$filename_index === FALSE) {
      $row_filename = $row[$filename_index];
      if (!file_exists($row_filename)) {
        $validation_success = FALSE;
        drupal_set_message(t('There is a filename referenced that does not exist on ' .
          'the file system.  "' . $row[$filename_index] . '" on row #' . $k . '.  ' .
          'Please upload fix this issue to be able to ingest this batch.'), 'error');
      }
    }
    */
    // If there is a normalized_date in the header, check this row's date value
    if (!($date_index === FALSE)) {
      $normalized_date = $row[$date_index];
      $check = islandora_digital_workflow_make_custom_csv_dates($row[$date_index], TRUE);
      if ($check == '') {
        $validation_success = FALSE;
        drupal_set_message(t('There is a bad date value for "normalized_date" = "' .
          $row[$date_index] . '" on row #' . $k . '.  Please upload the CSV after ' .
          'fixing this value.'), 'error');
      }
    }
  }

  return $validation_success;
}

/**
 * Helper function that will load a CSV spreadsheet by name $csv_file, and return
 * it as an associative array.  This will make sure that the headers are all lowercase.
 *
 * @param string $csv_file
 *   Filename pointing to the CSV file.
 * @return array
 *   The array contains two keys that each contain an array of values:
 *     ('header' => $file_header, 'rows' => $file_rows).
 * @throws Exception
 */
function islandora_digital_workflow_csv_file_as_array($csv_file) {
  $file_rows = $file_header = array();
  if (file_exists($csv_file)) {
    $row = 0;
    if (($handle = fopen(drupal_realpath($csv_file), "r")) !== FALSE) {
      try {
        while (($data = fgetcsv($handle)) !== FALSE) {
          $row++;
          if ($row > 1) {
            $file_rows[] = $data;
          }
          else {
            $tmp_data = $data;
            foreach ($tmp_data as $i => $val) {
              $tmp_data[$i] = str_replace(" ", "_", trim(strtolower($val)));
            }
            $file_header = $tmp_data;
          }
        }
      }
      catch (Exception $e) {
        throw new Exception('The file "'.$csv_file.'" could not be parsed as a CSV.');
      }
      fclose($handle);
    }
  }
  return array('header' => $file_header, 'rows' => $file_rows);
}

/**
 * Helper function to get a valid date value from CSV file.
 *
 * Will handle the CSV values for dates and create a "display date" and a "sort date" value as best as it can assuming:
 *      CSV value		Display date				Sort date
 * -------------------------------------------------------------------------------------------------
 *	1945/1955		1945-1955				1945-01-01T00:00:00-05:00
 * a	1945/1955 		ca. 1945-1955				1945-01-01T00:00:00-05:00
 *	1945-03/1955-04 	March 1945- April 1955			1945-03-01T00:00:00-05:00
 * a	1945-03/1955-04 	ca. March 1945- April 1955		1945-03-01T00:00:00-05:00
 * 	1945-03-05/1955-04-23	March 5, 1945- April 23, 1955		1945-03-05T00:00:00-05:00
 * a	1945-03-05/1955-04-23 	ca. March 5, 1945- April 23, 1955	1945-03-05T00:00:00-05:00
 *
 * @param string $short_date
 *   The value of the date to check.
 * @param boolean $suppress_display_messages
 *   Optional - whether or not to display messages of what was wrong with the date.
 * @return string
 *   The sanitized date value that can be used for the Islandora object.
 */
function islandora_digital_workflow_make_custom_csv_dates($short_date, $suppress_display_messages = FALSE) {
  $short_date = trim($short_date);
  $short_date_length = strlen($short_date);
  $h_format = 'F j, Y';
  $from_date = '';
  // Handle the special cases where there is a date range provided
  if ($short_date_length == 9 && ($short_date[4] == '/' || $short_date[4] == '-')) {
    $from_date = substr($short_date, 0, 4) . '-01-01';
    $to_date = substr($short_date, 5, PHP_INT_MAX) . '-01-01';
    $h_format = 'Y';
  }
  elseif ($short_date_length == 15 && ($short_date[7] == '/' || $short_date[7] == '-')) {
    $from_date = substr($short_date, 0, 7);
    $to_date = substr($short_date, 8, PHP_INT_MAX);
    $h_format = 'F Y';
  }
  elseif ($short_date_length == 21 && ($short_date[10] == '/' || $short_date[10] == '-')) {
    $from_date = substr($short_date, 0, 10);
    $to_date = substr($short_date, 11, PHP_INT_MAX);
  }
  if ($from_date) {
    $timestamp = strtotime($from_date);
    $timestamp_to_date = strtotime($to_date);
    $display_date = date($h_format, $timestamp) . '-' . date($h_format, $timestamp_to_date);
    return array(date('c', $timestamp), $display_date);
  }
  if (strstr($short_date, '-') == '') {
    if ($short_date_length == 4) {
      $short_date .= '-01-01';
      $h_format = 'Y';
    }
    elseif ($short_date_length == 6) {
      $short_date = substr($short_date, 0, 4) . '-' . substr($short_date, 4, 2) . '-01';
      $h_format = 'F Y';
    }
    elseif ($short_date_length <> 8 && !$from_date) {
      if ($short_date <> '') {
        if (!$suppress_display_messages) {
          drupal_set_message('bad date format for date value of "' . $short_date . '".', 'error');
        }
        return '';
      }
      else {
        return '';
      }
    }
  }
  else {
    if ($short_date_length == 7) {
      $short_date .= '-01';
      $h_format = 'F Y';
    }
    elseif ($short_date_length <> 10 && !$from_date) {
      if (!$suppress_display_messages) {
        drupal_set_message('bad date format for date value of "' . $short_date . '".', 'error');
      }
      return '';
    }
  }
  $timestamp = strtotime($short_date);
  return array(date('c', $timestamp), date($h_format, $timestamp));
}

function islandora_digital_workflow_get_csv_values_for_fieldname($fieldname, $csv_array) {
  $ret_array = array();
  // Make sure that there is at least one data row and that at least the identifier
  // and the column specified by $fieldname exists.
  if (isset($csv_array['header']) && (!array_search('identifier', $csv_array['header']) === FALSE) &&
      (!array_search($fieldname, $csv_array['header']) === FALSE) &&
      is_array($csv_array['rows']) && count($csv_array['rows']) > 1) {
    $index_of_fieldname = array_search($fieldname, $csv_array['header']);
    foreach ($csv_array['rows'] as $row_array) {
      $ret_array[$fieldname][] = isset($row_array[$index_of_fieldname]) ? $row_array[$index_of_fieldname] : '';
    }
  }
  return $ret_array;
}

/**
 * This returns the table of checkboxes based on the islandora models.
 *
 * Checkboxes are checked based on the islandora_digital_workflow_islandora_models
 * variable.  Also, this will check whether or not the module is installed 
 * (see: islandora_digital_workflow_model_ingest_module_info).
 *
 * @return array
 *   The array of Drupal form definitions.
 */
function islandora_digital_workflow_models_table() {
  $readable_models = islandora_digital_workflow_get_reduced_readable_cmodels();
  $configured_models = $return_form_elements = array();
  $configured_supported_models = variable_get('islandora_digital_workflow_islandora_models', array());
  foreach ($readable_models as $cmodel => $model) {
    // Each model depends on a specific module in order to be able to ingest.
    // The models that are not supported should be disabled.
    $this_model_module_info = islandora_digital_workflow_model_ingest_module_info($model);
    $title = ($this_model_module_info['exists']) ? $model : $model . ' (not installed)';
    $configured_models[$cmodel] = array(
      '#type' => 'checkbox',
      '#title' => $title,
      // Mark the checkboxes as checked based on islandora_digital_workflow_islandora_models
      '#default_value' => isset($configured_supported_models[$cmodel]) ? (($configured_supported_models[$cmodel] <> 0) ? 1 : 0) : 0,
      '#prefix' => '<div class="major-item">',
      '#suffix' => '</div>',
    );
    if (!$this_model_module_info['exists']) {
      $configured_models[$cmodel]['#default_value'] = 0;
      drupal_set_message('The module is not installed that can ingest ' . $model . ' objects, ' .
          ' so they can not be managed using the workflow until that module is installed (' .
          $this_model_module_info['git_link'] . ', ' . $this_model_module_info['uls_git_link'] .
          ').');
      $configured_models[$cmodel]['#disabled'] = TRUE;
      $configured_models[$cmodel]['#attributes'] = array('readonly' => 'readonly', 'disabled' => 'disabled');

    }
    $return_form_elements[str_replace(" ", "+", $model)] = $configured_models[$cmodel];
  }
  return $return_form_elements;
}

/**
 * Returns the information for each model's ingest module.
 *
 * @param array $model
 *   The array will contain the following key values:
 *   'exists' - whether or not the module exists (is installed)
 *   'module_name' - the name of the module
 *   'git_link' - link to the git repository link to the module code.
 *   'uls_git_link' - link to the git repository link to the ulsdevteam module code.
 */
function islandora_digital_workflow_model_ingest_module_info($model) {
  // The easiest way to achieve this is to use a switch case -- this may be rewritten
  // to have a configurable set of models => module_names for preparing the batch
  // for each model type, but that is potentially a big can of worms since each module
  // replacement could potentially have a different batch prepare function name.
  $retval = array(
    'exists' => FALSE,
    'module_name' => '',
    'git_link' => '',
    'uls_git_link' => '',
  );
  switch ($model) {
    case "Image": case "Basic Image": case "Large Image":
      $retval['module_name'] = 'islandora_batch';
      $retval['git_link'] = l($retval['module_name'], 'https://github.com/Islandora/islandora_batch.git');
      $retval['uls_git_link'] = l($retval['module_name'], 'https://github.com/ulsdevteam/islandora_batch.git');
      break;
    case "Finding Aid":
      $retval['module_name'] = 'islandora_batch';
      $retval['git_link'] = l($retval['module_name'], 'https://github.com/Islandora/islandora_book_batch.git');
      $retval['uls_git_link'] = l($retval['module_name'], 'https://github.com/ulsdevteam/islandora_book_batch.git');
      break;
    case "Internet Archive Book": case "Book":
      $retval['module_name'] = 'islandora_book_batch';
      $retval['git_link'] = l($retval['module_name'], 'https://github.com/Islandora/islandora_book_batch.git');
      $retval['uls_git_link'] = l($retval['module_name'], 'https://github.com/ulsdevteam/islandora_book_batch.git');
      break;
    case "Manuscript":
      $retval['module_name'] = 'islandora_manuscript_batch';
      $retval['git_link'] = l($retval['module_name'], '//');
      $retval['uls_git_link'] = l($retval['module_name'], 'https://github.com/ulsdevteam/islandora_manuscript_batch.git');
      break;
    case "Newspaper": case "Newspaper Issue": case "Newspaper/Serial":
      $retval['module_name'] = 'islandora_newspaper_batch';
      $retval['git_link'] = l($retval['module_name'], 'https://github.com/Islandora/islandora_newspaper_batch.git');
      $retval['uls_git_link'] = l($retval['module_name'], 'https://github.com/ulsdevteam/islandora_newspaper_batch.git');
      break;
    case "Web ARChive":
      $retval['module_name'] = 'islandora_batch';
      $retval['git_link'] = l($retval['module_name'], 'https://github.com/Islandora/islandora_book_batch.git');
      $retval['uls_git_link'] = l($retval['module_name'], 'https://github.com/ulsdevteam/islandora_book_batch.git');
      break;
  }
  $retval['exists'] = module_exists($retval['module_name']);
  return $retval;
}

/**
 * This will parse a row from a CSV file and return an array with two parts to the info:
 *
 * $prepared_filepath = The path to the folder that contains the MODS and image TIF.
 * $identifier - the identifier value (if provided in the spreadsheet).
 *
 * @param type $input_filename
 * @param type $batch_record
 * @param type $keep_created_MODS_files
 * @return mixed
 *   Either returns FALSE or an array that contains an array of values related
 * to each of the CSV rows.  Each array element contains the following fields:
 *  'title' - the Title for the row's object
 *  'identifier' - the PID for the row
 *  'filename' - the filename to the MASTER / OBJ resource
 *  'mods_filename' - the filename where the MODS has been saved.
 */
function islandora_digital_workflow_prepare_csv2mods($input_filename, $batch_record, $keep_created_MODS_files = FALSE) {
  if (!$input_filename) {
    return array();
  }

  $batch_path = islandora_digital_workflow_batch_path($batch_record);

  $default_depositor = $batch_record['default_depositor'];
  // TODO: capture batch-level values for default_genre, batch_default_CR_status,
  // batch_default_pub_status, and batch_default_CR_holder -- to potentially set
  // when creating MODS below.
  ini_set('auto_detect_line_endings', true);
  $outputFilename   = 'MODS.xml';
  // Open csv to read
  $inputFile  = fopen(drupal_realpath($input_filename), 'rt');

  // Get the headers of the file
  $headers = fgetcsv($inputFile);
  if ((array_search('identifier', $headers) === FALSE) and (array_search('Identifier', $headers) === FALSE)) {
    return FALSE;
  }

//  $working_directory = dirname(drupal_realpath($input_filename));

  echo "<h3>Working directory " . $batch_path . "</h3>";

  $st = " style='background-color:#F2F2F2;border:1px solid #222'";  $toggle = false;  $row_counter = 1;

  $csv_header_xpath_mappings = islandora_digital_workflow_get_csv_header_xpath_mappings();
  $mods_created = $files_copied = $bytes_written = 0;

  $ingest_namespace = islandora_digital_workflow_get_ingest_namespace();
  $identifiers_and_filepaths = array();
  $auto_number = 0;
  // Loop through each row creating a <row> node with the correct data
  while (($row = fgetcsv($inputFile)) !== FALSE) {
    // $row = array_map("utf8_encode", $row);
    // First, scan the entire row to get the identifier and the file_name value
    $toggle = !$toggle;
    echo '<div' . ($toggle ? $st : " style='border:1px solid #222'") . '>';
    echo "<h3>row " . $row_counter . "</h3>";
    $title = $row_file_name = $row_mods_id = '';
    $rowcopy = $row;

    // Loop through the row to get the $row_mods_id value before further 
    // processing since this field may be the last one in the row.
    foreach($headers as $i => $header) {
      $header = str_replace(" ", "_", strtolower(trim($header)));
      // Some partner decided to end the row of headers with a few commas like "depositor,,,,," - so must
      // check that the header actually has a value.
      if ($header && !$row_mods_id) {
        if (array_key_exists($i, $row)) {
          if ($header == 'identifier') {
            $row_mods_id = trim($row[$i]);
          }
        }
      }
    }

    if (!$row_mods_id) {
      $auto_number++;
      $row_mods_id = str_pad($auto_number, 4, '0', STR_PAD_LEFT);
    }
    foreach($headers as $i => $header) {
      // ALL LOWERCASE, replace spaces with "_"
      $header = str_replace(" ", "_", strtolower(trim($header)));
      // Some partner decided to end the row of headers with a few commas like "depositor,,,,," - so must
      // check that the header actually has a value.
      if ($header) {
        if (array_key_exists($i, $row)) {
          if ($header == 'identifier') {
            if ($keep_created_MODS_files) {
              $mods_directory_path = $batch_path . '/' . $row_mods_id;
              if (!file_exists($mods_directory_path)) {
                mkdir($mods_directory_path);
              }
            }
          }
          elseif (($header == 'file_name' || $header == 'filename') && array_key_exists($i, $row)) {
            $row_file_name = $batch_path . '/' . $row_mods_id . '/' . $row[$i];
            $outputFilename = str_replace(array('.tiff', '.tif'), '.xml', $row[$i]);
          }
          elseif ($header == 'normalized_date' && array_key_exists($i, $row)) {
            list($sort_date, $date) = islandora_digital_workflow_make_custom_csv_dates($row[$i], FALSE);
            // Add the sort_date field and solr 'date' field
            if ((array_key_exists('sort_date', $headers) === FALSE)) {
              $headers[] = 'sort_date';
              $rowcopy[] = $sort_date;
            }
            if ((array_key_exists('date', $headers) === FALSE)) {
              $headers[] = 'date';
              $rowcopy[] = $date;
            }
            islandora_digital_workflow_fix_if_csv_date_approximate($headers, $rowcopy);
          }
          elseif ($header == 'title') {
            $title = $row[$i];
          }
        }
      }
      $headers[$i] = $header;
    }
    // echo "<pre>".print_r($rowcopy, true)."</pre>";

    // MOVE the file into the row's working directory
    if ($row_file_name) {
      if (file_exists($row_file_name)) {
        $command = "cp $row_file_name $batch_path" . '/' . $row_mods_id . '/.';
        exec($command);
        echo "COPY $row_file_name to $batch_path" . '/' . $row_mods_id ."/.<br>";
        $files_copied++;
      }
    }

    // Create a new dom document with pretty formatting
    $doc  = new DOMDocument('1.0', 'utf-8'); // DomDocument();
    $doc->formatOutput   = true;

    // Start creating the MODS XML by adding the root node to the document.
    $root = $doc->createElement('sheet'); // mods:mods'); // ,'test value');
    $root->appendChild($doc->createAttribute('xmlns:xsi'))->appendChild($doc->createTextNode("http://www.w3.org/2001/XMLSchema-instance"));

    $root = $doc->appendChild($root);
    $child = NULL;
    $depositor_in_CSV = FALSE;
    foreach($headers as $i => $header) {
      if (array_key_exists($header, $csv_header_xpath_mappings)) {
        if (is_array($rowcopy) && array_key_exists($i, $rowcopy) && $rowcopy[$i]) {
          $text_node_value = $rowcopy[$i]; // mb_check_encoding($rowcopy[$i], 'UTF-8') ? $rowcopy[$i] : utf8_encode($rowcopy[$i]);
          $child = $doc->createElement($header);
          $child = $root->appendChild($child);
          $value = $doc->createTextNode($text_node_value);
          $value = $child->appendChild($value);
          if (!$depositor_in_CSV && $header == 'depositor') {
            $depositor_in_CSV = TRUE;
          }
        }
      }
    }
    if ($child) {
      $root->appendChild($child);
    }

    // After the CSV row has been processed, as long as the depositor value was not provided in that row,
    // and the batch provided a "default_depositor" value, then create this node now.
    if (!$depositor_in_CSV && $default_depositor && !array_key_exists('depositor', $headers)) {
      $child = $doc->createElement('depositor');
      $child = $root->appendChild($child);
      $value = $doc->createTextNode($default_depositor);
      $value = $child->appendChild($value);
    }
    $strxml = $doc->saveXML();

    echo "<blockquote><pre style='color:#c55'>".htmlspecialchars($strxml) ."</pre></blockquote>";

    if ($keep_created_MODS_files) {
      $handle = fopen($batch_path . '/' . $row_mods_id . '/' . $outputFilename, "w");
      $bytes_written += fwrite($handle, $strxml);
      fclose($handle);
    }
    echo "</div>";
    $filename = ($keep_created_MODS_files) ? islandora_digital_workflow_XMLtoMODS($batch_path . '/' . $row_mods_id . '/' . $outputFilename) : "";
    $mods_created += ($keep_created_MODS_files) ? ($filename <> '') : 1;
    echo "<p>created MODS from spreadsheet for <b>" . $row_mods_id . "</b></p>";

    $identifiers_and_filepaths[] = array(
        'title' => $title,
        'identifier' => $ingest_namespace . $row_mods_id,
        'filename' => $row_file_name,
        'mods_filename' => $filename);
    $row_counter++;
  }
  echo "<p>" . number_format($files_copied) . " files copied to incoming directory <b>" . $batch_path . "</b></p>";
  echo "<p>" . number_format($mods_created) . " MODS files created.</p>";

  return $identifiers_and_filepaths;
}

/**
 * Helper function for creating MODS from CSV - this will inspect the active row for the value in the
 * 'normalized_date_qualifier' field to see whether or not the date field value supplied row is approximate.
 *
 * @param array $headers
 *   An array of headers that match the CSV file so that the correct date field
 * can be found and updated if needed.
 * @param array $row
 *   VARIABLE parameter - can be changed to an approximate date value.
 */
function islandora_digital_workflow_fix_if_csv_date_approximate($headers, &$row) {
  $normalized_date_qualifier_index = array_search('normalized_date_qualifier', $headers);
  $date_index = array_search('date', $headers);
  if ($normalized_date_qualifier_index && isset($row[$normalized_date_qualifier_index])) {
    if ($row[$normalized_date_qualifier_index] == 'yes' || $row[$normalized_date_qualifier_index] == 'approximate') {
      $row[$date_index] = 'ca. ' . $row[$date_index];
    }
  }
}

/**
 * Helper function to perform XML to MODS transformation and save resultant MODS
 * over the original file.
 *
 * This same effect could be achieved using a template system because xslt is
 * relatively slow.  For now, the xslt transform method is used.
 *
 * @param string $xml_filename
 *   Filename that points to the CSV "sheet" xml file.  NOTE:  This file will be
 * updated if the transform can run.
 * @return string
 *   the filename for the new MODS file.
 */
function islandora_digital_workflow_XMLtoMODS($xml_filename) {
  $xsl = str_replace("/includes", "", dirname(__FILE__).'/transforms/sheet2mods.xsl');

  $xml_file = islandora_digital_workflow_convert_smart_quotes(file_get_contents($xml_filename));
  $MODS = ($xml_file) ? islandora_digital_workflow_runXslTransform(
            array(
              'xsl' => $xsl,
              'input' => $xml_file,
            )
          ) : '';

  // This file must be deleted in the process function that called this.
  $bytes_written = file_put_contents($xml_filename, $MODS);

  echo "<h3>after XSL transform</h3><blockquote><pre style='color:#28f'>" . htmlspecialchars(print_r($MODS, true)) . "</pre></blockquote>";

  return ($bytes_written) ? $xml_filename : '';
}

/**
 * Helper function to run an XSL transform on the provided $info['input'] file.
 *
 * @param array $info
 *   This array contains the xsl value and the input (xml) filenames.
 * @return string
 *   The content of the transformed result.
 */
function islandora_digital_workflow_runXslTransform($info) {
  $xsl = new DOMDocument('1.0', 'utf-8');
  $xsl->load($info['xsl']);
  $input = new DOMDocument('1.0', 'utf-8');
  $input->loadXML($info['input']);

  $processor = new XSLTProcessor();
  $processor->importStylesheet($xsl);
/*
  if (isset($info['php_functions'])) {
    $processor->registerPHPFunctions($info['php_functions']);
  }
*/
  return $processor->transformToXML($input);
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
 * Helper function to highight any instances of any word in the provided text.
 *
 * @param string $text
 *   The text that needs to be highlighted.
 * @param string $word
 *   The word to highlight in that text.
 * @param boolean $case_sensitive
 *   Optional - whether or not the search is case-sensitive.  Defaults to FALSE.
 * @return string
 *   The modified markup that could contain highlights.
 */
function islandora_digital_workflow_highlight($text, $word, $case_sensitive = FALSE) {
  $highlighted_text = "<span class='highlighted'>$word</span>";
  $text = ($case_sensitive) ?
      str_replace($word, $highlighted_text, $text) :
      str_ireplace($word, $highlighted_text, $text);
  preg_match_all("'<a.*?href=\"(.*?)\"[^>]*?>(.*?)</a>'si", $text, $matches);

  foreach ($matches[1] as $messed_up_link) {
    $updated_link = str_replace("<span class='highlighted'>" . $word . "</span>", $word, $messed_up_link);
    if ($messed_up_link <> $updated_link) {
      $text = str_replace($messed_up_link, $updated_link, $text);
    }
  }
  return $text;
}


/**
 * This will return a human readable version of "how long ago" for a given timetamp.
 *
 * @param integer $timestamp
 *   Linux timestamp from a MySQL record.
 * @return string
 *   The human readable "how long ago" value.
 */
function islandora_digital_workflow_timeago_from_timestamp($timestamp) {
  $dbDate = new DateTime($timestamp);
  $currDate = new DateTime(date('Ymd H:i:s'));
  $interval = $currDate->diff($dbDate);
  $ago_arr = array();
  if ($interval->y > 0) {
    $ago_arr[] = $interval->y . " yr" . ($interval->y == 1 ? '' : 's');
  }
  if ($interval->m > 0) {
    $ago_arr[] = $interval->m . " mo";
  }
  if ($interval->d > 0) {
    $ago_arr[] = $interval->d . " d";
  }
  if ($interval->h > 0) {
    $ago_arr[] = $interval->h . " h";
  }
  if ($interval->i > 0) {
    $ago_arr[] = $interval->i . " m";
  }
  if ($interval->s > 0) {
    $ago_arr[] = $interval->s . " s";
  }
  return implode(", ", $ago_arr) . ((count($ago_arr) > 0) ? ' ago' : '');
}

/**
 * This will return the set of files that are found in the $scan_path.
 *
 * This will also check the expected set of filenames to mark which ones are
 * missing -- ignoring the ".xml" files.
 *
 * @param type $scan_path
 */
function islandora_digital_workflow_scan_files($scan_path, $expected_file_records = array()) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/file_utilities');
  $files = islandora_digital_workflow_file_scan_directory($scan_path, '', TRUE);
  $ret_arr = array();
  $count_of_expected_file_records = count($expected_file_records);
  foreach ($files as $folder => $folder_files) {
    foreach ($folder_files as $file) {
      $relative_file_path = $folder . '/' . $file;
      $full_file_path = $scan_path . '/' . $relative_file_path;
      if (file_exists($full_file_path)) {
        $size = filesize($full_file_path);
        $found_file_pathinfo = pathinfo($full_file_path);
        $ext = $found_file_pathinfo['extension'];
        if ($ext <> 'xml') {
          $expected_file_records_index = 0;
          $found_matching_file = FALSE;
          while (!$found_matching_file && $expected_file_records_index < $count_of_expected_file_records) {
            $compare_file_record = $expected_file_records[$expected_file_records_index];
            if ($compare_file_record->filename_basename == $file) {
              $found_matching_file = TRUE;
            }
            $expected_file_records_index++;
          }
          $class = ($found_matching_file) ? '' : 'bad';
        }
        else {
          $class = '';
        }
        $ret_arr[$relative_file_path] = array(
            'filesize' => $size,
            'class' => $class,
        );
      }
    }
  }
  ksort($ret_arr);
  return $ret_arr;
}
