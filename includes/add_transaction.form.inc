<?php

/**
 * @file
 * Form to add a transaction to a batch or item.
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
  $action_description = ((is_array($action_record) && array_key_exists('action_description', $action_record)) ? $action_record['action_description'] : '');

  $breadcrumb = array(
    l(t('Home'), '<front>'),
    l(t('Islandora Digital Workflow'), 'islandora/islandora_digital_workflow'),
  );
  if ($node) {
    $breadcrumb[] = l($node->title, 'node/' . $node->nid . '/items');
    $breadcrumb[] = l(t('Transactions'), 'node/' . $node->nid . '/transactions');
  }
  drupal_set_breadcrumb($breadcrumb);

  // Display a little warning on this form if the user is trying to add an
  // action that applies to "all items" in the batch.
  if (islandora_digital_workflow_is_batch_action($action_record)) {
    drupal_set_message('Since the selected action is a "batch" action, adding it will update this items\' batch.', 'warning');
  }

  // if the transaction's action_id is a per-item transaction, else it is a
  // "per-batch" transaction as in "all items scanned".
  $actions = (islandora_digital_workflow_is_batch_action($action_record)) ?
    islandora_digital_workflow_get_batch_actions($batch_record['workflow_sequence_id']) :
    islandora_digital_workflow_get_batch_items_actions($batch_record['workflow_sequence_id']);
  $form['transaction_action'] = array(
      '#title' => t('Select Transaction'),
      '#type' => 'select',
      '#options' => $actions,
      '#attributes' => array('autocomplete' => 'off'),
      '#description' => 'Select the transaction to apply to the selected batch items',
      '#default_value' => $action_id,
  );

  $form['refer'] = array(
      '#type' => 'hidden',
      '#value' => $_SERVER['HTTP_REFERER'],
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
  if ($action_id == IDW_ACTION_PROBLEM || $action_id == IDW_ACTION_METADATA_FAIL_QC) {
    $form['problem_notes'] = array(
        '#type' => 'textarea',
        '#title' => 'Problem notes',
        '#description' => 'Enter the instructions to resolve the problem/s for the selected item/s.',
        '#rows' => 3,
    );
  }
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
  );
  $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Add transaction'),
  );
  $form['cancel'] = array(
      '#type' => 'submit',
      '#prefix' => '&nbsp; ',
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
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
  $timestamp = $form_state['values']['timestamp'];
  $action_id = $form_state['values']['transaction_action'];
  $batch_item_id = $form_state['values']['batch_item_id'];
  $batch_item_object = islandora_digital_workflow_get_batch_item($batch_item_id);
  $batch_item_identifier = $batch_item_object->identifier;
  $refer = $form_state['values']['refer'];
  $refer_url_parts = parse_url($refer);
  $action_record = islandora_digital_workflow_get_action($action_id);
  $problem_notes = ($action_id == IDW_ACTION_PROBLEM || $action_id == IDW_ACTION_METADATA_FAIL_QC) ?
      $form_state['values']['problem_notes'] : '';
  $nid = $form_state['values']['nid'];


  if ($form_state['triggering_element']['#value'] <> t('Cancel')) {
    islandora_digital_workflow_insert_transactions_record($action_id, NULL, $batch_item_id, NULL, $problem_notes);
  }
  // If inserting this transaction record triggers a Stage change, the value for
  // $goto will be wrong since $batch_item_id no longer points to this element.
  $batch_item_id = islandora_digital_workflow_get_batch_item_id_by_identifier($batch_item_identifier, NULL, FALSE);
  // Based on the action_id, set the $goto variable to return user to the correct
  // previous page. /node/163/item/62 - else can use the refer path value.
  $goto = (islandora_digital_workflow_is_batch_action($action_record)) ? $refer_url_parts['path'] :
    '/node/' . $nid . '/item/' . $batch_item_id;

  drupal_goto($goto);
}