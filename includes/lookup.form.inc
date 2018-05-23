<?php

/**
 * @file
 * Create batch form and submit handling for Islandora Digital Workflow module.
 */

/**
 * Handler for menu route "islandora/islandora_digital_workflow/lookup".  This
 * simply redirects to the corrislandora_digital_workflow_lookup_form_submitect handler without the "q=" part.
 */
function islandora_digital_workflow_lookup() {
  $q = isset($_POST['q']) ? $_POST['q'] : NULL;
  if ($q) {
    drupal_goto('/islandora/islandora_digital_workflow/lookup/' . $q);
  }
}

/**
 * Function that sets the Drupal variables with user's input.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_digital_workflow_lookup_form_submit($searchterm = '') {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  // Try to load a islandora_digital_workflow_batch record by name ... if it 
  // does not come back, the searchterm may relate to an identifier that is
  // associated with a batch.
  $matched_csv_only = FALSE;
  $batch_records = islandora_digital_workflow_find_batch_records_by_name($searchterm);
  if (!is_array($batch_records) || count($batch_records) < 1) {
    $batch_records = islandora_digital_workflow_get_batch_record_by_items_identifier($searchterm);
  }
  if (!is_array($batch_records) || count($batch_records) < 1) {
    $batch_records = islandora_digital_workflow_search_batch_csv_by_items_identifier($searchterm);
    $matched_csv_only = TRUE;
  }
  if (is_array($batch_records)) {
    // If there is only one result, jump to that page --- else, present the
    // results as a list that can be used to jump to each.
    if (count($batch_records) < 2) {
      drupal_set_message(t('Located batch "') . $batch_records[0]->batch_name . '" ' .
          t('by an item that matched identifier value "') . $searchterm . '".');
      drupal_goto('islandora/islandora_digital_workflow/edit_batch/' . $batch_records[0]->batch_name);
    }
    else {
      module_load_include('inc', 'islandora_digital_workflow', 'includes/lookup.form');
      $markup = theme('islandora_digital_workflow_lookup', array('searchterm' => ''));

      return $markup . islandora_digital_workflow_highlight(theme('islandora_digital_workflow_lookup_results', array(
          'batch_records' => $batch_records,
          'results_count' => count($batch_records),
          'searchterm' => $searchterm,
          'matched_csv_only' => $matched_csv_only,
        )
      ), $searchterm);
    }
  }
  else {
    drupal_set_message('"' . $searchterm . '" ' . t('could not be located.'));
    drupal_goto('islandora/islandora_digital_workflow/');
  }

}

