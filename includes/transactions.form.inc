<?php

/**
 * Displays transactions related to a workflow_batch node.
 *
 * @param object $node
 *   Drupal node.
 * @return string
 *   HTML markup.
 */
function islandora_digital_workflow_transactions($node) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/lookup.form');
  $lookup_form_markup = theme('islandora_digital_workflow_lookup', array('searchterm' => ''));

  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');

  $batch_name = $node->title;
  $results = islandora_digital_workflow_find_batch_records_by_name($batch_name);
  $batch_record = array_pop($results);

  $transactions = islandora_digital_workflow_get_batch_transactions_by_nid($node->nid);
  dpm($transactions);

  return $lookup_form_markup .
      theme('islandora_digital_workflow_transactions', array(
        'table_title' => 'Transactions',
        'table_description' => 'Listed below are all of the transactions that have been tracked for the batch:',
        'batch_record' => $batch_record,
        'transactions' => $transactions));
}

