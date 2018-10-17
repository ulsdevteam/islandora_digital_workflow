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
function islandora_digital_workflow_delete_batch_items() {
  $batch_name = arg(3);
  $form = drupal_get_form('islandora_digital_workflow_delete_batch_items_form', $batch_name);
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
 * @param string $batch_item_id
 *   Either "ids_by_session" or single id value of the items that need to be
 * deleted for these records in the islandora_digital_workflow_batch_items
 * table  When ids_by_session is used, the _SESSION['ids_by_session'] value
 * should contain the batch_item_id values of the underlying items for deletion.
 * @return array
 *   The Drupal form definition.
 */
function islandora_digital_workflow_delete_batch_items_form(array $form, array &$form_state, $batch_item_id = NULL) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
  if (isset($_SESSION['ids_by_session'])) {
    drupal_set_message('ids_by_session');
    drupal_set_message('<pre>' . print_r($_SESSION['ids_by_session'], true) . '</pre>');
  }

  unset($_SESSION['ids_by_session']);
  // If this form is processing an existing batch by batch_name, set up some
  // values
  $item_record_arr = islandora_digital_workflow_get_batch_item_record($batch_item_id);
  $item_obj = array_pop($item_record_arr);
  $batch_record = islandora_digital_workflow_get_batch_record_by_batch_id($item_obj->batch_id);
  if (isset($batch_record['batch_name']) && $batch_record['batch_name']) {
    $read_only_att = array('readonly' => 'readonly', 'disabled' => 'disabled');
    $button_text = '';
    $batch_path = islandora_digital_workflow_batch_path($batch_record);
  }
  else {
    drupal_set_message(t('A batch record matching the provided batch_item_id value "@batch_item_id" does not exist and can not be deleted.',
            array('@batch_item_id' => $batch_item_id)));
  }

  $form['batch_item_id'] = array(
      '#type' => 'hidden',
      '#default_value' => $batch_item_id
  );
  $form['warning'] = array(
      '#type' => 'item',
      '#markup' => '<div class="big_font bad">' .
          t('<b>THIS CAN NOT BE UNDONE!</b><br>Are you SURE that you need to ' .
            'delete this item and any transactions?') .
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
        '#default_value' => $batch_record['batch_name'],
      ),
      'batch_description' => array(
        '#type' => 'textarea',
        '#title' => t('Batch description'),
        '#attributes' => $read_only_att,
        '#description' => t('Description of the items in this batch.'),
        '#default_value' => $batch_record['batch_description'],
      ),
    );

  $form['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#attributes' => array('class' => array(
        'bad'
      )),
    );
  $form['cancel'] = array(
      '#type' => 'submit',
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
function islandora_digital_workflow_delete_batch_items_form_submit(array $form, array &$form_state) {
  $id = $form_state['triggering_element']['#id'];
  module_load_include('inc', 'islandora_digital_workflow', 'includes/node-utilities');
  $batch_record = islandora_digital_workflow_get_batch_record_by_name($form_state['values']['batch_name']);
  switch ($id) {
    case 'edit-delete':
      // Only two possible button text values -- edit or save.
      $current_time = date('H:i:s');
      drupal_set_message(t('[@date] Batch item deleted', array(
          '@date' => $current_time,
        )));
      drupal_goto('islandora/islandora_digital_workflow');
      break;
    case 'edit-cancel':
      drupal_set_message(t('Create Batch has been cancelled.'), 'info');
      drupal_goto('islandora/islandora_digital_workflow/edit_batch/' . $batch_record['batch_id']);
      break;
  }
}
