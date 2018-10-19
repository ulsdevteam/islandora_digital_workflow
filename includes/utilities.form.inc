<?php

/**
 * @file
 * Form to display the utilities functions that can be performed.
 */

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

  $breadcrumb = array(
    l(t('Home'), '<front>'),
    l(t('Islandora Digital Workflow'), 'islandora/islandora_digital_workflow'),
    l(t('Utilities'), 'islandora/islandora_digital_workflow/utilities'),
  );
  drupal_set_breadcrumb($breadcrumb);
  
  $batch_name = arg(4);

  $output_contents = $output_footer = array();
  $output_contents[] = '<p>Click on batches to recalculate the items / files size.</p>';

  $batch_records = islandora_digital_workflow_get_all_batch_records();
  if ($batch_name == '') {
    $output_contents[] = '<b>' . l(t('Recalculate for ALL batches'),
      'islandora/islandora_digital_workflow/utilities/recalculate_files/*') . '</b><br>';
  }
  foreach ($batch_records as $batch_record) {
    if ($batch_name == $batch_record->batch_name || $batch_name == '' || $batch_name == '*') {
      // This function will do the actual calculations per batch -- and put any
      // relevant output for the utilities form into the two $output_* parameter 
      // values.
      if ($batch_name == '') {
        $output_contents[] = l(t('Recalculate for "' .  $batch_record->batch_name . '"'),
          'islandora/islandora_digital_workflow/utilities/recalculate_files/' . $batch_record->batch_name) . '<br>';
      } else {
        islandora_digital_workflow_recalc_batch_files($batch_record, $output_contents, $output_footer);
      }
    }
    if ($batch_name == $batch_record->batch_name || $batch_name == '*') {
      $markup_arr[] = '<b class="good">Batch "' . $batch_record->batch_name .
          '" done recalculating item records\' "File Size" and "File ' .
          'Count" values.</b><br>';
    }
  }

  $output_contents[] = '<hr>' . t('Return to ') . l('Islandora Digital Workflow - Utilities', 'islandora/islandora_digital_workflow/utilities');
  $return_form = array(
      'file_size_recalc' => array(
          '#type' => 'item',
          '#markup' => implode("\n", $markup_arr) . implode("\n", $output_contents),
      )
  );
  return $return_form;
}

