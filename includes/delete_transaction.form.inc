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
function islandora_digital_workflow_delete_transaction() {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/lookup.form');
  $lookup_form_markup = theme('islandora_digital_workflow_lookup', array('searchterm' => ''));
  $nid = arg(1);
  $transaction_id = arg(3);
  $node = ($nid) ? node_load($nid) : NULL;
  drupal_set_title(t('Islandora Digital Workflow - Delete transaction'), CHECK_PLAIN);
  $form = drupal_get_form('islandora_digital_workflow_delete_transactions_form', $node, $transaction_id);
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
function islandora_digital_workflow_delete_transactions_form(array $form, array &$form_state, $node, $transaction_id) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  $batch_name = $node->title;
  $transaction = islandora_digital_workflow_get_batch_transaction($transaction_id);
  $form = array();

  $form['nid'] = array(
      '#type' => 'hidden',
      '#value' => $node->nid,
  );
  $form['batch_id'] = array(
      '#type' => 'hidden',
      '#value' => $transaction->batch_id,
  );
  $form['batch_item_id'] = array(
      '#type' => 'hidden',
      '#value' => $transaction->batch_item_id,
  );

  $form['transaction_id'] = array(
      '#type' => 'hidden',
      '#value' => $transaction_id,
  );
  $form['timestamp'] = array(
      '#type' => 'hidden',
      '#value' => $transaction->timestamp,
  );
  $form['message'] = array(
      '#type' => 'item',
      '#title' => 'Delete  "' . $transaction->description . '" (' .
        $transaction->how_long_ago . ') from batch "' . $batch_name . '"?<br>',
      '#description' => '<b>WARNING:</b> this can not be undone.',
  );
  $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Delete this transaction'),
  );
  $form['cancel'] = array(
      '#type' => 'submit',
      '#value' => t('Cancel'),
  );

  return $form;
}

/**
 * Delete transaction submit form handler.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_digital_workflow_delete_transactions_form_submit(array $form, array &$form_state) {
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
  if ($form_state['triggering_element']['#value'] == t('Delete this transaction')) {
    module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
    if (islandora_digital_workflow_delete_batch_transaction($transaction_id)) {
      drupal_set_message('Transaction has been deleted.');
      drupal_goto($goto);
    }
  }
}