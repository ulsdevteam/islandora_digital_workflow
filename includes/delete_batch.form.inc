<?php

/**
 * @file
 * Delete batch form and submit handling for Islandora Digital Workflow module.
 */

/**
 * The Drupal delete batch form representation.
 *
 * @return array
 *   The Drupal form definition.
 */
function islandora_digital_workflow_delete_batch() {
  $batch_name = arg(3);
  $form = drupal_get_form('islandora_digital_workflow_delete_batch_form', $batch_name);
  $markup = drupal_render($form);
  return $markup;
}

/**
 * Drupal form for the delete batch operation.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @param string $batch_name
 *   The name of the batch to delete.
 * @return array
 *   The Drupal form definition.
 */
function islandora_digital_workflow_delete_batch_form(array $form, array &$form_state, $batch_name = NULL) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');

  // If this form is processing an existing batch by batch_name, set up some
  // values
  $batch_record = islandora_digital_workflow_get_batch_record_by_name($batch_name);
  if (isset($batch_record['batch_name']) && $batch_record['batch_name']) {
    $batch_name = $batch_record['batch_name'];
    $batch_description = $batch_record['batch_description'];

    $read_only_att = array('readonly' => 'readonly', 'disabled' => 'disabled');
    $button_text = '';
    $batch_path = islandora_digital_workflow_batch_path($batch_record);
  }
  else {
    drupal_set_message(t('The batch record "@batch_name" does not exist and can not be deleted.',
            array('@batch_name' => $batch_name)));
    drupal_goto('islandora/islandora_digital_workflow/');
  }

  $form['batch_name'] = array(
      '#type' => 'hidden',
      '#default_value' => $batch_name
  );
  $form['nid'] = array(
      '#type' => 'hidden',
      '#default_value' => $batch_record['nid'],
  );
  $form['warning'] = array(
      '#type' => 'item',
      '#markup' => '<div class="big_font bad">' . 
          t('<b>THIS CAN NOT BE UNDONE!</b><br>Are you SURE that you need to ' .
            'delete this batch and all of the batch items and any transactions?') .
      '</div>',
    );
  $form['frame'] = array(
      '#type' => 'fieldset',
      '#title' => 'Batch details',
      'batch_name' => array(
        '#type' => 'textfield',
        '#title' => t('Batch name'),
        '#required' => true,
        '#attributes' => $read_only_att,
        '#description' => t('Human readable name for the batch of digital objects.'),
        '#default_value' => $batch_name,
      ),
      'batch_description' => array(
        '#type' => 'textarea',
        '#title' => t('Batch description'),
        '#attributes' => $read_only_att,
        '#description' => t('Description of the items in this batch.'),
        '#default_value' => $batch_description,
      ),
    );

  $form['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
    );
  $form['cancel'] = array(
      '#type' => 'submit',
      '#prefix' => '&nbsp; ',
      '#value' => t('Cancel'),
    );
  return $form;
}

/**
 * Function that sets the Drupal variables with user's input.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_digital_workflow_delete_batch_form_submit(array $form, array &$form_state) {
  $id = $form_state['triggering_element']['#id'];
  module_load_include('inc', 'islandora_digital_workflow', 'includes/node_utilities');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/file_utilities');

  $batch_record = islandora_digital_workflow_get_batch_record_by_name($form_state['values']['batch_name']);

  switch ($id) {
    case 'edit-delete':
      $batch_name = $form_state['values']['batch_name'];
      $batch_record = islandora_digital_workflow_get_batch_record_by_name($batch_name);
      $items = islandora_digital_workflow_get_batch_items_by_nid($batch_record['nid']);
      foreach ($items as $batch_item_obj) {
        islandora_digital_workflow_delete_batch_item($batch_item_obj->batch_item_id);
      }

      $current_delivery_path = islandora_digital_workflow_batch_path($batch_record, 'delivery');
      $current_problems_path = islandora_digital_workflow_batch_path($batch_record, 'problems');
      $current_working_path = islandora_digital_workflow_batch_path($batch_record, 'working');
      // Only try to rename those item folders that exist
      if (file_exists($current_delivery_path)) {
        if (!islandora_digital_workflow_deleteDirectory($current_delivery_path) ) {
          drupal_set_message(t('There was a problem deleting the "delivery folder" ' .
              'for the batch "@ident".  Check file permissions on the ' .
              'path:<ul><li>@path</li></ul>.', array(
                  '@path' => $current_delivery_path,
                  '@ident' => $batch_record['batch_name'])), 'error');
        }
      }
      if (file_exists($current_problems_path)) {
        if (!islandora_digital_workflow_deleteDirectory($current_problems_path) ) {
          drupal_set_message(t('There was a problem deleting the "problems folder" ' .
              'for the batch "@ident".  Check file permissions on the ' .
              'path:<ul><li>@path</li></ul>.', array(
                  '@path' => $current_problems_path,
                  '@ident' => $batch_record['batch_name'])), 'error');
        }
      }
      if (file_exists($current_working_path)) {
        if (!islandora_digital_workflow_deleteDirectory($current_working_path) ) {
          drupal_set_message(t('There was a problem deleting the "delivery folder" ' .
              'for the batch "@ident".  Check file permissions on the ' .
              'path:<ul><li>@path</li></ul>.', array(
                  '@path' => $current_working_path,
                  '@ident' => $batch_record['batch_name'])), 'error');
        }
      }

      islandora_digital_workflow_delete_batch_record($batch_record['batch_id']);
      if ($batch_record['nid']) {
        $delete_node = node_delete($batch_record['nid']);
      }
      $current_time = date('H:i:s');
      drupal_set_message(t('[@date] Batch deleted', array(
          '@date' => $current_time,
        )));
      drupal_goto('islandora/islandora_digital_workflow');
      break;
    case 'edit-cancel':
      drupal_set_message(t('Create Batch has been cancelled.'), 'message_info');
      drupal_goto('islandora/islandora_digital_workflow/edit_batch/' . $batch_record['batch_id']);
      break;
  }
}

