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
    $obatch_item_ids = $_SESSION['ids_by_session'];
    $batch_item_ids = $obatch_item_ids;
    $batch_item_id = array_pop($obatch_item_ids);
    unset($_SESSION['ids_by_session']);
  }
  elseif ($batch_item_id == 'ids_by_session') {
    $obatch_item_ids = unserialize($form_state['input']['batch_item_ids']);
    $batch_item_ids = $obatch_item_ids;
    $batch_item_id = array_pop($obatch_item_ids);
  }
  else {
    $batch_item_ids = array($batch_item_id);
  }

  // If this form is processing an existing batch by batch_name, set up some
  // values
  $item_record_arr = islandora_digital_workflow_get_batch_item_record($batch_item_id);
  $item_obj = array_pop($item_record_arr);
  $batch_record = islandora_digital_workflow_get_batch_record_by_batch_id($item_obj->batch_id);
  $read_only_att = array();
  if (isset($batch_record['batch_name']) && $batch_record['batch_name']) {
    $read_only_att = array('readonly' => 'readonly', 'disabled' => 'disabled');
  }
  else {
    drupal_set_message(t('A batch record matching the provided batch_item_id value "@batch_item_id" does not exist and can not be deleted.',
            array('@batch_item_id' => $batch_item_id)));
  }

  $batch_description_markup = theme('islandora_digital_workflow_batch_description', array(
        'batch_record' => $batch_record,
      ));

  $existing_batch_identifiers = islandora_digital_workflow_get_existing_identifiers($batch_record['batch_id']);
  $batch_item_identiers = array();
  foreach ($batch_item_ids as $id) {
    $batch_item_identiers[] = $existing_batch_identifiers[$id];
  }

  $form['batch_item_ids'] = array(
      '#type' => 'hidden',
      '#default_value' => serialize($batch_item_ids),
  );
  $form['warning'] = array(
      '#type' => 'item',
      '#markup' => '<div class="big_font bad">' .
          t('<b>THIS CAN NOT BE UNDONE!</b><br>Are you SURE that you need to ' .
            'delete these items and any related transactions?') .
      '</div>',
      '#suffix' => $batch_description_markup . theme('islandora_digital_workflow_rounded_box', array(
        'title' => '<span class="bad">Identifiers to be deleted:</span>',
        'contents' => '<ul><li>' . implode("</li><li>", $batch_item_identiers) . '</li></ul>',
      )),
    );
  $form['batch_name'] = array(
    '#type' => 'hidden',
    '#default_value' => $batch_record['batch_name'],
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
  module_load_include('inc', 'islandora_digital_workflow', 'includes/node_utilities');
  $batch_record = islandora_digital_workflow_get_batch_record_by_name($form_state['values']['batch_name']);
  switch ($id) {
    case 'edit-delete':
      $batch_item_ids = unserialize($form_state['values']['batch_item_ids']);
      foreach ($batch_item_ids as $batch_item_id) {
        islandora_digital_workflow_delete_batch_item($batch_item_id);
      }
      $current_time = date('H:i:s');
      drupal_set_message(t('[@date] Batch items deleted', array(
          '@date' => $current_time,
        )));
      drupal_goto('node/' . $batch_record['nid'] . '/items');
      break;
    case 'edit-cancel':
      drupal_set_message(t('Create Batch has been cancelled.'), 'info');
      drupal_goto('islandora/islandora_digital_workflow/edit_batch/' . $batch_record['batch_id']);
      break;
  }
  return;
}
