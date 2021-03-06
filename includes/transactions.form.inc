<?php

/**
 * @file
 * Form to display the transactions related to a batch's workflow node.
 */

/**
 * Displays transactions for a batch and a form to either add / remove them
 * from a batch.
 *
 * This approache is needed in order to display two separate forms on a single
 * page.  The lookup form's action needs to be handled separately from the
 * transaction_form.  Even though the node value is not passed as an argument, it
 * can be derived by using the arg() function.
 *
 * @return string
 *   HTML markup.
 */
function islandora_digital_workflow_batch_transactions() {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/lookup.form');
  $lookup_form_markup = theme('islandora_digital_workflow_lookup', array('searchterm' => ''));
  $nid = arg(1);
  $node = ($nid) ? node_load($nid) : NULL;
  $breadcrumb = array(
    l(t('Home'), '<front>'),
    l(t('Islandora Digital Workflow'), 'islandora/islandora_digital_workflow'),
  );
  if ($nid) {
    $breadcrumb[] = l($node->title, 'node/' . $nid . '/items');
  }
  drupal_set_breadcrumb($breadcrumb);

  $form = drupal_get_form('islandora_digital_workflow_batch_transactions_form', $node);
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
function islandora_digital_workflow_batch_transactions_form(array $form, array &$form_state, $node) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');

  $batch_name = $node->title;
  $batch_record = islandora_digital_workflow_get_batch_record_by_nid($node->nid);
  $transactions = islandora_digital_workflow_get_batch_transactions_by_nid($node->nid);

  $tr = theme('islandora_digital_workflow_transactions', array(
        'table_title' => 'Transactions',
        'table_description' => 'Listed below are all of the transactions that have been tracked for the batch:',
        'batch_record' => $batch_record,
        'transaction_records' => $transactions));

  $batch_items_actions = islandora_digital_workflow_get_batch_actions();
  $form = array();
  $batch_items = islandora_digital_workflow_get_batch_items_by_nid($batch_record['nid']);
  $batch_description_markup = theme('islandora_digital_workflow_batch_description', array(
    'batch_record' => $batch_record,
    'collection_review_links' => islandora_digital_workflow_collection_review_links($batch_record),
    'can_ingest_all' => islandora_digital_workflow_can_ingest_all($batch_items),
    'can_publish_all' => islandora_digital_workflow_can_publish_all($batch_items),
    'batch_mapped_from_webformsubmission' => islandora_digital_workflow_get_webformsubmission($batch_record['webform_submission_sid']),
  ));

  $form['batch_id'] = array(
      '#type' => 'hidden',
      '#value' => $batch_record['batch_id'],
      '#prefix' => $batch_description_markup,
  );
  $form['info'] = array(
      '#type' => 'item',
      '#markup' => theme('islandora_digital_workflow_messages_div', array(
        'message_type' => 'message_info',
        'message' => 'The transactions for each batch item are managed from ' .
          l('Batch Items', 'node/' . $node->nid . '/items'),
    ))
  );
  $form['add_batch_action'] = array(
      '#title' => t('Add Transaction'),
      '#type' => 'select',
      '#options' => $batch_items_actions,
      '#description' => 'Select the transaction to add to the entire batch.',
      '#default_value' => '',
  );
  if (user_access(ISLANDORA_DIGITAL_WORKFLOW_EDIT_DELETE_TRANSACTIONS)) {
    $form['remove_batch_action'] = array(
        '#title' => t('Remove Transaction'),
        '#type' => 'select',
        '#options' => $batch_items_actions,
        '#description' => 'Select the transaction to remove from the batch.',
        '#default_value' => '',
    );
  }
  $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Modify transactions'),
  );
  $form['transactions'] = array(
      '#type' => 'item',
      '#markup' => $tr,
    );

  return $form;
}

/**
 * The batch_transactions form submit handler.
 *
 * Function that updates a batch transaction with user's input.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_digital_workflow_batch_transactions_form_submit(array $form, array &$form_state) {
  $batch_items_actions = islandora_digital_workflow_get_batch_actions();
  $batch_id = $form_state['values']['batch_id'];
  $add_action_id = $form_state['values']['add_batch_action'];
  $remove_action_id = $form_state['values']['remove_batch_action'];
  if ($remove_action_id == '' && $add_action_id == '') {
    drupal_set_message(t('No transactions were added or removed.'));
  }
  else {
    if ($add_action_id) {
      $action_name = $batch_items_actions[$add_action_id];
      if (!(islandora_digital_workflow_insert_transactions_record($add_action_id, $batch_id, NULL))) {
        drupal_set_message(t('The transaction "@action_name" could not be added.', array(
            '@action_name' => $action_name
        )), 'error');
      }
      else {
        drupal_set_message(t('The transaction "@action_name" has been added.', array(
            '@action_name' => $action_name
        )));
      }
    }
    if ($remove_action_id) {
      $action_name = $batch_items_actions[$remove_action_id];
      if (!(islandora_digital_workflow_remove_transactions_record($remove_action_id, $batch_id, NULL))) {
        drupal_set_message(t('The transaction "@action_name" could not be removed.', array(
            '@action_name' => $action_name
        )), 'error');
      }
      else {
        drupal_set_message(t('The transaction "@action_name" has been removed.', array(
            '@action_name' => $action_name
        )));
      }
    }
  }
}
