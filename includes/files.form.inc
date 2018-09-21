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
  $results = islandora_digital_workflow_find_batch_records_by_name($batch_name);
  $batch_record = array_pop($results);

  $file_records = islandora_digital_workflow_get_batch_files_by_nid($node->nid);
  $batch_path = islandora_digital_workflow_batch_path($batch_record);
  // Scan for files in that directory - this will mark any files that are expected,
  // but not found with "bad" class.
  $is_paged_content = islandora_digital_workflow_is_paged_object($batch_record->default_type_of_resource);
  if ($is_paged_content) {
    $found_files = islandora_digital_workflow_scandir_paged_content_files($batch_path);
  }
  else {
    $found_files = islandora_digital_workflow_scan_files($batch_path, $file_records);
  }

  // Loop through the $file_records and see which ones are missing from the file system
  foreach ($file_records as $index => $file_item) {
    $relative_folder_file = str_replace($batch_path . '/', '', $file_item->filename);
    $file_records[$index]->class = (array_key_exists($relative_folder_file, $found_files) ? '' : 'bad');
  }

  return $lookup_form_markup . theme('islandora_digital_workflow_files', array(
        'table_title' => 'Batch Files',
        'table_description' => 'Listed below are all of the files related to the batch:',
        'batch_record' => $batch_record,
        'batch_path' => $batch_path,
        'node' => $node,
        'item_file_records' => $file_records,
        'found_files' => $found_files));
}
