<?php

/**
 * The Drupal process form representation.
 * 
 * This is to provide a form to enter identifiers of items or batch names and
 * a set of functions that can be performed.
 *
 * @return array
 *   The Drupal form definition.
 */
function islandora_digital_workflow_utilities_form() {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/lookup.form');
  $lookup_form_markup = theme('islandora_digital_workflow_lookup', array('searchterm' => ''));

  $breadcrumb = array(
    l(t('Home'), '<front>'),
    l(t('Islandora Digital Workflow'), 'islandora/islandora_digital_workflow'),
  );
  drupal_set_breadcrumb($breadcrumb);

  $utility_links_form = drupal_get_form('islandora_digital_workflow_actual_utilities_form');
  return $lookup_form_markup . drupal_render($utility_links_form);
}

/**
 * Form to display links to various Utilties functions.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @return array
 *   Drupal form definition.
 */
function islandora_digital_workflow_actual_utilities_form($form, &$form_state) {
  // display a simple set of links and descriptions that will perform general
  // utility functions on batches and items.
  $links = array();
  $links[] = '<p>' . l('Process Identifiers form', '/islandora/islandora_digital_workflow/utilities/process_form') .
      '<br><span class="small_lt_text">- form to process various functions on batches or items.</span></p>';
  $links[] = '<p>' . l('Recalculate all batch values from file system', '/islandora/islandora_digital_workflow/utilities/recalculate_files') .
      '<br><span class="small_lt_text">- form recalculate the "file_count" and "file_size" value for all batch records.<br></span></p>';
  $form = array(
      'links' => array(
          '#type' => 'item',
          '#title' => 'Utilities',
          '#markup' => "<ul><li>" . implode("</li><li>", $links) . "</li></ul>",
      ),
  );
  return $form;
}

/**
 * Helper function that will:
 *   1. scan all batch item folders
 *   2. send all results to islandora_digital_workflow_update_item_file_data
 * so that the records can be updated.
 *
 * @return array
 *   Drupal form definition.
 */
function islandora_digital_workflow_recalculate_files() {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');

  $markup_arr = array();
  $empty_size_count_arr = array('size' => 0, 'count' => 0, 'files' => array());

  $batch_records = islandora_digital_workflow_get_all_batch_records();
  foreach ($batch_records as $batch_record) {
    $batch_path = islandora_digital_workflow_batch_path($batch_record);
    $batch_folders = islandora_digital_workflow_find_folders_files($batch_path);
    $item_data = array();
    foreach ($batch_folders as $item_identifier => $found_folders_files) {
      foreach ($found_folders_files['filesize_date_files_raw'] as $filesize_date_files_raw) {
        $record_identifier = $batch_record->ingest_namespace . ":" . $item_identifier;
        $item_data[$record_identifier][] = $filesize_date_files_raw;
      }
    }
    $markup_arr[] = islandora_digital_workflow_update_item_file_data($batch_record, $item_data);
  }

  $markup_arr[] = '<h3 class="good">FINISHED recalculating the item records\' "File Size" and "File Count" values.</h3>';
  $markup_arr[] = t('Return to ') . l('Islandora Digital Workflow - Utilities', 'islandora/islandora_digital_workflow/utilities');
  $return_form = array(
      'file_size_recalc' => array(
          '#type' => 'item',
          '#markup' => implode("\n", $markup_arr),
      )
  );
  return $return_form;
}

