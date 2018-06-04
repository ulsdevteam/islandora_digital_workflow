<?php

/**
 * Displays items related to a workflow_batch node.
 *
 * @param object $node
 *   Drupal node.
 * @return string
 *   HTML markup.
 */
function islandora_digital_workflow_batch_items() {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/lookup.form');
  $lookup_form_markup = theme('islandora_digital_workflow_lookup', array('searchterm' => ''));
  $nid = arg(1);
  $node = ($nid) ? node_load($nid) : NULL;
  $form = drupal_get_form('islandora_digital_workflow_batch_items_form', $node);
  $markup = $lookup_form_markup . drupal_render($form);
  return $markup;
}

function islandora_digital_workflow_batch_items_form(array $form, array &$form_state, $node) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');

  $batch_name = $node->title;
  $results = islandora_digital_workflow_find_batch_records_by_name($batch_name);
  $batch_record = array_pop($results);

  // Prepare various objects for the dynamic form generation...
  $items = islandora_digital_workflow_get_batch_items_by_nid($node->nid);
  $options = array();
  foreach ($items as $item) {
    $options[$item->identifier] = array(
      'id' => $item->identifier,
      'Transactions' => theme('islandora_digital_workflow_transactions_glyphs', array('transaction_actions' => $item->transactions)),
      'Identifier' => l($item->identifier, '/node/' . $node->nid . '/item/' . $item->batch_item_id),
      'Title' => $item->title,
      'Filename' => $item->filename_basename,

    );
  }
  $items_header = array(
    'Transactions' => 'Transactions',
    'Identifier' => 'Identifier',
    'Title' => 'Title',
    'Filename' => 'Filename',
  );
  $batch_items_actions = islandora_digital_workflow_get_batch_items_actions();
  $form = array();
  $form['form_two']['add_batch_action'] = array(
      '#title' => t('Add Transaction'),
      '#type' => 'select',
      '#options' => $batch_items_actions,
      '#description' => 'Select the transaction to apply to the selected batch items',
      '#default_value' => '',
  );
  if (user_access(ISLANDORA_DIGITAL_WORKFLOW_EDIT_DELETE_TRANSACTIONS)) {
    $form['form_two']['remove_batch_action'] = array(
        '#title' => t('Remove Transaction'),
        '#type' => 'select',
        '#options' => $batch_items_actions,
        '#description' => 'Select the transaction to apply to the selected batch items',
        '#default_value' => '',
    );
  }
  $form['form_two']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Modify transactions'),
  );
  $form['form_two']['items'] = array(
      '#type' => 'tableselect',
      '#header' => $items_header,
      '#options' => $options,
      '#empty' => 'There are no items in the batch.',
    );

  return $form;
}

/**
 * Function that updates the items' transactions with user's input.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @param object $node
 *   Drupal node.
 * @return string
 *   HTML markup.
 */
function islandora_digital_workflow_batch_items_form_submit(array $form, array &$form_state) {
  $selected_identifiers = array_filter($form_state['values']['items']);
  $add_action_id = $form_state['values']['add_batch_action'];
  $remove_action_id = $form_state['values']['remove_batch_action'];
  if ($remove_action_id == '' && $add_action_id == '') {
    drupal_set_message('No transactions were added or removed');
  }
  else {
    $could_not_be_processed = array();
    foreach ($selected_identifiers as $identifier) {
      $batch_item_id = islandora_digital_workflow_get_batch_item_record_by_identifier($identifier);
      if ($add_action_id) {
        if (!islandora_digital_workflow_insert_transactions_record($add_action_id, NULL, $batch_item_id)) {
          $could_not_be_processed[] = $identifier;
        }
      }
      else {
        if (!islandora_digital_workflow_remove_transactions_record($remove_action_id, NULL, $batch_item_id)) {
          $could_not_be_processed[] = $identifier;
        }
      }
    }
    if (count($could_not_be_processed) > 0) {
      drupal_set_message(t('Some of the selected items could not be updated. ') .
        implode(", ", $could_not_be_processed));
    } else {
      drupal_set_message(t('All selected items have been updated.'));
    }
  }
}