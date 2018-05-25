<?php

/**
 * Displays items related to a workflow_batch node.
 *
 * @param object $node
 *   Drupal node.
 * @return string
 *   HTML markup.
 */
function islandora_digital_workflow_items($node) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');

  $batch_name = $node->title;
  $results = islandora_digital_workflow_find_batch_records_by_name($batch_name);
  $batch_record = array_pop($results);

  $items = islandora_digital_workflow_get_batch_items_by_nid($node->nid);

  return theme('islandora_digital_workflow_items', array(
        'table_title' => 'Batch Items',
        'table_description' => 'Listed below are all of the items related to the batch:',
        'batch_record' => $batch_record,
        'node' => $node,
        'item_records' => $items));
}
