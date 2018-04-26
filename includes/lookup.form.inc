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
  // does not come back, the searchterm may relate to a barcode that is associated
  // with a batch.
  $batch_record = islandora_digital_workflow_get_batch_record_by_name($searchterm);
  if (is_array($batch_record)) {
    drupal_set_message(t('Located batch "') . $searchterm . '"');
    drupal_goto('islandora/islandora_digital_workflow/edit_batch/' . $searchterm);
  }
  $batch_record = islandora_digital_workflow_get_batch_record_by_items_identifier($searchterm);
  if (is_array($batch_record)) {
    drupal_set_message(t('Located batch "') . $batch_record['batch_name'] . '" ' .
        t('by an item that matched identifier value "') . $searchterm . '".');
    drupal_goto('islandora/islandora_digital_workflow/edit_batch/' . $batch_record['batch_name']);
  }
  else {
    drupal_set_message('"' . $searchterm . '" ' . t('could not be located.'));
    drupal_goto('islandora/islandora_digital_workflow/');
  }

}
