<?php

/**
 * This array mapping is needed from various places in the code.
 * 
 * Provided here as a function so that key lookup is possible as well as 
 * searching for the "Content type" name given the model name.
 *
 * @return type
 */
function islandora_digital_workflow_content_type_to_islandora_model_mappings() {
  return array(
    'Audio' => 'islandora:sp-audioCModel',
    'Book' => 'islandora:bookCModel',
    'Disk Image' => 'islandora:sp_disk_image',
    'Finding Aid' => 'islandora:findingAidCModel',
    'Image' => 'islandora:sp_basic_image',
    'Large Image' => 'islandora:sp_large_image_cmodel',
    'Manuscript' => 'islandora:manuscriptCModel',
    'Newspaper' => 'islandora:newspaperIssueCModel',
    'Oral History' => 'islandora:oralhistoriesCModel',
    'PDF' => 'islandora:sp_pdf',
    'Video' => 'islandora:sp_videoCModel',
    'Web Archive' => 'islandora:sp_web_archive',
  );
}

/**
 * This will return the islandora model name for a given content type.
 *
 * @param type $content_type
 * @return type
 */
function islandora_digital_workflow_content_type_to_islandora_model($content_type) {
  $islandora_content_type_mapping = islandora_digital_workflow_content_type_to_islandora_model_mappings();
  return (array_key_exists($content_type, $islandora_content_type_mapping)) ? $islandora_content_type_mapping[$content_type] : '';
}

function islandora_digital_workflow_create_batch_record_by_form($form_state) {
  global $user;
  // TODO: add a validate call instead of checking the values of $form_state here.
  if (is_array($form_state) && isset($form_state['values']['batch_name'])
    && $form_state['values']['batch_name']) {
    $batch_name = $form_state['values']['batch_name'];
    $record = islandora_digital_workflow_get_batch_record_by_name($batch_name);
    if ((is_null($record))) {
      $batch_id = db_insert('islandora_digital_workflow_batch')
      ->fields(array(
        'batch_external_id' => $batch_name,
        'batch_description' => 'unset',
        'batch_default_perm_notes' => '',
        'batch_default_CR_holder' => '',
        'structural_metadata_treatment' => '',
        'image_editing_treatment' => '',
        'blank_and_missing_treatment' => '',
        'page_edge_treatment' => '',
        'batch_condition_handling' => '',
        'batch_host' => '',
        'user' => $user->name,
      ))
      ->execute();
    }
    else {
      $batch_id = db_update('islandora_digital_workflow_batch')
      ->fields(array(
        'batch_external_id' => $batch_name,
        'batch_description' => 'unset',
        'batch_default_perm_notes' => '',
        'batch_default_CR_holder' => '',
        'structural_metadata_treatment' => '',
        'image_editing_treatment' => '',
        'blank_and_missing_treatment' => '',
        'page_edge_treatment' => '',
        'batch_condition_handling' => '',
        'batch_host' => '',
        'user' => $user->name,
      ))
      ->condition('batch_external_id', $batch_name)
      ->execute();
    }
    return islandora_digital_workflow_get_batch_record_by_name($batch_name);
  }
  else {
    return NULL;
  }
}

function islandora_digital_workflow_get_batch_record_by_name($batch_name) {
  if ($batch_name) {
    $query = db_select('islandora_digital_workflow_batch', 'b')
        ->fields('b')
        ->condition('b.batch_external_id', $batch_name)
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
 * This still provides a test as to whether or not a field gets into the initial XML from the sheet - and the headings title
 * that would match for that field.
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

function islandora_digital_workflow_get_mysql_options($table_name, $option_name = '', $key_field, $value_field, $sort_field = '', $option_value = 0) {
  $query = 'SELECT ' . $key_field . ', ' . $value_field . ' FROM ' . $table_name . ($sort_field ? ' ORDER BY ' . $sort_field : '');
  $results = db_query($query);
  $rows = $results->fetchAllAssoc($key_field);

  if ($option_name) {
    array_unshift($rows, array($option_value => 'Select ' . $option_name));
  }
  return $rows;
}

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

function islandora_digital_workflow_get_sites_options() {
  $sites = islandora_digital_workflow_get_solr_options('RELS_EXT_hasModel_uri_ms:*siteCModel', 'PID', 'fgs_label_s');
  if (count($sites) < 1) {
    $sites[0] = 'No sites objects configured.';
  }
  return $sites;
}

/**
 * Helper function to look up the names of the collections based on a possible comma-separated list
 */
function islandora_digital_workflow_lookup_collection_names() {
  $rows = array();

  $query = 'SELECT * FROM collection ORDER BY `name`';

  $result = db_query($query);
  while ($row = mysqli_fetch_assoc($result)) {
    $rows[$row['id']] = $row;
  }
  return $rows;
}

/**
 * This will return the ingest path for a given batch by combining the batch's host setting
 * and the batch_external_id.  If the folder does not exist, it will be created.  It will be
 * named 'batch_{$batch_external_id}'.
 *
 * In the case when the batch is coming from the ftp to box.com, the system will use
 * the return the Default Ingest Path (configured admin/islandora/workflow) and the files
 * would the downloaded and extracted to this path from "box.com"
 *
 * Note about the folder permissions:
 *   The folder must be writable by apache as well as by other admin members of our department.
 *
 * @param array $batch_values
 * @return string
 */
function islandora_digital_workflow_batch_path($batch_values) {
  if (!is_array($batch_values) || !array_key_exists('batch_external_id', $batch_values)) {
    drupal_set_message('The batch path could not be determined.  Either the values are missing or ' .
            'there is no value for the batch "Name" (batch_external_id field) yet.', 'warning');
    return '';
  }
  $ingest_path = $batch_values['batch_host'];
  if (!$batch_values['batch_host'] || $ingest_path == 'ftp' || (ltrim($ingest_path, 'ftp:') <> $ingest_path)) {
    $ingest_path = variable_get('islandora_digital_workflow_ingest_prepared_path', '/ingest/islandora_ingest');
  } elseif (strstr($ingest_path, ":")) {
    @list($ingest_host, $ingest_path) = explode(":", $ingest_path, 2);
    drupal_set_message('This batch is configured to use files from a different server.  That server needs ' .
            'to be configured as a mounted folder under the /ingest directory and the ' . l('Available Host Alias', '/admin/islandora/workflow/') .
            ' configuration would need to be updated to point to the mounted location.', 'error');
  } else {
    $ingest_path = $ingest_path;
  }
  $batch_path = rtrim($ingest_path, '/') . '/batch_' . $batch_values['batch_external_id'];
  if (!file_exists($batch_path)) {
    $call = mkdir($batch_path);
    if (!$call) {
      drupal_set_message('The batch folder "' . $batch_path . '" could not be created.', 'error');
    }
  }

  return (file_exists($batch_path)) ? $batch_path : '';
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
 * @return array('header' => $file_header, 'rows' => $file_rows)
 * @throws Exception
 */
function islandora_digital_workflow_csv_file_as_array($csv_file) {
  $file_rows = $file_header = array();
  if (file_exists($csv_file)) {
    $row = 0;
    if (($handle = fopen($csv_file, "r")) !== FALSE) {
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
 * Will handle the CSV values for dates and create a "display date" and a "sort date" value as best as it can assuming:
 *      CSV value		Display date				Sort date
 * -------------------------------------------------------------------------------------------------
 *	1945/1955		1945-1955				1945-01-01T00:00:00-05:00
 * a	1945/1955 		ca. 1945-1955				1945-01-01T00:00:00-05:00
 *	1945-03/1955-04 	March 1945- April 1955			1945-03-01T00:00:00-05:00
 * a	1945-03/1955-04 	ca. March 1945- April 1955		1945-03-01T00:00:00-05:00
 * 	1945-03-05/1955-04-23	March 5, 1945- April 23, 1955		1945-03-05T00:00:00-05:00
 * a	1945-03-05/1955-04-23 	ca. March 5, 1945- April 23, 1955	1945-03-05T00:00:00-05:00
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