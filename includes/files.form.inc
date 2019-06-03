<?php

/**
 * @file
 * Form to display the files related to a batch item.
 */

/**
 * Displays files related to a workflow_batch node.
 *
 * @param object $node
 *   Drupal node.
 * @return string
 *   HTML markup.
 */
function islandora_digital_workflow_files($node) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/lookup.form');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
  drupal_add_js('misc/collapse.js');

  $breadcrumb = array(
    l(t('Home'), '<front>'),
    l(t('Islandora Digital Workflow'), 'islandora/islandora_digital_workflow'),
    l($node->title, 'node/' . $node->nid),
  );
  drupal_set_breadcrumb($breadcrumb);

  $lookup_form_markup = theme('islandora_digital_workflow_lookup', array('searchterm' => ''));

  $batch_name = $node->title;
  $batch_record = islandora_digital_workflow_get_batch_record_by_nid($node->nid);
  $batch_items = islandora_digital_workflow_get_batch_items_by_nid($batch_record['nid']);
  $batch_description_markup = theme('islandora_digital_workflow_batch_description', array(
    'batch_record' => $batch_record,
    'collection_review_links' => islandora_digital_workflow_collection_review_links($batch_record),
    'can_ingest_all' => islandora_digital_workflow_can_ingest_all($batch_items),
    'can_publish_all' => islandora_digital_workflow_can_publish_all($batch_items),
    'batch_mapped_from_webformsubmission' => islandora_digital_workflow_get_webformsubmission($batch_record['webform_submission_sid']),
  ));
  $batch_delivery_path = islandora_digital_workflow_batch_path($batch_record);
  $file_records = islandora_digital_workflow_get_batch_files_by_nid($node->nid, $batch_delivery_path);
  $batch_path = islandora_digital_workflow_batch_path($batch_record, 'working');
  // Scan for files in that directory - this will mark any files that are expected,
  // but not found with "bad" class.
  $is_paged_content = islandora_digital_workflow_is_paged_object($batch_record['islandora_model']);
  if (!$is_paged_content) {
    $found_files = islandora_digital_workflow_scan_files($batch_path, array(), TRUE);
    foreach ($file_records as $index => $file_item) {
      $file_records[$index]->class = '';
      $file_records[$index]->filename = 'foo?';
    }
  }
  else {
    $found_files = array();
    foreach ($file_records as $idx => $item) {
      $item_working_path = $batch_path . '/' . $item->identifier;
      $item_working_paged_object_path = $batch_path . '/' . $item->identifier . '/' . islandora_digital_workflow_object_subfolder_name($item->islandora_model, $batch_record);
      $item_files = islandora_digital_workflow_scan_files($item_working_path, array(), TRUE);
      // $item_files = islandora_digital_workflow_scandir_paged_content_files($item_working_paged_object_path, FALSE);
      foreach ($item_files as $k => $v_arr) {
        $item_files[$k]['scan_path'] = $item_working_paged_object_path;
      }
      $found_files[$item->identifier] = $item_files;
    }
    // $found_files = islandora_digital_workflow_scandir_paged_content_files($batch_path);
    // Loop through the $file_records and see which ones are missing from the file system
    foreach ($file_records as $index => $file_item) {
      $relative_folder_file = str_replace($batch_path . '/', '', $file_item->filename);
      $file_records[$index]->class = (array_key_exists($relative_folder_file, $found_files) ? '' : 'bad');
    }
  }

  return $lookup_form_markup . $batch_description_markup .
      theme('islandora_digital_workflow_files', array(
        'table_title' => 'Batch Files',
        'table_description' => 'Listed below are all of the files related to the batch:',
        'batch_record' => $batch_record,
        'batch_path' => $batch_path,
        'node' => $node,
        'is_paged_content' => $is_paged_content,
        'item_file_records' => $file_records,
        'found_files' => $found_files));
}
