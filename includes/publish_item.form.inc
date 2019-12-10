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
    islandora_digital_workflow_publish_object($batch_item_id);
  }
  catch (Exception $ex) {
    $had_exception = TRUE;
  }
  return TRUE;
}
