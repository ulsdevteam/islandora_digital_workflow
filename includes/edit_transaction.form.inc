<?php

/**
 * @file
 * Form to allow editing of a transaction.
 */

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
function islandora_digital_workflow_edit_transaction() {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/lookup.form');
  $lookup_form_markup = theme('islandora_digital_workflow_lookup', array('searchterm' => ''));
  $nid = arg(1);
  $transaction_id = arg(3);
  $node = ($nid) ? node_load($nid) : NULL;
  drupal_set_title(t('Islandora Digital Workflow - Edit transaction'), CHECK_PLAIN);
  $form = drupal_get_form('islandora_digital_workflow_edit_transactions_form', $node, $transaction_id);
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
function islandora_digital_workflow_edit_transactions_form(array $form, array &$form_state, $node, $transaction_id) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  $batch_name = $node->title;
  $transaction = islandora_digital_workflow_get_batch_transaction($transaction_id);
  $form = array();

  $breadcrumb = array(
    l(t('Home'), '<front>'),
    l(t('Islandora Digital Workflow'), 'islandora/islandora_digital_workflow'),
  );
  if ($node) {
    $breadcrumb[] = l($node->title, 'node/' . $node->nid);
  }
  if ($transaction_id && $node) {
    $breadcrumb[] = l(t('Transactions'), 'node/' . $node->nid . '/transactions');
  }
  drupal_set_breadcrumb($breadcrumb);
  $batch_record = islandora_digital_workflow_get_batch_record_by_nid($node->nid);
  // if the transaction's action_id is a per-item transaction, else it is a
  // "per-batch" transaction as in "all items scanned".
  $action_record = islandora_digital_workflow_get_action($transaction->action_id);
  $actions = (islandora_digital_workflow_is_batch_action($action_record)) ?
    islandora_digital_workflow_get_batch_actions($batch_record['workflow_sequence_id']) :
    islandora_digital_workflow_get_batch_items_actions($batch_record['workflow_sequence_id']);

  $form['transaction_action'] = array(
      '#title' => t('Select Transaction'),
      '#type' => 'select',
      '#options' => $actions,
      '#attributes' => array('autocomplete' => 'off'),
      '#description' => 'Select the transaction to apply to the selected batch items',
      '#default_value' => $transaction->action_id,
      '#attributes' => array(
        'onchange' => 'add_action_selection(this)',
      ),
  );
  $form['wrapper']['notes_wrapper'] = array(
      '#type' => 'fieldset',
      '#title' => 'Problem notes',
      '#attributes' => array('class' => array('display_none')),
      'problem_notes' => array(
          '#type' => 'textarea',
          '#description' => 'Enter the instructions to resolve the problem/s for the selected item/s.',
          '#rows' => 3,
          '#default_value' => '',
      ),
  );
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
      '#title' => 'Edit  "' . $transaction->description . '" (' .
        $transaction->how_long_ago . ') from batch "' . $batch_name . '"?<br>',
      '#description' => '<b>NOTE:</b> this only updates the action and will not update the timestamp value for the transaction.',
  );
  $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Update transaction'),
  );
  $form['cancel'] = array(
      '#type' => 'submit',
      '#prefix' => '&nbsp; ',
      '#value' => t('Cancel'),
  );

  return $form;
}

/**
 * Edit transaction submit form handler.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_digital_workflow_edit_transactions_form_submit(array $form, array &$form_state) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
  $transaction_id = $form_state['values']['transaction_id'];
  $timestamp = $form_state['values']['timestamp'];
  $action_id = $form_state['values']['transaction_action'];
  $batch_item_id = $form_state['values']['batch_item_id'];
  $problem_notes = $form_state['values']['problem_notes'];
  $nid = $form_state['values']['nid'];
  $action_record = islandora_digital_workflow_get_action($action_id);
  // Based on the action_id, set the $goto variable to return user to the correct
  // previous page. /node/163/item/62
  $goto = (islandora_digital_workflow_is_batch_action($action_record)) ? '/node/' . $nid . '/transactions' :
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