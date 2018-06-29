<?php

/**
 * Displays the form to either add / remove transactions from a batch.
 *
 * This approache is needed in order to display two separate forms on a single
 * page.  The lookup form's action needs to be handled separately from the
 * transaction_form.  Even though the node value is not passed as an argument, it
 * can be derived by using the arg() function.
 *
 * @return string
 *   HTML markup.
 */
function islandora_digital_workflow_add_transaction() {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/lookup.form');
  $lookup_form_markup = theme('islandora_digital_workflow_lookup', array('searchterm' => ''));
  $nid = arg(1);
  $batch_item_id = arg(3);
  $action_id = arg(4);

  $node = ($nid) ? node_load($nid) : NULL;
  drupal_set_title(t('Islandora Digital Workflow - Add transaction'), CHECK_PLAIN);
  $form = drupal_get_form('islandora_digital_workflow_add_transactions_form', $node, $batch_item_id, $action_id);
  $markup = $lookup_form_markup . drupal_render($form);
  return $markup;
}

/**
 * Displays transactions related to a workflow_batch node.
 *
 * @param object $node
 *   Drupal node.
 * @return string
 *   HTML markup.
 */
function islandora_digital_workflow_add_transactions_form(array $form, array &$form_state, $node, $batch_item_id, $action_id) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  $batch_name = $node->title;
  $form = array();
  $batch_record = islandora_digital_workflow_get_batch_record_by_nid($node->nid);
  $action_record = islandora_digital_workflow_get_action($action_id);
  $batch_item_object = islandora_digital_workflow_get_batch_item($batch_item_id);
  $batch_item_identifier = $batch_item_object->identifier;
  $action_description = $action_record['description'];
  // if the transaction's action_id is < 100, it is a per-item transaction, else
  // it is a "per-batch" transaction as in "all items scanned".
  $actions = ($action_id < 100) ?
    islandora_digital_workflow_get_batch_items_actions() :
    islandora_digital_workflow_get_batch_actions();
  $form['transaction_action'] = array(
      '#title' => t('Select Transaction'),
      '#type' => 'select',
      '#options' => $actions,
      '#attributes' => array('autocomplete' => 'off'),
      '#description' => 'Select the transaction to apply to the selected batch items',
      '#default_value' => $action_id,
  );

  $form['nid'] = array(
      '#type' => 'hidden',
      '#value' => $node->nid,
  );
  $form['batch_id'] = array(
      '#type' => 'hidden',
      '#value' => $batch_record['batch_id'],
  );
  $form['batch_item_id'] = array(
      '#type' => 'hidden',
      '#value' => $batch_item_id,
  );
  $form['transaction_action'] = array(
      '#type' => 'hidden',
      '#value' => $action_id,
  );

  $form['timestamp'] = array(
      '#type' => 'hidden',
      '#value' => date('Y/m/d'),
  );
  $form['message'] = array(
      '#type' => 'item',
      '#title' => 'Add  "' . $action_description . '" to item "' . $batch_item_identifier . '"?<br>',
      '#description' => '<b>NOTE:</b> this only updates the action and will not update the timestamp value for the transaction.',
  );
  $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Add transaction'),
  );
  $form['cancel'] = array(
      '#type' => 'submit',
      '#value' => t('Cancel'),
  );

  return $form;
}

/**
 * Add transaction submit form handler.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_digital_workflow_add_transactions_form_submit(array $form, array &$form_state) {
  $transaction_id = $form_state['values']['transaction_id'];
  $timestamp = $form_state['values']['timestamp'];
  $action_id = $form_state['values']['transaction_action'];
  $batch_item_id = $form_state['values']['batch_item_id'];
  $nid = $form_state['values']['nid'];
  // Based on the action_id, set the $goto variable to return user to the correct
  // previous page. /node/163/item/62
  $goto = ($action_id > 100) ? '/node/' . $nid . '/transactions' : 
    '/node/' . $nid . '/item/' . $batch_item_id;
  if ($form_state['triggering_element']['#value'] == t('Cancel')) {
    drupal_goto($goto);
  }
  if ($form_state['triggering_element']['#value'] == t('Update transaction')) {
    module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
    // Save the updated transaction here:
    if (islandora_digital_workflow_update_batch_transaction($transaction_id, $action_id, $timestamp)) {
      drupal_goto($goto);
    }
  }
}