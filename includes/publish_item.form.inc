<?php

/**
 * @file
 * Include file for publish related functions.
 */

/**
 * Function to publish an entire batch of items.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @param type $batch_id
 *   The batch_id identifier of a workflow batch record.
 * @return array
 *   The Drupal form definition.
 */
function islandora_digital_workflow_publish_all_batch(array $form, array &$form_state, $batch_id) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');

  $success = TRUE;
  $batch_record = islandora_digital_workflow_get_batch_record_by_batch_id($batch_id);
  $items = islandora_digital_workflow_get_batch_items_by_nid($batch_record['nid']);
  if (count($items) > 0) {
    foreach ($items as $item) {
      $success |= islandora_digital_workflow_do_publish_item($item->batch_item_id, $batch_record);
    }
  }
  return $success;
}

/**
 * The user-form definition for approving / cancelling the publish of an item.
 * 
 * @todo check completedness of this item and only display the confirm dialog
 *       if the item is ready for publish.
 * 
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @param integer $batch_item_id
 *   The batch_item_id identifier of an item's record.
 * @return array
 *   The Drupal form definition.
 */
function islandora_digital_workflow_publish_item(array $form, array &$form_state, $batch_item_id) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');

  // Get the batch item record --
  $batch_item_arr = islandora_digital_workflow_get_batch_item_record($batch_item_id);
  $batch_item_object = array_pop($batch_item_arr);

  $form = array(
    'confirm-message' => array(
      '#type' => 'item',
      '#markup' => '<b>Clicking this button will publish this item.  ' .
          'Are you sure that you want to publish "' . $batch_item_object->identifier .
          '" now?</b><br>',
    ),
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Ok'),
    ),
    'cancel' => array(
      '#type' => 'submit',
      '#prefix' => '&nbsp; ',
      '#value' => t('Cancel'),
    ),
    'batch_item_id' => array(
        '#type' => 'hidden',
        '#default_value' => $batch_item_id,
    )
  );
  return $form;
}

/**
 * The submit form handler for approving / cancelling the publish of an item.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_digital_workflow_publish_item_submit(array $form, array &$form_state) {
  $id = $form_state['triggering_element']['#id'];
  $batch_item_id = $form_state['values']['batch_item_id'];
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
  $batch_item_arr = islandora_digital_workflow_get_batch_item_record($batch_item_id);
  $batch_item_object = array_pop($batch_item_arr);

  switch ($id) {
    case 'edit-submit':
      islandora_digital_workflow_do_publish_item($batch_item_id);
      drupal_set_message(t('Publish Item submitted for @batch_identifier.', array(
          '@batch_identifier' => $batch_item_object->identifier,
      )), 'status');
      break;
    case 'edit-cancel':
      drupal_set_message(t('Publish Item has been cancelled for @batch_identifier.', array(
          '@batch_identifier' => $batch_item_object->identifier,
      )), 'message_info');
      break;
  }
  // For the redirect, load the batch record in order to get the node's nid value.
  $batch_record = islandora_digital_workflow_get_batch_record_by_batch_id($batch_item_object->batch_id);
  drupal_goto('node/' . $batch_record['nid'] . '/item/' . $batch_item_id);
}

/**
 * This will publish a single batch item.
 *
 * @global object $user
 *   Drupal user object.
 * @param type $batch_item_id
 *   A batch item identifier.
 * @param type $batch_record
 *   A batch record as an array.
 *
 * @return boolean
 *   Success / failure of the prepare call.
 */
function islandora_digital_workflow_do_publish_item($batch_item_id, $batch_record = NULL) {
  global $user;
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');

  // Get the batch item record -- see what type of object that is
  $batch_item_arr = islandora_digital_workflow_get_batch_item_record($batch_item_id);
  $batch_item_object = array_pop($batch_item_arr);
  if (is_null($batch_record)) {
    $batch_record = islandora_digital_workflow_get_batch_record_by_batch_id($batch_item_object->batch_id);
  }

  $output = array();
  $return_val = 1;
  $publish_set_fields = NULL;
  try {
    $had_exception = FALSE;
  
    $ingest_namespace = islandora_digital_workflow_get_ingest_namespace($batch_record);
    $ingest_namespace_nocolon = rtrim($ingest_namespace, ':');
    $item_assign_pid = (isset($batch_item_object->assigned_pid) && $batch_item_object->assigned_pid <> '') ? $batch_item_object->assigned_pid :
            $ingest_namespace_nocolon . ':' . trim($batch_item_object->identifier);
  }
  catch (Exception $ex) {
    $had_exception = TRUE;
  }
  return TRUE;
}

/**
 * Helper function to lookup and return the correponding "batch_ingest" module
 * for a content model.
 *
 * @param string $fedora_model
 *   The name of a fedora model.
 * @return string
 *   When a model match is found, the name of the drush prepare command for the
 * correponding "batch_ingest" module for that content model.
 */
function islandora_digital_workflow_get_preprocessor($fedora_model) {
  // perhaps want to use this function call  islandora_digital_workflow_model_ingest_module_info
  $mapping = array(
      'islandora:sp_basic_image' => 'islandora_batch_preprocess',
      'islandora:sp_large_image_cmodel' => 'islandora_batch_preprocess',
      'islandora:findingAidCModel' => 'islandora_newspaper_batch_preprocess',
      'islandora:bookCModel' => 'islandora_book_batch_preprocess',
      'islandora:manuscriptCModel' => 'islandora_manuscript_batch_preprocess',
      'islandora:newspaperCModel ' => 'islandora_newspaper_batch_preprocess',
      'islandora:newspaperIssueCModel' => 'islandora_newspaper_batch_preprocess',
      'islandora:sp_web_archive' => 'islandora_newspaper_batch_preprocess',
  );
  return (array_key_exists($fedora_model, $mapping) ? $mapping[$fedora_model] :
    '');
}
