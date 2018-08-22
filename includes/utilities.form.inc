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

  $process_form = drupal_get_form('islandora_digital_workflow_actual_utilities_form');
  return $lookup_form_markup . drupal_render($process_form);
}

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
      'submit' => array(
          '#type' => 'submit',
          '#value' => t('Submit'),
      ),
  );
  if(isset($form_state['storage'], $form_state['storage']['process_results'])) {
    $form['myowntext_display'] = array(
      '#markup' => $form_state['storage']['process_results'],
      '#prefix' => '',
      '#suffix' => '',
    );
  }
  return $form;
}

function islandora_digital_workflow_actual_utilities_form_submit($form, &$form_state) {
  $markup = '';

  $btn_id = $form_state['triggering_element']['#id'];
  $mode = $form_state['values']['mode'];
  if ($mode == '0' || $mode == '1' || $mode == '2' || $mode == '3') {
    drupal_set_message('Invalid <b>Processing mode</b> selection.');
    return;
  }

  switch ($btn_id) {
    case 'edit-submit':
      $fn = 'islandora_digital_workflow_process_mode__' . $mode;
      if (function_exists($fn)) {
        $markup = $fn($form_state['values']['barcodes']);
      }
      else {
        drupal_set_message('That processing mode function "' . $fn . '" does not exist.', 'warning');
      }
      break;

    default:
      drupal_set_message('Triggering element "' . $btn_id . '" is not supported.');
      break;
  }
}

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

  $return_form = array(
      'file_size_recalc' => array(
          '#type' => 'item',
          '#markup' => implode("\n", $markup_arr),
      )
  );
  return $return_form;
}