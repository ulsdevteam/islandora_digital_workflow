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

  $process_form = drupal_get_form('islandora_digital_workflow_actual_process_form');
  return $lookup_form_markup . drupal_render($process_form);
}

function islandora_digital_workflow_actual_process_form($form, &$form_state) {
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
  if(isset($form_state['storage'], $form_state['storage']['process_results'])) {
    $form['myowntext_display'] = array(
      '#markup' => $form_state['storage']['process_results'],
      '#prefix' => '',
      '#suffix' => '',
    );
  }
  return $form;
}

function islandora_digital_workflow_actual_process_form_submit($form, &$form_state) {
  $markup = '';

  $btn_id = $form_state['triggering_element']['#id'];
  $mode = $form_state['values']['mode'];
  if ($mode == '0' || $mode == '1' || $mode == '2' || $mode == '3') {
    drupal_set_message('Invalid <b>Processing mode</b> selection.');
    return;
  }

  switch ($btn_id) {
    case 'edit-submit':
      $fn = 'islandora_digital_workflow_process_mode__' . $mode;
      if (function_exists($fn)) {
        $markup = $fn($form_state['values']['barcodes']);
      }
      else {
        drupal_set_message('That processing mode function "' . $fn . '" does not exist.', 'warning');
      }
      break;

    default:
      drupal_set_message('Triggering element "' . $btn_id . '" is not supported.');
      break;
  }
  if ($markup) {
    $form_state['rebuild'] = TRUE;
    $form_state['storage']['process_results'] = theme('islandora_digital_workflow_simple_markup', array('markup' => $markup));
  }
}

function islandora_digital_workflow_process_mode__something(&$identifiers) {
  
}

function islandora_digital_workflow_process_mode__somethingelse(&$identifiers) {

}

function islandora_digital_workflow_process_mode__diff(&$identifiers) {
  $identifiers_arr = explode("\n", $identifiers);
  $markup = '<div class="text-report">';
  $markup .= "<b>Start " . date('Y-m-d H:i:s') . "</b><br>";

  foreach ($identifiers_arr as $identifier) {
    $identifier = trim($identifier);
    $markup .= 'diff for ' . $identifier . "<br>";
  }
  $markup .= "<b>Done " . date('Y-m-d H:i:s') . "</b><br>";
  $markup .= "<b>" . count($identifiers_arr) . " objects processed</b><br>";
  $markup .= '</div>';
  return $markup;
}

