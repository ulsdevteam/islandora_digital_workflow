<?php

/**
 * @file
 * Set of utility functions that are widely used throughout the module code.
 */

/**
 * Creates a minimal batch stub record that will be updated in other parts of
 * the code.
 *
 * @todo add a validate call instead of checking the values of $form_state here.
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
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');

  global $user;
  if (is_array($form_state) && isset($form_state['values']['batch_name'])
    && $form_state['values']['batch_name']) {
    $batch_name = $form_state['values']['batch_name'];
    $batch_record = islandora_digital_workflow_get_batch_record_by_name($batch_name);

    $batch_request_date = (($form_state['values']['is_batch_request'] == 1) ?
        strtotime($form_state['values']['batch_request_due_date']['year'] . '/' .
          $form_state['values']['batch_request_due_date']['month'] . '/' .
          $form_state['values']['batch_request_due_date']['day']) :
        NULL);

    $field_values = array(
        'batch_name' => $batch_name,
        'batch_description' => $form_state['values']['batch_description'],
        'default_type_of_resource' => $form_state['values']['default_type_of_resource'],
        'islandora_model' =>  islandora_digital_workflow_get_name_of_tid($form_state['values']['cmodel_select']),
        'is_batch_active' => 0,
        'is_batch_request' => $form_state['values']['is_batch_request'],
        'batch_requestor' => $form_state['values']['batch_requestor'],
        'batch_request_due_date' => $batch_request_date,
        'batch_priority_tid' => $form_state['values']['batch_priority'],
        'user' => $user->name,
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
 * @todo add a validate call instead of checking the values of $form_state here.
 *
 * @global object $user
 *   Drupal user object.
 * @param array $form_state
 *   Drupal representation of form_state as an array.
 * @param object $node
 *   Drupal node object.
 * @param string $csv_filename
 *   The filename that points to the CSV file.
 * @param integer $alternative_uid
 *   when provided, this will override the current-logged-in user's uid value.
 * @return mixed
 *   Either redurns the batch record OR NULL if there is not enough info to
 * create the record
 */
function islandora_digital_workflow_create_batch_record_by_form($form_state, $node, $csv_filename, $alternative_uid = NULL) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');

  global $user;
  if (is_array($form_state) && isset($form_state['values']['batch_name'])
    && $form_state['values']['batch_name']) {
    $batch_name = $form_state['values']['batch_name'];
    $batch_record = islandora_digital_workflow_get_batch_record_by_name($batch_name);

    $uploaded_csv_file = ($csv_filename) ? file_get_contents(drupal_realpath($csv_filename)) : FALSE;
    $batch_request_date = ((array_key_exists('is_batch_request', $form_state['values'])) && ($form_state['values']['is_batch_request'] == 1) ?
        strtotime($form_state['values']['batch_request_due_date']['year'] . '/' .
          $form_state['values']['batch_request_due_date']['month'] . '/' .
          $form_state['values']['batch_request_due_date']['day']) :
        NULL);

    // Since the form can be modified and potentially may not include some
    // values, check whether or not these form_state fields exist before
    // trying to access their values.  This is the reason for some of these
    // fields to have a logic test for setting the record's value.
    $field_values = array();
    if ($batch_name) {
      $field_values['batch_name'] = $batch_name;
    }
    if (array_key_exists('batch_description', $form_state['values'])) {
      $field_values['batch_description'] = $form_state['values']['batch_description'];
    }
    if (array_key_exists('is_batch_active', $form_state['values'])) {
      $field_values['is_batch_active'] = $form_state['values']['is_batch_active'];
    }
    if (array_key_exists('is_batch_request', $form_state['values'])) {
      $field_values['is_batch_request'] = $form_state['values']['is_batch_request'];
    }
    if (array_key_exists('batch_priority', $form_state['values'])) {
      $field_values['batch_priority_tid'] = $form_state['values']['batch_priority'];
    }
    if (array_key_exists('batch_requestor', $form_state['values'])) {
      $field_values['batch_requestor'] = $form_state['values']['batch_requestor'];
    }
    if ($batch_request_date) {
      $field_values['batch_request_due_date'] = $batch_request_date;
    }
    if (array_key_exists('object_count', $form_state['values'])) {
      $field_values['object_count'] = $form_state['values']['object_count'];
    }
    if (array_key_exists('ingest_namespace', $form_state['values'])) {
      $field_values['ingest_namespace'] = $form_state['values']['ingest_namespace'];
    }
    if (array_key_exists('default_ead_id', $form_state['values'])) {
      $field_values['default_ead_id'] = $form_state['values']['default_ead_id'];
    }
    else {
      $field_values['default_ead_id'] = '';
    }
    if (array_key_exists('ocr_language', $form_state['values'])) {
      $field_values['ocr_language'] = $form_state['values']['ocr_language'];
    }
    else {
      $field_values['ocr_language'] = '';
    }
    if (array_key_exists('default_depositor', $form_state['values'])) {
      $field_values['default_depositor'] = $form_state['values']['default_depositor'];
    }
    if (array_key_exists('default_genre', $form_state['values'])) {
      $field_values['default_genre'] = $form_state['values']['default_genre'];
    }
    if (array_key_exists('batch_default_pub_status', $form_state['values'])) {
      $field_values['batch_default_pub_status'] = $form_state['values']['batch_default_pub_status'];
    }
    if (array_key_exists('batch_default_CR_status', $form_state['values'])) {
      $field_values['batch_default_CR_status'] = $form_state['values']['batch_default_CR_status'];
    }
    if (array_key_exists('batch_default_CR_holder', $form_state['values'])) {
      $field_values['batch_default_CR_holder'] = $form_state['values']['batch_default_CR_holder'];
    }
    if (array_key_exists('cmodel_select', $form_state['values'])) {
      $field_values['islandora_model'] = islandora_digital_workflow_get_name_of_tid($form_state['values']['cmodel_select']);
    }
    if (array_key_exists('default_type_of_resource', $form_state['values'])) {
      $field_values['default_type_of_resource'] = $form_state['values']['default_type_of_resource'];
    }
    if (array_key_exists('collections_select', $form_state['values'])) {
      $field_values['mapto_collections'] = implode(",", $form_state['values']['collections_select']);
    }
    if (array_key_exists('workflow_sequence_id', $form_state['values'])) {
      $field_values['workflow_sequence_id'] = $form_state['values']['workflow_sequence_id'];
    }
    $field_values['nid'] = is_object($node) ? $node->nid : 0;
    $field_values['user'] = $user->name;
//        'lastmod' => time(),

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

    // if configured, set the site_id field and value
    //   mapto_site_id_values / sites_select
    if (variable_get('islandora_digital_workflow_use_isMemberOfSite', 0)) {
      if (array_key_exists('sites_select', $form_state['values'])) {
        $field_values['mapto_site_id_values'] = implode(",", $form_state['values']['sites_select']);
      }
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
    islandora_digital_workflow_sync_node_to_items($updated_batch_record, $node, $csv_filename, TRUE, $alternative_uid);
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
 * Helper function to sync the islandora_digital_workflow_batch_items records
 * to a given workflow_batch node or batch_record.
 *
 * @param array $batch_record
 *   The batch record as an array.
 * @param object $node
 *   Drupal node object.
 * @param string $csv_filename
 *   The filename that points to the CSV file.
 * @param boolean $empty_items_table
 *   Whether or not to clear the items table when synching the node to items.
 * @param integer $alternative_uid
 *   when provided, this will override the current-logged-in user's uid value.
*/
function islandora_digital_workflow_sync_node_to_items($batch_record, $node, $csv_filename, $empty_items_table = FALSE, $alternative_uid = NULL) {
  if (is_array($batch_record)) {
    // Set a variable for this for readibility on the subsequent function call.
    $keep_created_MODS_files = ($csv_filename <> '');
    $csv_file_sources = islandora_digital_workflow_prepare_csv2mods($csv_filename, $batch_record, $keep_created_MODS_files);
    if ($csv_file_sources && count($csv_file_sources) > 0) {
      $batch_item_identifiers_to_transaction_ids = ($empty_items_table) ?
          islandora_digital_workflow_get_batch_item_ids_to_transaction_ids($batch_record['batch_id']) :
          array();
      if ($empty_items_table) {
        // Delete any pre-existing batch_items records for this batch's batch_id.
        if (db_delete('islandora_digital_workflow_batch_items')
            ->condition('batch_id', $batch_record['batch_id'])
            ->execute()) {
          drupal_set_message(t('Previous batch item records deleted for "@batch_name".',
              array('@batch_name' => $batch_record['batch_name'])), 'status');
        }
      }
      foreach ($csv_file_sources as $csv_file_source) {
        islandora_digital_workflow_sync_batch_item_record($batch_record, $csv_file_source, $batch_item_identifiers_to_transaction_ids, $alternative_uid);
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
 * @param array $batch_item_identifiers_to_transaction_ids
 *   an associative array where key is old batch_item->identifier value and the
 *   value is the old transaction_id value
 * @param integer $alternative_uid
 *   when provided, this will override the current-logged-in user's uid value.
 */
function islandora_digital_workflow_sync_batch_item_record($batch_record, $csv_file_source, $batch_item_identifiers_to_transaction_ids, $alternative_uid = NULL) {
  global $user;
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
  if (file_exists($csv_file_source['mods_filename'])) {
    $mods_content = file_get_contents($csv_file_source['mods_filename']);
    $mods_content = islandora_digital_workflow_convert_smart_quotes($mods_content);
  }
  else {
    $mods_content = '';
  }
  $title = islandora_digital_workflow_convert_smart_quotes($csv_file_source['title']);

  // take each mods_filename and put this into the item record
  $fields = array(
      'batch_id' => $batch_record['batch_id'],
      'identifier' => $csv_file_source['identifier'],
      'title' => $title,
      'filename' => $csv_file_source['filename'],
      'mods' => $mods_content,
      'marc' => '',
  );
  // Get a list of existing batch_item identifiers - only insert the record if
  // one does not exist for this identifier.
  $existing_batch_identifiers = islandora_digital_workflow_get_existing_identifiers($batch_record['batch_id']);
  // Each MODS is inserted into the item record - this will insert the record:
  $batch_item_id = (array_search($csv_file_source['identifier'], $existing_batch_identifiers) === FALSE) ?
    islandora_digital_workflow_insert_batch_item($fields) :
    array_search($csv_file_source['identifier'], $existing_batch_identifiers);

  $item_is_paged = islandora_digital_workflow_item_is_paged_content($batch_item_id, $batch_record);
  if ($item_is_paged) {
    unset($fields['filename']);
  }
  // Only update the transaction records for this identifier.
  if (!is_null($batch_item_id) && array_key_exists($csv_file_source['identifier'], $batch_item_identifiers_to_transaction_ids)) {
    foreach ($batch_item_identifiers_to_transaction_ids[$csv_file_source['identifier']] as $timestamp_transaction_id) {
      db_update('islandora_digital_workflow_transactions')
        ->fields(array(
            'timestamp' => $timestamp_transaction_id['timestamp'],
            'batch_item_id' => $batch_item_id,
            'drupal_uid' => (($alternative_uid) ? $alternative_uid : $user->uid),
        ))
        ->condition('transaction_id', $timestamp_transaction_id['transaction_id'])
        ->execute();
      // check to see if this specific change will trigger any transaction Stage changes
      islandora_digital_workflow_check_trigger_stage_change($timestamp_transaction_id['transaction_id']);
    }
  }
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
 * @param array $batch_record
 *   Array of values from the islandora_digital_workflow_batch table that represents
 * a single batch.
 * @param boolean $is_delivery
 *   Optional, default FALSE.  When set, the path is based on the path configured
 * as "Incoming Delivery Directory" islandora_digital_workflow_ingest_delivery_path
 * else "Ingest Working Directory" islandora_digital_workflow_ingest_prepared_path.
 * @return string
 *   The batch_path for the given batch_values record.
 */
function islandora_digital_workflow_batch_path($batch_record, $is_delivery = FALSE) {
  if (is_object($batch_record)) {
    $batch_record = (array) $batch_record;
  }
  if (!is_array($batch_record) || !array_key_exists('batch_name', $batch_record)) {
    drupal_set_message(t('The batch path could not be determined.  Either the values are missing or ' .
            'there is no value for the batch "Name" (batch_name field) yet.'), 'warning');
    return '';
  }
  $ingest_path = ($is_delivery) ? variable_get('islandora_digital_workflow_ingest_delivery_path', file_directory_temp()) :
    variable_get('islandora_digital_workflow_ingest_prepared_path', file_directory_temp());
  if ($ingest_path == 'ftp' || (ltrim($ingest_path, 'ftp:') <> $ingest_path)) {
    $ingest_path = variable_get('islandora_digital_workflow_ingest_prepared_path', file_directory_temp());
  } elseif (strstr($ingest_path, ":")) {
    @list($ingest_host, $ingest_path) = explode(":", $ingest_path);
    drupal_set_message(t('This batch is configured to use files from a different server.  ' .
        'That server needs to be configured as a mounted folder under the /ingest ' .
        'directory and the ') . l(t('Available Host Alias'), '/admin/islandora/islandora_digital_workflow') .
        t(' configuration would need to be updated to point to the mounted location.'),
        'error');
  }

  $batch_path = rtrim($ingest_path, '/') . '/' . $batch_record['batch_name'];
  $batch_path = islandora_digital_workflow_filename_sanitize($batch_path);

  if (!file_exists($batch_path)) {
    $call = mkdir($batch_path);
    if (!$call) {
      drupal_set_message(t('The batch folder "@batch_path" could not be created.', array(
          '@batch_path' => $batch_path
      )), 'error');
    }
  }

  return (file_exists($batch_path)) ? $batch_path : '';
}

/**
 * Helper function to return the value of the
 * islandora_digital_workflow_ingest_namespace - with the right trailing colon.
 *
 * @param array $batch_record
 *   [optional] The batch record in question.
 * @return string
 *   The sanitized ingest_namespace value.
 */
function islandora_digital_workflow_get_ingest_namespace($batch_record = array()) {
  return rtrim(($batch_record && isset($batch_record['ingest_namespace'])) ?
      $batch_record['ingest_namespace'] :
      variable_get('islandora_digital_workflow_ingest_namespace', 'islandora'
    ), ':') . ':';
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
    'islandora:sp_audioCModel',
    'islandora:sp-audioCModel',
    'islandora:collectionCModel',
    'islandora:CollectionModel',
    'islandora:compoundCModel',
    'islandora:sp_disk_image',
    'islandora:entityCModel',
    'islandora:eventCModel',
    'islandora:mapCModel',
    'islandora:newspaperPageCModel',
    'islandora:oralhistoriesCModel',
    'islandora:organizationCModel',
    'islandora:sp_pdf',
    'islandora:sp_strict_pdf',
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
 * This will get all configured models and filter them to display only the ones
 * that the user has access to use.
 *
 * @return array
 *   An associative array where the key is the taxonomy id for that content type
 * and the value is the name.
 */
function islandora_digital_workflow_get_accessible_cmodels() {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/access_permissions');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/taxonomy_utilities');
  $readable_models = islandora_digital_workflow_get_reduced_readable_cmodels();

  $configured_supported_models = variable_get('islandora_digital_workflow_islandora_models', array());
  $content_types_vocab = taxonomy_vocabulary_machine_name_load(IDW_CONTENT_TYPE_TAXONOMY_MACHINE_NAME);

  $accessible_models = array('' => 'Select Object Model');
  foreach ($readable_models as $cmodel => $model) {
    if (isset($configured_supported_models[$cmodel]) && $configured_supported_models[$cmodel] == 1) {
      // Now, check that the user has access to this model
      $permission = islandora_digital_workflow_permission_of_model($cmodel);
      if (user_access($permission)) {
        $tid = islandora_digital_workflow_get_tid_by_name($cmodel, $content_types_vocab);
        $accessible_models[$tid] = $cmodel;
      }
    }
  }
  return $accessible_models;
}

/**
 * This returns the set of islandora models that are configured to have
 * workflow sequences.
 *
 * @return array
 *   The possible array of islandora models that are currently configured
 * to be handled by an existing workflow sequence.
 */
function islandora_digital_workflow_get_cmodels_having_sequences() {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
  $models = islandora_digital_workflow_get_reduced_readable_cmodels();
  $model_pid_keys = array_keys($models);
  // with the $model_pid_keys, run a mysql query to get the supported models
  // from `islandora_digital_workflow_model_sequence`.
  $sequences = islandora_digital_workflow_get_workflow_model_sequences();
  $return_models = array();
  // Only keep the values if they are also found within a workflow sequence.
  foreach ($model_pid_keys as $islandora_model_pid) {
    $found = FALSE;
    foreach ($sequences as $sequence_id => $model_sequence) {
      $found |= array_key_exists($islandora_model_pid, $model_sequence);
    }
    if ($found) {
      $return_models[$islandora_model_pid] = array(
          'pid' => $islandora_model_pid,
          'label' => $models[$islandora_model_pid],
      );
    }
  }
  return $return_models;
}

/**
 * Returns all content types for the current Islandora deployment.
 *
 * This will also strip off the " Content Model" suffix.
 *
 * @todo check user permissions here for this $model_name before adding.
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
      $return_models[$model_name] = str_replace($suffix, "", $model_name_readable);
    }
  }
  return $return_models;
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
        drupal_set_message(t('There is a bad date value for "normalized_date" = "@norm" ' .
          ' on row #@row.  Please upload the CSV after fixing this value.', array(
              '@row' => $k,
              '@norm' => $row[$date_index]
          )), 'error');
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
function islandora_digital_workflow_csv_file_as_array($csv_file, $key_is_fielnames = FALSE) {
  $file_rows = $file_header = array();
  if (file_exists($csv_file)) {
    $row = 0;
    if (($handle = fopen(drupal_realpath($csv_file), "r")) !== FALSE) {
      try {
        while (($data = fgetcsv($handle)) !== FALSE) {
          $row++;
          if ($row > 1) {
            if ($key_is_fielnames) {
              $tmp_data = array();
              foreach ($file_header as $index => $fieldname) {
                $tmp_data[$fieldname] = $data[$index];
              }
              $file_rows[] = $tmp_data;
            }
            else{
              $file_rows[] = $data;
            }
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
          drupal_set_message(t('Bad date format for date value of "@short_date".', array(
              '@short_date' => $short_date
          )), 'error');
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
          drupal_set_message(t('Bad date format for date value of "@short_date".', array(
              '@short_date' => $short_date
          )), 'error');
      }
      return '';
    }
  }
  $timestamp = strtotime($short_date);
  return array(date('c', $timestamp), date($h_format, $timestamp));
}

/**
 * This will return the values for a given field from a CSV array.
 *
 * @param string $fieldname
 *   The name of the field to search for
 * @param array $csv_array
 *   An array of arrays - where each inner array is a row of CSV data.
 * @return array
 *   The array of values for the given fieldname.
 */
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
    $model_requirements = array();
    $model_requirements = islandora_digital_workflow_object_model_datastream_requirements($cmodel);

    $dsid_requirements_table_arr = array();
    foreach ($model_requirements['required'] as $dsid => $ds_mime_and_id) {
      $dsid_requirements_table_arr[] = '<span class="required_dsid" title="For &quot;' . $model . "&quot;, the " . $dsid . ' datastream derivative is required">' . $dsid . '</span>';
    }
    foreach ($model_requirements['optional'] as $dsid => $ds_mime_and_id) {
      $dsid_requirements_table_arr[] = '<span class="optional_dsid" title="For &quot;' . $model . "&quot;, the " . $dsid . ' datastream derivative is optional">' . $dsid . '</span>';
    }
    $dsid_requirements_table = '<p><b>Datastream requirements:</b> ' .
            implode(" | ", $dsid_requirements_table_arr) . '</p></div>';

    $this_model_module_info = islandora_digital_workflow_model_ingest_module_info($cmodel);

    $title = ($this_model_module_info['exists']) ? $model : $model . ' (not installed)';
    $configured_models[$cmodel] = array(
      '#type' => 'checkbox',
      '#title' => $title,
      // Mark the checkboxes as checked based on islandora_digital_workflow_islandora_models
      '#default_value' => isset($configured_supported_models[$cmodel]) ? (($configured_supported_models[$cmodel] <> 0) ? 1 : 0) : 0,
      '#prefix' => '<div class="major-item">',
      '#suffix' => $dsid_requirements_table . '</div>',
    );
    if ($this_model_module_info['git_branch'] <> '') {
       $configured_models[$cmodel]['#suffix']  = '<div class="indent_small_text">' .
           '<b>Module: </b>' . 
           $this_model_module_info['uls_git_link'] .
           ' <i>' . $this_model_module_info['git_branch'] . '</i>' .
           $configured_models[$cmodel]['#suffix'];
    }
    if (!$this_model_module_info['exists']) {
      $configured_models[$cmodel]['#default_value'] = 0;
      drupal_set_message(t('The module is not installed that can ingest @model objects, ' .
          ' so they can not be managed using the workflow until that module is installed (', array(
              '@model' => $model
          )) .
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
    case "islandora:sp_basic_image": case "islandora:sp_large_image_cmodel":
      $retval['module_name'] = 'islandora_batch';
      $git_link = 'https://github.com/Islandora/islandora_batch.git';
      $uls_git_link = 'https://github.com/ulsdevteam/islandora_batch.git';
      break;
    case "islandora:findingAidCModel":
      $retval['module_name'] = 'islandora_batch';
      $git_link = 'https://github.com/Islandora/islandora_batch.git';
      $uls_git_link = 'https://github.com/ulsdevteam/islandora_batch.git';
      break;
    case "islandora:bookCModel":
      $retval['module_name'] = 'islandora_book_batch';
      $git_link = 'https://github.com/Islandora/islandora_book_batch.git';
      $uls_git_link = 'https://github.com/ulsdevteam/islandora_book_batch.git';
      break;
    case "islandora:manuscriptCModel":
      $retval['module_name'] = 'islandora_manuscript_batch';
      $git_link = '//';
      $uls_git_link = 'https://github.com/ulsdevteam/islandora_manuscript_batch.git';
      break;
    case "islandora:newspaperCModel": case "islandora:newspaperIssueCModel":
      $retval['module_name'] = 'islandora_newspaper_batch';
      $git_link = 'https://github.com/Islandora/islandora_newspaper_batch.git';
      $uls_git_link = 'https://github.com/ulsdevteam/islandora_newspaper_batch.git';
      break;
    case "islandora:sp_web_archive":
      $retval['module_name'] = 'islandora_batch';
      $git_link = 'https://github.com/Islandora/islandora_book_batch.git';
      $uls_git_link = 'https://github.com/ulsdevteam/islandora_book_batch.git';
      break;
  }
  $link_options = array('attributes'=>array(
      'title' => 'Drupal module source code',
      'target' => '_blank'));
  $retval['git_link'] = l($retval['module_name'], $git_link, $link_options);
  $retval['uls_git_link'] = l($retval['module_name'], $uls_git_link, $link_options);

  $retval['exists'] = module_exists($retval['module_name']);
  $prev_dir = getcwd();
  if ($retval['exists']) {
    $path = drupal_realpath(drupal_get_path('module', $retval['module_name']));
    chdir($path);
    // Execute a command in git to find the branch of the underlying model
    $output = $return_var = array();
    exec('/usr/bin/git rev-parse --abbrev-ref HEAD', $output, $return_var);
    $retval['git_branch'] = ((($return_var == -1 || $return_var == 255)) ? '' : implode('', $output));
    chdir($prev_dir);
  }
  else {
    $retval['git_branch'] = '';
  }

  return $retval;
}

/**
 * This will parse a row from a CSV file and return an array with two parts to the info:
 *
 * $prepared_filepath = The path to the folder that contains the MODS and image TIF.
 * $identifier - the identifier value (if provided in the spreadsheet).
 *
 * @todo capture batch-level values for default_genre, batch_default_CR_status,
 *       batch_default_pub_status, and batch_default_CR_holder -- to potentially set
 *       when creating MODS below.
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
    // Look to see if the batch_record has a uploaded_csv_file
    if ($batch_record['uploaded_csv_file']) {
      $temp_csv_filename = drupal_realpath(drupal_tempnam('temporary://', 'batch_objects.csv'));
      if ($bytes_written = file_put_contents($temp_csv_filename, $batch_record['uploaded_csv_file'])) {
        $input_filename = $temp_csv_filename;
        $keep_created_MODS_files = TRUE;
      }
    }
  }
  else {
    $temp_csv_filename = '';
  }
  if (!$input_filename) {
    return array();
  }

  $batch_path = islandora_digital_workflow_batch_path($batch_record);

  ini_set('auto_detect_line_endings', true);
  $outputFilename   = 'MODS.xml';
  // Open csv to read
  $inputFile  = fopen(drupal_realpath($input_filename), 'rt');

  // Get the headers of the file
  $headers = fgetcsv($inputFile);
  if (!is_array($headers)) { return FALSE; }
  if ((array_search('identifier', $headers) === FALSE) and (array_search('Identifier', $headers) === FALSE)) {
    return FALSE;
  }

  //  echo "<h3>Working directory " . $batch_path . "</h3>";

  $st = " style='background-color:#F2F2F2;border:1px solid #222'";  $toggle = false;  $row_counter = 1;

  $csv_header_xpath_mappings = islandora_digital_workflow_get_csv_header_xpath_mappings();
  $mods_created = $auto_number = 0;

  $ingest_namespace = islandora_digital_workflow_get_ingest_namespace($batch_record);
  $identifiers_and_filepaths = array();
  // Loop through each row creating a <row> node with the correct data
  while (($row = fgetcsv($inputFile)) !== FALSE) {
    // $row = array_map("utf8_encode", $row);
    // First, scan the entire row to get the identifier and the file_name value
    $toggle = !$toggle;
    // echo "<h3>row " . $row_counter . "</h3>";
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
            // For ISLANDORA-214, this will need to be different for some paged-content.
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

    // Create a new dom document with pretty formatting
    $doc  = new DOMDocument('1.0', 'utf-8'); // DomDocument();
    $doc->formatOutput   = true;

    // Start creating the MODS XML by adding the root node to the document.
    $root = $doc->createElement('sheet'); // mods:mods'); // ,'test value');
    $root->appendChild($doc->createAttribute('xmlns:xsi'))->appendChild($doc->createTextNode("http://www.w3.org/2001/XMLSchema-instance"));

    $root = $doc->appendChild($root);
    $child = NULL;
    // Set all of the "default_FIELD_in_CSV" values to false --
    $depositor_in_CSV = $default_genre_in_CSV = $batch_default_CR_status_in_CSV = $batch_default_CR_holder_in_CSV = $batch_default_pub_status_in_CSV = FALSE;
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
          if (!$default_genre_in_CSV && $header == 'genre') {
            $default_genre_in_CSV = TRUE;
          }
          if (!$batch_default_CR_status_in_CSV && $header == 'copyright_status') {
            $batch_default_CR_status_in_CSV = TRUE;
          }
          if (!$batch_default_pub_status_in_CSV && $header == 'publication_status') {
            $batch_default_pub_status_in_CSV = TRUE;
          }
          if (!$batch_default_CR_holder_in_CSV && $header == 'rights_holder') {
            $batch_default_CR_holder_in_CSV = TRUE;
          }
        }
      }
    }
    if ($child) {
      $root->appendChild($child);
    }

    // After the CSV row has been processed, as long as the depositor value was not provided in that row,
    // and the batch provided a "default_depositor" value, then create this node now.
    if (!$depositor_in_CSV && $batch_record['default_depositor'] && !array_key_exists('depositor', $headers)) {
      $child = $doc->createElement('depositor');
      $child = $root->appendChild($child);
      $value = $doc->createTextNode($batch_record['default_depositor']);
      $value = $child->appendChild($value);
    }
    if (!$default_genre_in_CSV && $batch_record['default_genre'] && !array_key_exists('genre', $headers)) {
      $child = $doc->createElement('genre');
      $child = $root->appendChild($child);
      $value = $doc->createTextNode($batch_record['default_genre']);
      $value = $child->appendChild($value);
    }
    if (!$batch_default_CR_status_in_CSV && $batch_record['batch_default_CR_status'] && !array_key_exists('copyright_status', $headers)) {
      $child = $doc->createElement('copyright_status');
      $child = $root->appendChild($child);
      $value = $doc->createTextNode($batch_record['batch_default_CR_status']);
      $value = $child->appendChild($value);
    }
    if (!$batch_default_pub_status_in_CSV && $batch_record['batch_default_pub_status'] && !array_key_exists('publication_status', $headers)) {
      $child = $doc->createElement('publication_status');
      $child = $root->appendChild($child);
      $value = $doc->createTextNode($batch_record['batch_default_pub_status']);
      $value = $child->appendChild($value);
    }
    if (!$batch_default_CR_holder_in_CSV && $batch_record['batch_default_CR_holder'] && !array_key_exists('rights_holder', $headers)) {
      $child = $doc->createElement('rights_holder');
      $child = $root->appendChild($child);
      $value = $doc->createTextNode($batch_record['batch_default_CR_holder']);
      $value = $child->appendChild($value);
    }
    $strxml = $doc->saveXML();

    if ($keep_created_MODS_files) {
      $handle = fopen($batch_path . '/' . $row_mods_id . '/' . $outputFilename, "w");
      $bytes_written = fwrite($handle, $strxml);
      fclose($handle);
    }
    $filename = ($keep_created_MODS_files) ? islandora_digital_workflow_XMLtoMODS($batch_path . '/' . $row_mods_id . '/' . $outputFilename, $batch_record['batch_id'], $ingest_namespace . $row_mods_id) : "";
    $mods_created += ($keep_created_MODS_files) ? ($filename <> '') : 1;
    // echo "<p>created MODS from spreadsheet for <b>" . $row_mods_id . "</b></p>";

    $identifiers_and_filepaths[] = array(
        'title' => $title,
        'identifier' => $row_mods_id,
        'filename' => $row_file_name,
        'mods_filename' => $filename);
    $row_counter++;
  }
  // echo "<p>" . number_format($mods_created) . " MODS files created.</p>";

  if ($temp_csv_filename <> '' && file_exists($temp_csv_filename)) {
    unlink($temp_csv_filename);
  }
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
function islandora_digital_workflow_XMLtoMODS($xml_filename, $batch_id, $identifier) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
  
  $batch_item_id = islandora_digital_workflow_get_batch_item_id_by_identifier($identifier, $batch_id);

  $xsl = str_replace("/includes", "", dirname(__FILE__).'/transforms/sheet2mods.xsl');

  $xml_file = islandora_digital_workflow_convert_smart_quotes(file_get_contents($xml_filename));
  $MODS = ($xml_file) ? islandora_digital_workflow_runXslTransform(
            array(
              'xsl' => $xsl,
              'input' => $xml_file,
            )
          ) : '';

  // This file must be deleted in the process function that called this.
  $xml_filename = islandora_digital_workflow_filename_sanitize($xml_filename);
  $bytes_written = file_put_contents($xml_filename, $MODS);
  if ($batch_item_id && $bytes_written) {
    islandora_digital_workflow_insert_transactions_record(IDW_ACTION_ADD_MODS_RECORD, NULL, $batch_item_id);
  }
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
  // This has to use a "higlighting-as-we-go" approach due to the possible
  // highlight replacement to tag attributes, etc.  This parses the entire content
  // and only highlights what is between tags.
  $lines = explode("\r\n", $text);
  $opened_tag = FALSE;
  $render_output = '';
  $tag_queue = '';
  $current_queue = '';
  foreach ($lines as $line) {
    for ($line_index = 0; $line_index  < strlen($line); $line_index ++) {
      if ($line[$line_index] == '<') {
        $render_output .= $tag_queue . ($tag_queue ? '>' : '') .
            islandora_digital_workflow_highlight_queue_replacement($current_queue, $word, $case_sensitive);
        $tag_queue = '';
        $current_queue = '';
      }
      $opened_tag |= ($line[$line_index] == '<');
      if ($opened_tag && ($line[$line_index] == '>')) {
        $opened_tag = FALSE;
      }
      elseif (!$opened_tag) {
        $current_queue .= $line[$line_index];
      }
      if ($opened_tag) {
        $tag_queue .= $line[$line_index];
      }
    }
    $render_output .= "\r\n";
  }
  $render_output .= islandora_digital_workflow_highlight_queue_replacement($current_queue, $word, $case_sensitive);
  return $render_output;
}

/**
 * Little helper function for "higlighting-as-we-go" approach.
 *
 * @param string $text
 *   Content "queued" string to highlight.
 * @param string $word
 *   The word to highlight.
 * @param type $case_sensitive
 *   Whether or not the highlighting is supposed to be case-sensitive.
 * @return string
 *   The section of content with any highlighting applied.
 */
function islandora_digital_workflow_highlight_queue_replacement($text, $word, $case_sensitive = FALSE) {
  $highlighted_text = "<span class='highlighted'>$word</span>";
  $text = ($case_sensitive) ?
      str_replace($word, $highlighted_text, $text) :
      str_ireplace($word, $highlighted_text, $text);
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
  $_compared = ($dbDate > $currDate) ? ' from now' : ' ago';
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
  return implode(", ", $ago_arr) . ((count($ago_arr) > 0) ? $_compared : '');
}

/**
 * This will return the set of files that are found in the $scan_path.
 *
 * This will also check the expected set of filenames to mark which ones are
 * missing -- ignoring the ".xml" files.
 *
 * @param string $scan_path
 *   Folder to scan.
 * @param array $expected_file_records
 *   Array of stdObjects that have properties for each batch item that needs
 * to be scanned.
 */
function islandora_digital_workflow_scan_files($scan_path, $expected_file_records = array()) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/file_utilities');
  $files = islandora_digital_workflow_file_scandir($scan_path, '', TRUE);
  $ret_arr = array();
  foreach ($files as $folder => $folder_files) {
    if (is_array($folder_files)) {
      foreach ($folder_files as $file) {
        $ret_arr[$folder . '/' . $file] = islandora_digital_workflow_get_filesize_class($scan_path, $folder, $file, FALSE);
      }
    }
    else {
      $ret_arr[$folder_files] = islandora_digital_workflow_get_filesize_class($scan_path, '', $folder_files, FALSE);
    }
  }
  ksort($ret_arr);
  return $ret_arr;
}

/**
 * This will return all of the folder names that exist in and under the $root
 * path value.
 *
 * @return array
 *   array of folder paths with each element containing an array of files in that
 * folder.
 */
function islandora_digital_workflow_find_folders_files($root) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/file_utilities');
  
  $return_folders = array();
  if ($root) {
    $folders = islandora_digital_workflow_file_scandir($root, '', TRUE);
    foreach ($folders as $key => $files) {
      $return_folders[$key]['item_name'] = $key;
      $onelevel_filepaths = array();
      foreach ($files as $filekey => $file) {
        if (is_array($file)) {
          foreach ($file as $cfile) {
            $onelevel_filepaths[] = $filekey . '/' . $cfile;
          }
        }
        else {
          $onelevel_filepaths[] = $file;
        }
      }
      $files = $folders[$key] = $onelevel_filepaths;
      $return_folders[$key]['files'] = $files;
      $return_folders[$key]['files_count'] = count($files);
      $filesize_date_files = islandora_digital_workflow_add_filesize_date_to_files($root . '/' . $key, $files);
      $return_folders[$key]['filesize_date_files_raw'] = $filesize_date_files;
      $return_folders[$key]['files_table_rows'] = islandora_digital_workflow_convert_tab_to_table_cells($root . '/' . $key, $filesize_date_files);
    }
  }
  return $return_folders;
}

/**
 * This will inspect all of the $values and will remove any $values that does not
 * appear in the $item_names list of possible batch folders.  Also, this will
 * return the set of names that were removed for display to the user.
 *
 * @param array $values
 *   An array of folder paths to check.
 * @param array $item_names
 *   An array of possible batch folder names that is used for checking.
 * @return array
 *   The set of folder names that were removed and will not be inspected as
 * other delivery folders.
 */
function islandora_digital_workflow_filter_out_non_batch_folders(&$values, $item_names) {
  $bad_folder_names = array();
  foreach ($values as $item_name => $info) {
    if (array_search($info['item_name'], $item_names) === FALSE) {
      unset($values[$item_name]);
      $bad_folder_names[] = $item_name;
    }
  }
  return $bad_folder_names;
}

/**
 * This will convert the tab strings into html markup for a table row.
 *
 * @param string $files_root
 *   the file path where these batch files are located.
 * @param array $rows
 *   An array of strings that are TAB "\t" delimited.
 * @return string
 *   The html markup for a table row for each row of file info.
 */
function islandora_digital_workflow_convert_tab_to_table_cells($files_root, $rows) {
  $return_string = '';
  $toggle = FALSE;
  $last_file_folder = '';
  foreach ($rows as $row) {
    $file_name = $row['file'];
    $file_size = $row['size_as_string'];
    $file_date = $row['date_as_string'];
    @list($this_file_folder, $junk) = explode("/", $file_name);
    if ($last_file_folder <> $this_file_folder) {
      $toggle = !$toggle;
    }
    // @list($file_name, $file_size, $file_date) = explode("\t", $row_str);
    $problem_notes_content = (strstr($file_name, 'problem_notes.txt')) ? file_get_contents($files_root . '/' . $file_name)
            : '';
    $return_string .= '<tr class="lookup_result ' . (($toggle) ? 'evenrow' : 'oddrow'). 
            (($last_file_folder == $this_file_folder) ? '' : ' spaceUnder') . '"' .
            ($problem_notes_content ? ' title="' . htmlspecialchars($problem_notes_content) . '"' : '') .
            '><td>' . $file_name . '</td><td>' . $file_size . '</td><td>' . $file_date . '</td></tr>';
    $last_file_folder = $this_file_folder;
  }
  return $return_string;
}

/**
 * This will return the set of files that are found in the $scan_path that 
 * likely relate to any paged content files.
 *
 * This will also check the expected set of filenames to mark which ones are
 * missing -- ignoring the ".xml" files.
 *
 * @param type $scan_path
 */
function islandora_digital_workflow_scandir_paged_content_files($scan_path) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/file_utilities');
  $files = islandora_digital_workflow_file_scandir($scan_path, '', TRUE);
  $ret_arr = array();
  foreach ($files as $folder => $folder_files) {
    if (is_array($folder_files)) {
      foreach ($folder_files as $file) {
        $pop_file = (is_array($file)) ? array_pop($file): $file;
        $ret_arr[$folder . '/' . $pop_file] = islandora_digital_workflow_get_filesize_class($scan_path, $folder, $pop_file, TRUE);
      }
    }
    else {
      $ret_arr[$folder_files] = islandora_digital_workflow_get_filesize_class($scan_path, '', $folder_files, TRUE);
    }
  }
  ksort($ret_arr);
  return $ret_arr;
}

/**
 * This will inspect the scan_path, folder, and file to see if the file exists,
 * whether or not the extension is acceptable for ingest, and returns the
 * filesize AND class for this element.
 *
 * @param string $scan_path
 *   The scan_path where the folders of batch files are stored.
 * @param string $folder
 *   The folder under the scan_path where the file is stored.
 * @param string $file
 *   The file from the batch that needs filesize and CSS class.
 * @param boolean $tif_must_be_numeric
 * @return array
 *   The returned array has a 'filesize' (string) and a 'class' (string) value.
 */
function islandora_digital_workflow_get_filesize_class($scan_path, $folder, $file, $tif_must_be_numeric = FALSE) {
  $pop_file = (is_array($file)) ? array_pop($file): $file;
  $relative_file_path = (($folder) ? $folder . '/' : '') . $pop_file;
  $full_file_path = $scan_path . '/' . $relative_file_path;
  if (file_exists($full_file_path)) {
    $size = filesize($full_file_path);
    $found_file_pathinfo = pathinfo($full_file_path);
    $ext = (array_key_exists('extension', $found_file_pathinfo) ? $found_file_pathinfo['extension'] : '');
    $filename = (array_key_exists('filename', $found_file_pathinfo) ? $found_file_pathinfo['filename'] : '');
    if ($ext <> 'xml') {
      $class = '';
      if ($tif_must_be_numeric) {
        if (!($ext == 'tif' && is_numeric($filename))) {
          $class = 'bad';
        }
      }
    }
    else {
      $class = '';
    }
    return array(
      'filesize' => $size,
      'class' => $class,
    );
  }
}

/**
 * Returns all applicable content models that respond to the datasream ingested
 * hook -- for the purpose of saving the derivative back to the parent object.
 *
 * @return array
 */
function islandora_digital_workflow_retrieve_applicable_paged_content_cmodels() {
  return array(
      'islandora:manuscriptCModel',
      'islandora:bookCModel',
      'islandora:newspaperIssueCModel');
}

/**
 * Helper function to validate the uploaded_csv_file.
 *
 * This operates on the current $_FILES -- so it can only be called on form post
 * and potentially sets a $_SESSION variable for use in the submit function,
 * see: islandora_digital_workflow_create_batch_form_submit() above.
 *
 * @return boolean
 */
function islandora_digital_workflow_validate_csv_file_upload() {
  $valid_upload = FALSE;

  if (isset($_FILES['files']) && isset($_FILES["files"]["tmp_name"]['file'])) {
    $dest = drupal_tempnam('temporary://', 'batch_objects.csv');
    if ($_FILES['files']['error']['file'] == UPLOAD_ERR_OK) {
      $tmp_name = (file_exists($_FILES["files"]["tmp_name"]['file'])) ? $_FILES["files"]["tmp_name"]['file'] : $_SESSION['uploaded_csv_filename'];
      if (copy($tmp_name, $dest)) {
        $valid_upload = islandora_digital_workflow_validate_csv($dest);
        if (file_exists($_FILES["files"]["tmp_name"]['file'])) {
          $_SESSION['uploaded_csv_filename'] = $dest;
        }
      }
      else {
        drupal_set_message(t('File move operation from "@tmp_name" to "@dest" failed', array(
            '@tmp_name' => $tmp_name,
            '@dest' => $dest,
        )), 'error');
      }
    }
  }
  // Do not delete the temp file - it is used later via the
  // $_SESSION['upload_csv_filename'] variable.
  return $valid_upload;
}

/**
 * To merge a new CSV upload with that of an existing batch record.
 *
 * @todo create $merged_csv_file by using fputcsv calls like this:
 *         fputcsv($output, array('id','name','description'));
 *         foreach($prod as $product) {
 *           fputcsv($output, $product);
 *         }
 *
 * @param array $batch_record
 *   The batch record as an array.
 * @param object $node
 *   Drupal node object.
 * @param string $new_csv_filename
 *   The filename that points to the CSV file.
 */
function islandora_digital_workflow_merge_uploaded_csv_files(&$batch_record, $node, $new_csv_filename) {
  $csv_array = islandora_digital_workflow_csv_file_as_array($new_csv_filename, TRUE);
  $merged_csv_file = '';
  $merged_rows = array();
  if (is_array($csv_array)) {
    // Use the islandora_digital_workflow_csv_file_as_array to parse the rows
    // correctly according to CSV standards.
    $new_header = (isset($csv_array['header']) ? $csv_array['header'] : array());
    $new_rows = (isset($csv_array['rows']) ? $csv_array['rows'] : array());

    $old_header = $old_rows = array();
    if ($batch_record['uploaded_csv_file']) {
      $temp_csv_filename = drupal_realpath(drupal_tempnam('temporary://', 'batch_objects.csv'));
      if ($bytes_written = file_put_contents($temp_csv_filename, $batch_record['uploaded_csv_file'])) {
        $arr = islandora_digital_workflow_csv_file_as_array($temp_csv_filename, TRUE);
        $old_header = (isset($arr['header']) ? $arr['header'] : array());
        $old_rows = (isset($arr['rows']) ? $arr['rows'] : array());
        unlink($temp_csv_filename);
      }
    }

    // Check the rows from this CSV are in the same order as the rows defined
    // from first row of the batch record's uploaded_csv_file.
    if ((count($new_rows) > 1)) { //  && (count($old_rows) > 1)) {
      $old_header_str = implode(",", $old_header);
      $new_header_str = implode(",", $new_header);
      if ($old_header_str == $new_header_str) {
        $merged_csv_file = $old_header_str . "\n";
        foreach ($old_rows as $old_row) {
          $merged_csv_file .= islandora_digital_workflow_array_to_csv($old_row);
          // $merged_csv_file .= implode(",", $old_row) . "\n";
        }
        foreach ($new_rows as $new_row) {
          $merged_csv_file .= islandora_digital_workflow_array_to_csv($new_row);
          // $merged_csv_file .= implode(",", $new_row) . "\n";
        }
      }
      else {
        // Make a new merged CSV file by looping through both sets of headers
        // to compose an array that represents both sheets' data.
        $work_new_rows = $work_old_rows = $merged_header = $rows = array();
        $first_fieldname = '';
        foreach ($old_header as $header_index => $fieldname) {
          $merged_header[$fieldname] = $fieldname;
          if (!$first_fieldname) { $first_fieldname = $fieldname; }
        }
        foreach ($new_header as $header_index => $fieldname) {
          $merged_header[$fieldname] = $fieldname;
          if (!$first_fieldname) { $first_fieldname = $fieldname; }
        }
        $work_old_rows = $old_rows;
        $work_new_rows = $new_rows;
        foreach ($work_old_rows as $row_index => $row_array) {
          foreach ($old_header as $header_index => $fieldname) {
            $merged_rows[$fieldname][] = array_key_exists($fieldname, $row_array) ? $row_array[$fieldname] : '';
          }
        }
        foreach ($work_new_rows as $row_index => $row_array) {
          foreach ($new_header as $header_index => $fieldname) {
            $merged_rows[$fieldname][] = array_key_exists($fieldname, $row_array) ? $row_array[$fieldname] : '';
          }
        }

        // Now, make the merged uploaded_csv_file from the merged_rows array
        $merged_csv_file = implode(',', $merged_header) . "\n";
        // This nested loop will make an array that contains the field values
        // from both sources - in the order of fields specified by the merge
        // of both sources' first row of fieldnames.
        foreach ($merged_rows[$first_fieldname] as $row_index => $row_array) {
          $tmp_array = array();
          foreach ($merged_header as $fieldname) {
            if (strstr($merged_rows[$fieldname][$row_index], ",")) {
              $val = (strstr($merged_rows[$fieldname][$row_index], '"') <> '') ?
                  str_replace('"', '""', $merged_rows[$fieldname][$row_index]) :
                  $merged_rows[$fieldname][$row_index];
              $tmp_array[] = '"' . $val . '"';
            }
            else {
              $tmp_array[] = $merged_rows[$fieldname][$row_index];
            }
          }
          if (count($tmp_array) > 0) {
            $merged_csv_file .= implode(",", $tmp_array) . "\n";
          }
        }
      }
    }
  }
  if ($merged_csv_file) {
    $temp_csv_filename = drupal_realpath(drupal_tempnam('temporary://', 'merged_csv.csv'));
    if ($bytes_written = file_put_contents($temp_csv_filename, $merged_csv_file)) {
      $csv_array = islandora_digital_workflow_csv_file_as_array($temp_csv_filename, TRUE);
      if (array_key_exists('uploaded_csv_filename', $_SESSION) && file_exists($_SESSION['uploaded_csv_filename'])) {
        // This usage of _SESSION['uploaded_csv_filename can get tricky, but
        // must set this value here because the referencing code calls another
        // function that uses this value to work with the batch's csv file.
        file_put_contents($_SESSION['uploaded_csv_filename'], $merged_csv_file);
      }
      unlink($temp_csv_filename);
    }

    $batch_record['uploaded_csv_file'] = $merged_csv_file;
    $new_value = theme('islandora_digital_workflow_batch_defaults', array('islandora_digital_workflow_batch' => $batch_record));
    $field_values = array('uploaded_csv_file' => $merged_csv_file, 'object_count' => count($merged_rows));
    islandora_digital_workflow_update_batch($batch_record['batch_id'], $field_values);

    $node->field_batch_defaults[LANGUAGE_NONE][0]['value'] = $new_value;
    $csv_values_arr = islandora_digital_workflow_get_csv_values_for_fieldname('identifier', $csv_array);
    $node->field_identifiers[$node->language][0]['value'] = implode("\n", $csv_values_arr['identifier']);
    node_save($node);
  }
}

/**
 * This will take an array and return it as a legal CSV string -- used for each
 * row to make a while file CSV representation for the data.
 *
 * @param type $array
 * @return type
 */
function islandora_digital_workflow_array_to_csv($array) {
  $cleansed_arr = array();
  foreach ($array as $value) {
    $wrap_char = (strstr($value, ",")) ? '"' : '';
    $cleansed_arr[] = $wrap_char . str_replace('"', '""', $value) . $wrap_char;
  }
  return implode(",", $cleansed_arr) . "\n";
}

/**
 * Inspect the datastream requirements for a given model PID.
 *
 * This will return an array of optional and required datastreams.
 *
 * @param string $model_pid
 *   Fedora content model PID
 * @return array
 *   Array of required / optional dasastreams for model_requirements
 */
function islandora_digital_workflow_object_model_datastream_requirements($model_pid) {
  $model = @islandora_object_load($model_pid);
  if (isset($model) && $model) {
    $ds_reqs = islandora_get_datastreams_requirements_from_content_model($model);
    if (array_key_exists('RELS-EXT', $ds_reqs)) {
      if (array_key_exists('RELS-EXT', $ds_reqs) && $ds_reqs['RELS-EXT']['optional']) {
        $ds_reqs['RELS-EXT']['optional'] = FALSE;
      }
      if (array_key_exists('TN', $ds_reqs) && $ds_reqs['TN']['optional']) {
        $ds_reqs['TN']['optional'] = FALSE;
      }
    }
    // We want to override the value for RELS-INT here - it's required.
    if (is_array($ds_reqs)) {
      $model_requirements = $ds_reqs;
    }
  }
  // RELS-EXT
  // Structure the return array to have two subarrays -- one with "required"
  // datastreams and another with "optional" datastreams.
  $return_array = array();
  foreach ($model_requirements as $dsid => $model_requirement) {
    if ($model_requirement['optional'] == FALSE) {
      $return_array['required'][$dsid] = array(
          'id' => $model_requirement['id'],
          'mime' => $model_requirement['mime'],
      );
    }
    else {
      $return_array['optional'][$dsid] = array(
          'id' => $model_requirement['id'],
          'mime' => $model_requirement['mime'],
      );
    }
  }
  return $return_array;
}

/**
 * Will return the CSS class name for the action (which is dynamically created
 * from the actions configuration to the correct glyph graphic for each action).
 *
 * @param string $transaction_name
 *   The name of the action -- straight from the islandora_digital_workflow_actions.name field.
 * @return string
 *   The class name for the action that should display the correct glyph.
 */
function islandora_digital_workflow_glyph_class($transaction_name) {
  return 'transaction_action_' . (($transaction_name) ? strtolower(str_replace(array("-", " "), "_", $transaction_name)) : 'spacer');
}

/**
 * Will determine whether or not all of the actions are completed.
 *
 * Since the variable $item_record_transactions is a merged set of item actions
 * with the required actions for the objects' workflow sequence, any action that
 * still needs to be completed will have a -1 transaction_id value.
 *
 * @param array $item_record_transactions
 *   Array of objects that detail each transaction for the item.
 * @param array $required_batch_actions
 *   Array of objects that relate to the workflow sequence of the item's
 * related batch.
 * @return boolean
 *   TRUE if all required actions for the objects' workflow sequence are completed.
 */
function islandora_digital_workflow_item_ingest_prerequisites_all_done($item_record_transactions, $required_batch_actions) {
  // To see if all actions are done, loop through all of the item actions to see
  // if any of these are "stub transaction" objects that are needed to indicate
  // any required actions on the item that have not been completed.
  $retval = TRUE;
  // Make an array of the required_batch_actions that are set to is_ingest_prerequisite.
  $ingest_prerequisite_ids = array();
  foreach ($required_batch_actions as $required_batch_action) {
    if (!$required_batch_action['is_ingest_prerequisite']) {
      $ingest_prerequisite_ids[] = $required_batch_action['action_id'];
    }
  }
  foreach ($item_record_transactions as $transaction_obj) {
    $action_is_ingest_prerequisite = !(array_search($transaction_obj->action_id, $ingest_prerequisite_ids) === FALSE);
    if (!$action_is_ingest_prerequisite) {
      $retval &= ($transaction_obj->transaction_id <> -1);
    }
  }
  return $retval;
}

/**
 * Is the action_id a batch_action or an item_action?
 *
 * @param integer $action_record
 *   The `islandora_digital_workflow_actions` record for the action OR a workflow
 * sequence array that is a join between the islandora_digital_workflow_actions
 * with the islandora_digital_workflow_sequence table OR a record from 
 * islandora_digital_workflow_get_batch_action_options or 
 * islandora_digital_workflow_get_item_action_options.  All of these arrays should
 * contain the required 'is_batch_action' array key.
 * @return boolean
 *   Whether or not the given $action_id represnts a batch_action (else it is
 * an item_action's identifier).
 */
function islandora_digital_workflow_is_batch_action($action_record) {
  if (is_array($action_record) && array_key_exists('is_batch_action', $action_record)) {
    return $action_record['is_batch_action'];
  }
  else {
    return FALSE;
  }
}

/**
 * This will loop through all workflow sequence actions to identify which
 * workflow sequences use the action.
 *
 * @param integer $action_id
 *   The `islandora_digital_workflow_actions` table's action_id value.
 * @return mixed
 *   Can return NULL if no sequence is found to use the action_id - otherwise,
 * the return value is an array of the names of all sequences.  If the user has
 * access to ISLANDORA_DIGITAL_WORKFLOW_MANAGE_SEQUENCES, the names will be
 * returned as links to the manage page for this sequence.
 */
function islandora_digital_workflow_sequences_using_action($action_id) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
  $workflow_sequences = islandora_digital_workflow_get_workflow_sequences();
  $workflow_sequence_actions = islandora_digital_workflow_get_workflow_sequence_actions();
  $workflow_sequences_using_action = islandora_digital_workflow_using_action($workflow_sequence_actions, $action_id);
  if (count($workflow_sequences_using_action) > 0) {
    $ret_sequence_links = array();
    foreach ($workflow_sequences_using_action as $workflow_sequence_id) {
      $ret_sequence_links[] = ((user_access(ISLANDORA_DIGITAL_WORKFLOW_MANAGE_SEQUENCES)) ?
          l($workflow_sequences[$workflow_sequence_id]['name'], '/admin/islandora/islandora_digital_workflow/workflow_sequences',
              array(
                  'fragment' => $workflow_sequences[$workflow_sequence_id]['name'],
                  'external' => TRUE,
                  'attributes' => array('target' => '_blank'),
              )
          ) :
          $workflow_sequences[$workflow_sequence_id]);
    }
    return $ret_sequence_links;
  }
  else {
    return FALSE;
  }
}

/**
 * Determines which workflow sequences actually use a given action.
 *
 * @param array $workflow_sequence_actions
 *   An array of workflow sequences and all of their actions.  The key to the
 * top-level array is the workflow_sequence_id value.
 * @param integer $action_id
 *   An action_id value that matches an action on any batch, item, or page.
 * @return array
 *   an array of workflow_sequence_id values that are being used by the supplied
 * $action_id value.
 */
function islandora_digital_workflow_using_action($workflow_sequence_actions, $action_id) {
  $workflow_sequences_using_action = array();
  foreach ($workflow_sequence_actions as $workflow_sequence_id => $workflow_sequence_actions) {
    foreach ($workflow_sequence_actions as $workflow_sequence_action) {
      if (array_key_exists(0, $workflow_sequence_action)) {
        $workflow_sequence_action = array_shift($workflow_sequence_action);
      }
      if ($workflow_sequence_action['action_id'] == $action_id) {
        $workflow_sequences_using_action[$workflow_sequence_id] = $workflow_sequence_id;
      }
    }
  }
  return $workflow_sequences_using_action;
}

/**
 * This will move the file/s related to any given batch item to the destination
 * location.
 *
 * @param integer $batch_item_id
 *   The batch_item_id identifier that needs to have its file moved.
 * @param string $destination_root
 *   The root path -- could be the problems folder, could be delivery.
 * @param string $problem_notes
 *   Any notes that the user entered for setting the action to problem state.
 * ONLY applies when setting the action to the problem state IDW_ACTION_PROBLEM.
 */
function islandora_digital_workflow_move_batch_item_file($batch_item_id, $destination_root, $problem_notes = '') {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
  //  load the batch_item record for this batch_item_id to get the filename
  $item_record_results = islandora_digital_workflow_get_batch_item_record($batch_item_id);
  $batch_item_obj = array_pop($item_record_results);
  //  load the batch related to this batch_item_id to get the batch name (path under the root)
  $batch_record = islandora_digital_workflow_get_batch_record_by_batch_id($batch_item_obj->batch_id);

  $path_parts = pathinfo($batch_item_obj->filename);
  $path_split = explode("/", $batch_item_obj->filename);
  $item_name = $path_split[count($path_split) - 2];
  $source_root = $path_parts['dirname'];

  // Check that the source folder exists.
  if (!file_exists($source_root)) {
    drupal_set_message(t('The item folder "@source_root" does not exist and the files can not be moved to the "@destination_root" folder.', array(
        '@source_root' => $source_root,
        '@destination_root' => $destination_root
    )), 'warning');
  }
  else {
    $source_files = islandora_digital_workflow_scan_files($source_root);
    $batch_path = $destination_root . '/' . $batch_record['batch_name'];
    if (!file_exists($batch_path)) {
      mkdir($batch_path);
    }
    $item_path = $batch_path . '/' . $item_name;
    if (!file_exists($item_path)) {
      mkdir($item_path);
    }

    @list($junk, $short_item_filename) = explode("/" . $batch_record['batch_name'] . "/", $batch_item_obj->filename);
    // Append the info on which item is selected in the notes.
    $problem_notes .= (($problem_notes) ? "\n\n----------------------\n" : "").
        "Item title:    " . $batch_item_obj->title . "\n" .
        "Item filename: " . $short_item_filename . "\n" .
        "Batch name:    " . $batch_record['batch_name'] . "\n";
    file_put_contents($item_path . '/problem_notes.txt', $problem_notes);

    $command = 'mv ' . $source_root . '/* ' . $item_path . '/.';
    $output = $return_var = array();
    exec($command, $output, $return_var);
  }
}

/**
 * This will copy the FRX and SQL files from the forena_reports/ folder into the
 * forena configured folder locations so they are able to be used after installing.
 *
 * This will also be called from the admin interface so that the reports would be
 * merged at that point in time ONLY if they haven't been deployed yet.
 *
 * @return BOOLEAN
 *   TRUE when the deployment succeeds to move the files or if it was already
 * deployed.
 */
function islandora_digital_workflow_check_and_deploy_forena_reports() {
  if (!module_exists('forena')) {
    $link_att = array('attributes' => array('target' => '_blank'));
    drupal_set_message(t('The Forena reporting module is not installed.  ') .
        l('Forena', 'https://git.drupal.org/project/forena.git', $link_att) . ' | ' .
        l('ulsdevteam "Forena"', 'https://github.com/ulsdevteam/forena.git', $link_att), 'error'
    );
    return FALSE;
  }

  // Get the forena report FRX and SQL file deployment locations.
  module_load_include('module', 'forena', 'forena');

  $forena_frx_path = forena_report_path();
  $forena_data_source_idw = Frx::RepoMan()->repository('islandora_digital_workflow_sql');
  if (is_object($forena_data_source_idw) && $forena_frx_path) {
    $forena_sql_path = $forena_data_source_idw->block_path;
    $frx_files = islandora_digital_workflow_scan_files($forena_frx_path);
    $sql_files = islandora_digital_workflow_scan_files($forena_sql_path);

    // Finally set up the file paths needed if deploying.
    $this_folder = dirname(__FILE__);
    @list($sites_location, $junk) = explode("sites/", $this_folder);
    // Since $this_folder is going to end with /includes, strip that off
    $this_folder = str_replace("/includes", "", $this_folder);

    // Perform the file deployment from the islandora_digital_workflow/forena_reports
    // folder locations into the FRX and SQL folders.
    // Only deploy the FRX files if they do not exist
    if (count($frx_files) < 1) {
      $from =  $this_folder . '/forena_reports/frx';
      $to = $sites_location . $forena_frx_path;
      islandora_digital_workflow_deploy_files_to_forena_install($from, $to);
      drupal_set_message(t('Deployed Forena FRX files to: "@to"<br>' .
          'from: "@from".', array(
              '@to' => $to,
              '@from' => $from,
          )), 'info');
    }
    // Only deploy the SQL files if they do not exist
    if (count($sql_files) < 1) {
      $from = $this_folder . '/forena_reports/sql';
      $to = $sites_location . $forena_sql_path;
      islandora_digital_workflow_deploy_files_to_forena_install($from, $to);
      drupal_set_message(t('Deployed Forena SQL files to: "@to"<br>' .
          'from: "@from".', array(
              '@to' => $to,
              '@from' => $from,
          )), 'info');
    }
  }
  else {
    module_load_include('inc', 'islandora_digital_workflow', 'includes/db');

    // the 'islandora_digital_workflow_sql' forena data source did not exist --
    // create it here and display the message that the page needs to reload.
    drupal_set_message(t('The "islandora_digital_workflow_sql" Forena data ' .
        'source is not configured and will be created.  You will need to ' .
        'reload this page in order to complete the deployment of the Islandora ' .
        'Digital Workflow reports and SQL files.'), 'error');
    islandora_digital_workflow_create_islandora_digital_workflow_sql_configuration();
    return FALSE;
  }
  return TRUE;
}

/**
 * This will create the destination folder if needed (if possible), and copy
 * all files from the "$from" to the "$to"
 *
 * @param string $from
 *   The folder from which the files are deployed.
 * @param string $to
 *   The folder to where the folders are deployed.
 */
function islandora_digital_workflow_deploy_files_to_forena_install($from, $to) {
  // make sure that the destination and source folders do not end with "/" just in case.
  $to = rtrim($to, '/');

  if (!file_exists($to)) {
    $success = mkdir($to);
    if (!$success) {
      drupal_set_message(t('The destination folder did not exist and could not ' .
          'be created.  Please check the file permissions of the parent folder for "@to".', array(
              '@to' => $to
          )), 'error');
    }
    return FALSE;
  }
  @list($junk, $keep_from) = explode("sites/", $from);
  $files = islandora_digital_workflow_file_scandir('sites/' . $keep_from, '', TRUE);
  $from = rtrim($from, '/');
  $success = FALSE;
  foreach ($files as $file) {
    $success &= copy($from . '/' . $file, $to . '/' . $file);
  }
  if (!$success) {
      drupal_set_message(t('The forena files could not be deployed due to permissions ' .
          'on the folder.  Please check the file permissions of the parent folder for "@to".', array(
              '@to' => $to
          )), 'error');
  }
  return $success;
}

/**
 * Determines whether or not the object is paged-content.
 * 
 * @param AbstractObject|string $object_or_string
 *   Either an Islandora object or a string value of a fedora model.
 * @return boolean
 *   Whether or not the object is a paged-content model.
 */
function islandora_digital_workflow_is_paged_object($object_or_string) {
  $paged_content_models = islandora_digital_workflow_retrieve_applicable_paged_content_cmodels();
  if (is_object($object_or_string)) {
    return (is_object($object_or_string)) ? array_intersect($paged_content_models, $object_or_string->models) : FALSE;
  }
  elseif (is_null($object_or_string)) {
    return FALSE;
  }
  else {
    $array = array($object_or_string);
    return array_intersect($paged_content_models, $array);
  }
}

/**
 * Returns whether or not the current Drupal path needs to load the module's javascript.
 *
 * @param string $path
 *   The request menu path - matches a $menu route from islandora_digital_workflow_menu.module,
 * see islandora_digital_workflow_menu().
 * @return boolean
 *   Whether or not the javascript should be loaded.
 */
function islandora_digital_workflow_path_needs_js($path) {
  $paths_needing_js = array(
      'node/%/items',
      'node/%/edit_transaction/%',
      'node/%/add_transaction/%/%',
      // repeat since the "node_load" function expects this to be named %node
      'node/%node/items',
      'node/%node/edit_transaction/%',
      'node/%node/add_transaction/%/%',
      // admin screen for workflow_sequence_actions/edit/%
      'admin/islandora/islandora_digital_workflow/workflow_sequence_actions/add',
      'admin/islandora/islandora_digital_workflow/workflow_sequence_actions/edit/%'
  );
  return ($path && !is_array($path)) ? (!(array_search($path, $paths_needing_js) === FALSE))
      : FALSE;
}

/**
 * Converts a string to a valid UNIX filename.
 * @param $string The filename to be converted
 * @return $string The filename converted
 */
function islandora_digital_workflow_convert_to_filename($string) {
  // Replace spaces with underscores and makes the string lowercase
  $string = str_replace (array(" ", "-", ")", "(", "[", "]", "{", "}", "?", "!"), "_", $string);
  $string = str_replace ("..", ".", $string);
  $string = strtolower ($string);

  // Match any character that is not in our whitelist
  preg_match_all ("/[^0-9^a-z^_^.]/", $string, $matches);

  // Loop through the matches with foreach
  foreach ($matches[0] as $value) {
    $string = str_replace($value, "", $string);
  }
  return $string;
}

/**
 * This will create collection objects and return nothing.  The collections
 * will be used as the --parent parameter for ingesting objects.
 *
 * @param array $collection_pids
 */
function islandora_digital_workflow_create_review_collections($collection_pids) {
  if (!$collection_pids) {
    return FALSE;
  }
  $module_path = drupal_get_path('module', 'islandora_digital_workflow');
  $connection = islandora_get_tuque_connection();
  $repository = $connection->repository;
  foreach ($collection_pids as $pid) {
    $copy_from_pid = str_replace("_review", "", $pid);
    $copy_from_object = islandora_object_load($copy_from_pid);
    $use_label = (is_object($copy_from_object) ? $copy_from_object->label : 'unknown review collection');
    $object_review = islandora_object_load($pid);
    if (!is_object($object_review)) {
      $object_review = $repository->constructObject($pid);
      $object_review->label = $use_label;
      $object_review->models = 'islandora:collectionCModel';

      // COLLECTION_POLICY Datastream.
      $datastream = $object_review->constructDatastream('COLLECTION_POLICY', 'X');
      $datastream->label = 'COLLECTION_POLICY';
      $datastream->mimetype = 'application/xml';
      $datastream->setContentFromFile("$module_path/xml/islandora_basic_collection_collection_policy.xml", FALSE);
      $object_review->ingestDatastream($datastream);

      $repository->ingestObject($object_review);
    }
    else {
      $alias = str_replace(array('[', ']', '"', "'"), '', str_replace(array(' ', ':', '_'), '-', strtolower($use_label)));
      $source = 'islandora/object/' . $pid;
      // copy the copy_from_object's pathauto record for this new object_review
      $results = db_query("SELECT * FROM url_alias WHERE source like " .
          "'%" . $copy_from_pid . "%' LIMIT 1")->fetchAll();
      foreach ($results as $row) {
        $alias = $row->alias . '-review';
        $source = str_replace($copy_from_pid, $copy_from_pid . "_review", $row->source);
      }
      // Because pathauto can make multiple references using different routes, delete
      // all possible source routes.
      db_query("DELETE FROM url_alias WHERE alias = '" . $alias . "'");
      // Now, the
      db_query("REPLACE INTO url_alias (`alias`,`source`,`language`) VALUES ('" . $alias . "','" . $source . "','und')");
    }

    if (is_object($copy_from_object)) {
      $object_review->relationships = $copy_from_object->relationships;
      foreach ($copy_from_object as $copy_from_datastream) {
        $datastream = $object_review->constructDatastream($copy_from_datastream->id);
        $datastream->label = $copy_from_datastream->label . ' {review}';
        $datastream->mimeType = $copy_from_datastream->mimeType;

        $datastream->setContentFromString($copy_from_datastream->content);
        $object_review->ingestDatastream($datastream);
      }
    }
  }
  return;
}

/**
 * Helper function to return the array of glyph filenames -- called from various
 * places in the code.
 *
 * @return array
 *   Straight array of filenames used for the static glyphs.
 */
function islandora_digital_workflow_static_glyph_filenames() {
  return array(
      'add-marc-record' => 'images/add-marc-record_20.png',
      'add-mods-record' => 'images/add-mods-record_20.png',
      'check-in-physical-item' => 'images/check-in-physical-item_20.png',
      'check-item-against-physical-object' => 'images/check-item-against-physical-object_20.png',
      'check-out-physical-item' => 'images/check-out-physical-item_20.png',
      'ingest-item-into-repository' => 'images/ingest-item-into-repository_20.png',
      'rescanned-the-item' => 'images/rescanned-the-item_20.png',
      'scan-the-item' => 'images/scan-the-item_20.png',
      'system-made-derivatives' => 'images/system-made-derivatives_20.png',
      'unconfigured-reserved-action' => 'images/unconfigured-reserved-action_20.png',
      'custom-glyph-01' => 'images/custom-glyph-01_20.png',
      'custom-glyph-02' => 'images/custom-glyph-02_20.png',
      'custom-glyph-03' => 'images/custom-glyph-03_20.png',
      'custom-glyph-04' => 'images/custom-glyph-04_20.png',
      'custom-glyph-05' => 'images/custom-glyph-05_20.png',
      'custom-glyph-06' => 'images/custom-glyph-06_20.png',
      'custom-glyph-07' => 'images/custom-glyph-07_20.png',
      'custom-glyph-08' => 'images/custom-glyph-08_20.png',
      'custom-glyph-09' => 'images/custom-glyph-09_20.png',
      'custom-glyph-10' => 'images/custom-glyph-10_20.png',
      'custom-glyph-11' => 'images/custom-glyph-11_20.png',
      'custom-glyph-12' => 'images/custom-glyph-12_20.png',
      'custom-glyph-13' => 'images/custom-glyph-13_20.png',
      'custom-glyph-14' => 'images/custom-glyph-14_20.png',
      'custom-glyph-15' => 'images/custom-glyph-15_20.png',
      'custom-glyph-16' => 'images/custom-glyph-16_20.png',
      'custom-glyph-17' => 'images/custom-glyph-17_20.png',
      'custom-glyph-18' => 'images/custom-glyph-18_20.png',
      'custom-glyph-19' => 'images/custom-glyph-19_20.png',
      'custom-glyph-20' => 'images/custom-glyph-20_20.png',
      'custom-glyph-21' => 'images/custom-glyph-21_20.png',
      );
}

/**
 * This has dual-purposes:
 *   1) generate the output for a set of items to be processed with a given mode
 *   2) add the items / processing mode to the islandora_digital_workflow_process_set
 *      and islandora_digital_workflow_process_set_items tables.
 *
 * @param string $title
 *   The title for the display block.
 * @param string $mode
 *   The processing mode.
 * @param type $items
 *   Array of item identifiers.
 * @return string
 *   HTML output for the process set.
 */
function islandora_digital_workflow_make_process_set($title, $mode, $items) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
  // If there are items, look up the identifiers' item_id values from the
  // items table, else return an empty string.
  $item_ids = islandora_digital_workflow_get_batch_item_ids($items);
  $process_set_id = islandora_digital_workflow_make_process_set_records($mode, $item_ids);
  // Before leaving, close the mysql connection.
  return theme('islandora_digital_workflow_process_set_display', array(
          'title' => $title,
          'process_set_id' => $process_set_id,
          'items' => $items));
}

/**
 * Update the batch status record for the matching a given $ingested_object.  This will
 * create an item record if it does not exist already.
 *
 * @param object $ingested_object
 * @param integer $transaction_action_id
 */
function islandora_digital_workflow_set_status_for_pid($ingested_object, $action_id) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');

  // Take the namespace off of the object id to the identifier
  $ingest_namespace = islandora_digital_workflow_get_ingest_namespace();
  @list($namespace, $identifier) = explode(":", $ingested_object->id);
  // Look up the batch_items record for this identifier -- and add a transaction
  // to set the provided $action_id value.
  $batch_item_id = islandora_digital_workflow_get_batch_item_id_by_identifier($identifier);
  if (!islandora_digital_workflow_insert_transactions_record($action_id, NULL, $batch_item_id, NULL, '')) {
    $variables = array('@ingested_pid' => $ingested_object->id, '@action_id' => $action_id);
    watchdog('islandora_digital_workflow', 'The system was not able to add a ' .
        'transaction [@action_id.] for a recently ingested object with PID ' .
        '"@ingested_pid". If this object was ingested outside of the Islandora ' .
        'Digital Workflow, this message can be ignored.', $variables,
            WATCHDOG_NOTICE);
  }
}

/**
 * Helper function to process the input identifiers for "process identifiers".
 *
 * @param string $posted_text
 *   Text that was entered in the identifiers textarea control.
 * @param boolean $trim_lines
 *   Whether or not to trim the whitespace from lines
 * @return array
 *   An array with each line as an array element.
 */
function islandora_digital_workflow_post_text_to_array($posted_text, $trim_lines = TRUE) {
  $as_array = explode("\n", $posted_text);
  if ($trim_lines) {
    foreach ($as_array as $idx => $line) {
      $as_array[$idx] = trim($line);
    }
  }
  return $as_array;
}

/**
 * Returns the select box options for MODS typeOfResource field.
 *
 * Values taken from https://www.loc.gov/standards/mods/userguide/typeofresource.html
 *
 * @return array
 *   An array of the typeOfResource values.
 */
function islandora_digital_workflow_get_default_type_of_resource_options($include_select_value_option = FALSE) {
  $retarray = array();
  if ($include_select_value_option) {
    $retarray[] = 'Select a type of resource';
  }
  $retarray['text'] = 'text';
  $retarray['cartographic'] = 'cartographic';
  $retarray['notated music'] = 'notated music';
  $retarray['sound recording'] = 'sound recording';
  $retarray['sound recording-musical'] = 'sound recording-musical';
  $retarray['sound recording-nonmusical'] = 'sound recording-nonmusical';
  $retarray['still image'] = 'still image';
  $retarray['moving image'] = 'moving image';
  $retarray['three dimensional object'] = 'three dimensional object';
  $retarray['software; multimedia'] = 'software; multimedia';
  $retarray['mixed material'] = 'mixed material';
  return $retarray;
}

/**
 * This will purge a specific datastream from an object or all of its children.
 *
 * @param type $identifiers
 *   Array of identifiers to affect.
 * @param string $dsid
 *   The datastream identifier such as OCR, OBJ, MODS, etc.
 * @return string
 *   HTML markup of the purge results.
 */
function islandora_digital_workflow_purge_ds_for_identifiers($identifiers, $dsid) {
  module_load_include('inc', 'islandora_paged_content', 'includes/utilities');

  $missing_identifiers = array();
  $identifiers_arr = explode("\n", $identifiers);
  $markup = '<div class="text-report">';
  $ingest_namespace = islandora_digital_workflow_get_ingest_namespace();
  foreach ($identifiers_arr as $identifier) {
    $identifier = trim($identifier);
    $pid = $ingest_namespace . $identifier;
    $islandora_object = islandora_object_load($pid);
    if (is_object($islandora_object)) {
      $is_paged = islandora_digital_workflow_is_paged_object($islandora_object);
      $pages = ($is_paged) ? islandora_paged_content_get_pages($islandora_object) : array();
      islandora_digital_workflow_purge_childpages_datastreams($islandora_object, $pages, $dsid);
      if (!$is_paged) {
        @$islandora_object->purgeDatastream($dsid);
      }
      $markup .= '<div class="good">Successfully purged ' . $dsid . ' datastream for ' .
          (($is_paged ? 'all pages of ' : '')) . $pid . '.</div>';
    }
    else {
      if ($identifier) {
        $missing_identifiers[] = $identifier;
      }
    }
  }
  if (count($missing_identifiers) > 0) {
    drupal_set_message(t('Some identifiers could not be found ') . implode(", ", $missing_identifiers), 'warning');
  }
  $markup .= '</div>';
  return $markup;
}

/**
 * Helper function that will purge the datastreams from the child page objects - used
 * after PDF, or OCR datastreams have been aggregated successfully.  This will check for the
 * datastream to exist on the parent object before deleting the child's datastreams.
 *
 */
function islandora_digital_workflow_purge_childpages_datastreams($islandora_object, $pages, $dsid) {
  // Remove the PDF datastreams for all child pages if the PDF has been aggregated
  if (isset($islandora_object[$dsid])) {
    foreach ($pages as $page) {
      $page_object = islandora_object_load($page['pid']);
      $datastream = @$page_object[$dsid];
      if ($datastream) {
        @$page_object->purgeDatastream($dsid);
      }
    }
  }
}

/**
 * This will sort the array of objects by their weight field value.
 *
 * If the object does not have a weight field value, the $fallback_field_name value will
 * be used.
 *
 * @param array $array_of_objects
 *   Array of objects - for example an array of Drupal vocabulary terms.
 * @param string $fallback_field_name
 *   OPTIONAL parameter to specify the field to use if any of the objects does
 * not have the weight field set.
 * @param string $final_key_field
 *   Optional parameter to specify the object's field name used as the key for
 * the final array.
 * @return array
 *   Structure should be the same as the passed array, but sorted items by
 * their underlying objects' weight values.
 */
function islandora_digital_workflow_sort_array_objects_by_weight($array_of_objects, $fallback_field_name = 'tid', $final_key_field = 'tid') {
  $sorted_array = array();
  $sort_field = 'weight';
  // Loop through to be sure that all have a weight -- else, use the tid field,
  // else set to 'tid'
  foreach ($array_of_objects as $object_index => $object) {
    if (!isset($object->weight)) {
      // Other fields on an object *could be* tested and coded for here.
      $sort_field = $fallback_field_name;
    }
  }

  foreach ($array_of_objects as $object_index => $object) {
    $sorted_array[$object->$sort_field][] = array($object_index => $object);
  }

  ksort($sorted_array);
  if (count($sorted_array) > 0) {
    $return_array = array();
    foreach ($sorted_array as $sorted_kv => $object_array) {
      foreach ($object_array as $obj_index => $object) {
        $object = array_pop($object);
        $return_array[$object->$final_key_field] = $object;
      }
    }
    return $return_array;
  } else {
    return $array_of_objects;
  }
}

/**
 * Will sanitize a string so that it is legal to use as a filename.
 *
 * This could cause a problem if two similar strings are passed into here with
 * the intention that they need to represent two separate files, and they both
 * result in the same sanitized string.
 *
 * For example:  "sanitized?.zip" and "sanitized***[].zip" could both end up
 *   with a name of "sanitized.zip"
 *
 * @param string $filename
 *   An intended filename value.
 * @param string $extension
 *   Optional parameter to set an extension for the file.
 * @return string
 *   The sanitized filename should be legal for any file system.
 */
function islandora_digital_workflow_filename_sanitize($filename, $extension = '') {
  $orig_filename_pathinfo = pathinfo($filename);
  $path = ($extension && array_key_exists('extension', $orig_filename_pathinfo)) ?
      str_replace('.' . $orig_filename_pathinfo['extension'], $extension, $filename) :
      $filename . $extension;

  $pathinfo = pathinfo($path);
  $dirname = (array_key_exists('dirname', $pathinfo) ? (($pathinfo['dirname'] <> '.') ? $pathinfo['dirname'] : '') : '');
  if (strstr($dirname, '/') <> '') {
    $dirname_parts = explode("/", $dirname);
    foreach ($dirname_parts as $idx => $dirname_part) {
      $dirname_parts[$idx] = islandora_digital_workflow_filename_sanitize($dirname_part);
    }
    $dirname = implode("/", $dirname_parts);
  }

  // Make sure it ends with a slash.
  $dirname = (($dirname) ? rtrim($dirname, '/') . '/' : '');
  $sanitized = (array_key_exists('basename', $pathinfo) ? $pathinfo['basename'] : $path);
  $sanitized = mb_ereg_replace("([^\w\s\d\-_~.])", '', $sanitized);
  $sanitized = mb_ereg_replace("([\.]{2,})", '', $sanitized);

  return $dirname . $sanitized;
}

/**
 * Returns whether or not the item is paged-content.
 *
 * @param integer $batch_item_id
 *   The islandora_digital_workflow_batch_items.batch_item_id value
 * @param array $batch_record
 *   A islandora_digital_workflow_batch record as an array.
 * @return boolean
 *   Whether or not the item is paged-content.
 */
function islandora_digital_workflow_item_is_paged_content($batch_item_id, $batch_record = array()) {
  $item_record_results = islandora_digital_workflow_get_batch_item_record($batch_item_id);
  $batch_item_obj = array_pop($item_record_results);
  if (is_object($batch_item_obj)) {
    return islandora_digital_workflow_is_paged_object($batch_item_obj->islandora_model);
  }
  if (array_key_exists('islandora_model', $batch_record)) {
    return islandora_digital_workflow_is_paged_object($batch_record['islandora_model']);
  }

  return FALSE;
}

/**
 * This will copy the file for the given $batch_item_id item to the working
 * directory for the batch.
 *
 * The path to the delivery files is looked up using the item's batch record
 * and the item record.  Additionally, this code will check the configuration
 * and optionally clean up (delete) the delivery files.
 *
 * @param integer $batch_item_id
 *   $batch_item_id value matching an individual batch item.
 * @param boolean $is_problems_location
 *   (Optional) This defaults to FALSE, so most of the code calls this to copy
 * from the delivery folder...
 * @return boolean
 *   Whether or not there was a successful file copy operation.
 */
function islandora_digital_workflow_copy_delivery_file($batch_item_id, $is_problems_location = FALSE) {
  $item_record_results = islandora_digital_workflow_get_batch_item_record($batch_item_id);
  $batch_item_obj = array_pop($item_record_results);
  $item_is_paged = islandora_digital_workflow_item_is_paged_content($batch_item_id);

  if (!is_object($batch_item_obj)) {
    drupal_set_message(t('There was a problem accessing the batch_item record for ' .
        '@batch_item_id.', array(
            '@batch_item_id' => $batch_item_id,
            ))
        );
    return FALSE;
  }
  $sanitized_delivery_filename = islandora_digital_workflow_get_item_delivery_filename($batch_item_obj);
  $working_filename = $batch_item_obj->filename;
  if ($item_is_paged) {
    module_load_include('inc', 'islandora_digital_workflow', 'includes/file_utilities');

    $batch_record = islandora_digital_workflow_get_batch_record_by_batch_id($batch_item_obj->batch_id);
    $item_working_folder = islandora_digital_workflow_batch_path($batch_record, FALSE) . '/' . $batch_item_obj->identifier;
    // Scan the source folder - and copy over all page resources
    $item_folder = islandora_digital_workflow_get_item_delivery_filename($batch_item_obj);
    $item_files = islandora_digital_workflow_scan_files($item_folder);
    $copied = islandora_digital_workflow_copy_resequence_files($item_files, $item_folder, $item_working_folder);
  }
  else {
    $copied = copy($sanitized_delivery_filename, $working_filename);
  }

  if ($copied) {
    // Load the batch record for this item so that the sizes can be recalculated.
    $batch_record = islandora_digital_workflow_get_batch_record_by_batch_id($batch_item_obj->batch_id);
    islandora_digital_workflow_recalc_batch_files($batch_record);

    if (variable_get('islandora_digital_workflow_delete_delivery_files', 0)) {
      drupal_set_message('Delete files after scan copies made set');
      $deleted = @unlink($sanitized_delivery_filename);
      if (!$deleted) {
        drupal_set_message(t('The file @master_filename_basename was copied to ' .
            '@working_filename, but the original file "@sanitized_delivery_filename" ' .
            'could not be deleted.  Check file permissions on the server location ' .
            'for the source path.', array(
                '@sanitized_delivery_filename' => $sanitized_delivery_filename,
                '@master_filename_basename' => $batch_item_obj->master_filename_basename,
                '@working_filename' => $working_filename
            )));
      }
      // Additionally, when islandora_digital_workflow_delete_delivery_files is set,
      // if the item is the last item in the batch to be copied, then ALSO try to delete
      // the delivery folder itself.



    }
    else {
      drupal_set_message('Delete files after scan copies made NOT set');
    }
  }
  else {
    drupal_set_message(t('The file @master_filename_basename could not be copied ' .
        'to @working_filename.  Check file permissions on the server location for ' .
        'the destination path.', array(
            '@master_filename_basename' => $batch_item_obj->master_filename_basename,
            '@working_filename' => $working_filename
        )));
  }
  return $copied;
}

/**
 * This will try to return the true path to the item's master file.
 *
 * @param ingeger|stdClass $batch_item
 *   This is the item for which to get the delivery filename.
 * @param mixed $batch_record
 *   Used to get the path to the batch's host path.
 * @return boolean
 */
function islandora_digital_workflow_get_item_delivery_filename($batch_item, $batch_record = NULL) {
  // Only load the batch_record if the calling code passed batch_item as the object's
  // batch_item_id value.
  if (!is_object($batch_item)) {
    // The value must be the batch_item_id integer value.  Load the object as $batch_item.
    $item_record_results = islandora_digital_workflow_get_batch_item_record($batch_item);
    $batch_item = array_pop($item_record_results);
    if (!is_object($batch_item)) {
      drupal_set_message(t('There was a problem accessing the batch_item record for ' .
          '@batch_item_id.', array(
              '@batch_item_id' => $batch_item,
              ))
          );
      return FALSE;
    }
  }
  // Look up this batch_item's batch so that the full path to the files can be
  // determined.
  // Only load the batch_record if the calling code has not passed the batch_record array.
  $batch_record = !(is_null($batch_record)) ? $batch_record : islandora_digital_workflow_get_batch_record_by_batch_id($batch_item->batch_id);
  // The great thing is that this function call will CREATE the directory
  // if it doesn't exist.
  $batch_path = islandora_digital_workflow_batch_path($batch_record, TRUE);

  // Need to change to "Incoming Delivery Directory" and get the object's file --
  // and copy the item's file into the "Ingest Working Directory" location.
  // If the configuration is set to islandora_digital_workflow_delete_delivery_files
  // the code should try to delete the item.
  $target_filename = $batch_item->filename;
  $pathinfo = pathinfo($target_filename);
  $basename = (array_key_exists('basename', $pathinfo) ? (($pathinfo['basename'] <> '.') ? $pathinfo['basename'] : '') : '');
  $item_path = $batch_path . '/' . $batch_item->identifier;
  $delivery_filename = $item_path . '/' . $basename;
  $sanitized_delivery_filename = islandora_digital_workflow_filename_sanitize($delivery_filename);
  return $sanitized_delivery_filename;
}

/**
 * This helper routine will call the code to recalculate each of the provided 
 * batches.  The $output_* variables are passed by reference are are updated
 * within the code below.
 *
 * @param array|stdClass $batch_record
 *   The batch's islandora_digital_workflow_batch record.
 * @param string $output_contents
 *   HTML markup for the recalculating of the batch/es.
 * @param type $output_footer
 *   HTML markup for process form for this processing mode.
 */
function islandora_digital_workflow_recalc_batch_files($batch_record, &$output_contents = array(), &$output_footer = array()) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
  $batch_record = (array) $batch_record;
  $batch_path = islandora_digital_workflow_batch_path($batch_record);
  $batch_folders = islandora_digital_workflow_find_folders_files($batch_path);
  $item_data = array();
  if (count($batch_folders) > 0) {
    $output_contents[] = '<p>Processing batch "' . $batch_record['batch_name'] . '"</p>';
    foreach ($batch_folders as $item_identifier => $found_folders_files) {
      foreach ($found_folders_files['filesize_date_files_raw'] as $filesize_date_files_raw) {
        $record_identifier = $batch_record['ingest_namespace'] . ":" . $item_identifier;
        $item_data[$record_identifier][] = $filesize_date_files_raw;
      }
    }
    $output_footer[] = islandora_digital_workflow_update_item_file_data($batch_record, $item_data);
  }
}