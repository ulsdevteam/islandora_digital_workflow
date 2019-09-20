<?php

/**
 * @file
 * Form to handle the node/{nid}/batch handler to display a batch record.
 */

/**
 * The Drupal batch form representation.
 * 
 * This is for use after the batch record has been created and should display on 
 * the "Batch" tab for the node view if the user has access.
 *
 * @param object $node
 *   Drupal node object.
 * @return array
 *   The Drupal form definition.
 */
function islandora_digital_workflow_batch($node) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/lookup.form');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/create_batch.form');

  $lookup_form_markup = theme('islandora_digital_workflow_lookup', array('searchterm' => ''));

  $breadcrumb = array(
    l(t('Home'), '<front>'),
    l(t('Islandora Digital Workflow'), 'islandora/islandora_digital_workflow'),
    l($node->title, 'node/' . $node->nid . '/items'),
  );
  drupal_set_breadcrumb($breadcrumb);

  $batch_name = $node->title;
  $batch_form = drupal_get_form('islandora_digital_workflow_create_batch_form', $batch_name);
  return $lookup_form_markup . drupal_render($batch_form);
}