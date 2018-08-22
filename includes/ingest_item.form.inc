<?php

/**
 * @file
 * Ingest Item form and submit handling for Islandora Digital Workflow module.
 */

/**
 * Function that sets the Drupal variables with user's input.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @param string $identifier
 *   The item identifier value.
 */
function islandora_digital_workflow_ingest_item(array $form, array &$form_state, $identifier) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');

  $identifier = urldecode($identifier);
  drupal_set_message('Ingest Item "' . $identifier . '"');
  
}

