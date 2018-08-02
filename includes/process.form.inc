<?php

/**
 * The Drupal process form representation.
 * 
 * This is to provide a form to enter identifiers of items or batch names and
 * a set of functions that can be performed.
 *
 * @return array
 *   The Drupal form definition.
 */
function islandora_digital_workflow_process_form() {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/lookup.form');

  $lookup_form_markup = theme('islandora_digital_workflow_lookup', array('searchterm' => ''));

  $breadcrumb = array(
    l(t('Home'), '<front>'),
    l(t('Islandora Digital Workflow'), 'islandora/islandora_digital_workflow'),
  );
  drupal_set_breadcrumb($breadcrumb);

  $process_form = drupal_get_form('islandora_digital_workflow_get_process_form');
  return $lookup_form_markup . drupal_render($process_form);
}

function islandora_digital_workflow_get_process_form($form, &$form_state) {
  $mode_options = array(
    'something' => 'Do something',
    'somethingelse' => 'Something else',
    'diff' => 'And now for something completely different',
  );
  $form = array(
      'identifiers' => array(
          '#type' => 'textarea',
          '#title' => 'Identifiers',
          '#description' => 'Enter item identifiers or batch names. Each barcode should be on a separate line.',
          '#default_value' => 'default value',
      ),

      'mode' => array(
          '#type' => 'select',
          '#title' => 'Processing mode',
          '#description' => 'Depending on what needs to be done, select the specific processing mode operation.',
          '#options' => $mode_options,
          '#default_value' => '',
      ),

      'submit' => array(
          '#type' => 'submit',
          '#value' => t('Submit'),
      ),
  );
  return $form;
}