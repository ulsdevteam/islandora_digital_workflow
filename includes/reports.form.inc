<?php

/**
 * The Drupal reports form representation.
 * 
 * This is to present the user with a list of possible reports on the batches
 * and items.
 *
 * @param object $node
 *   Drupal node object.
 * @return array
 *   The Drupal form definition.
 */
function islandora_digital_workflow_reports() {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/lookup.form');

  $lookup_form_markup = theme('islandora_digital_workflow_lookup', array('searchterm' => ''));

  $breadcrumb = array(
    l(t('Home'), '<front>'),
    l(t('Islandora Digital Workflow'), 'islandora/islandora_digital_workflow'),
  );
  drupal_set_breadcrumb($breadcrumb);

  $reports_form = drupal_get_form('islandora_digital_workflow_get_reports_form');
  return $lookup_form_markup . drupal_render($reports_form);
}

function islandora_digital_workflow_get_reports_form($form, &$form_state) {
  $form = array(
      'input1' => array(
          '#type' => 'textfield',
          '#title' => 'Input 1',
          '#description' => 'foobar',
          '#default_value' => 'default value',
      ),
      'delete' => array(
          '#type' => 'submit',
          '#value' => t('Load Report'),
      ),
  );
  return $form;
}