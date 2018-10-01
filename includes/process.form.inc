<?php

/**
 * @file
 * Form to diplay the utility functions that can be processed.
 */

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
    l(t('Utilities'), 'islandora/islandora_digital_workflow/utilities'),
  );
  drupal_set_breadcrumb($breadcrumb);

  $process_form = drupal_get_form('islandora_digital_workflow_actual_process_form');
  return $lookup_form_markup . drupal_render($process_form);
}

/**
 * For to process mode operations on a set of identifiers or batch names.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @return array
 *   The Drupal form definition.
 */
function islandora_digital_workflow_actual_process_form($form, &$form_state) {
  if (array_key_exists('build_info', $form_state) &&
      array_key_exists('args', $form_state['build_info']) &&
      array_key_exists(0, $form_state['build_info']['args']) &&
      array_key_exists('values', $form_state['build_info']['args'][0])) {
    $form_state['values'] = $form_state['build_info']['args'][0]['values'];
    // "rebuild" will cause this to return the form rebuilt to the call from
    // upitt_workflow_process_set to drupal_get_form.
    $form_state['rebuild'] = TRUE;
  }

  $mode_options = array(
    '1' => 'Standard Processing',
    'something' => 'Something',
    '2' => 'Special Functionality',
    'somethingelse' => 'Something else',
    'blah' => 'And now for something completely different',
    '3' => 'Info / Cleanup',
    'check_ingested' => 'Check ingested',
  );
  $form = array(
      'identifiers' => array(
          '#type' => 'textarea',
          '#title' => 'Identifiers',
          '#description' => 'Enter item identifiers or batch names. Each barcode should be on a separate line.',
          '#default_value' => ((array_key_exists('values', $form_state) && array_key_exists('identifiers', $form_state['values']))   ?
            $form_state['values']['identifiers'] : ''),
      ),

      'mode' => array(
          '#type' => 'select',
          '#title' => 'Processing mode',
          '#description' => 'Depending on what needs to be done, select the specific processing mode operation.',
          '#options' => $mode_options,
          '#default_value' => ((array_key_exists('values', $form_state) && array_key_exists('mode', $form_state['values']))   ?
            $form_state['values']['mode'] : ''),
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

/**
 * Submit handler for process mode operations on a set of identifiers or batch
 * names.  This code will dynamically make a function name from the selected
 * "mode" value and pass the identifiers on to that for processing -- and
 * present the results.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @return string
 *   The Drupal form definition.
 */
function islandora_digital_workflow_actual_process_form_submit($form, &$form_state) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  $markup = '';

  $btn_id = $form_state['triggering_element']['#id'];
  $mode = $form_state['values']['mode'];
  if ($mode == '0' || $mode == '1' || $mode == '2' || $mode == '3') {
    drupal_set_message('Invalid <b>Processing mode</b> selection.');
    return;
  }

  switch ($btn_id) {
    case 'edit-submit':
      // Make a variable to set the function name for the call below:
      $fn = 'islandora_digital_workflow_process_mode__' . $mode;
      if (function_exists($fn)) {
        $identifiers_as_text = $form_state['values']['identifiers'];
        $identifiers_array = islandora_digital_workflow_post_text_to_array($form_state['values']['identifiers']);
        // If there is a function, the value for $markup can now be populated.
        $markup = $fn($identifiers_array);
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

/**
 * TESTING "check_ingested"
 *
 * @param type $identifiers_array
 */
function islandora_digital_workflow_process_mode__check_ingested(&$identifiers_array) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');

  $timestamps['start'] = date('Y-m-d H:i:s');
  $contents = islandora_digital_workflow_make_process_set('Not ingested', 'check_ingested', $identifiers_array);
  $timestamps['done'] = date('Y-m-d H:i:s');

  $markup = theme('islandora_digital_workflow_text_report_wrapper', array(
    'inner_title' => 'Check ingested',
    'contents' => $contents,
    'footer' => "<b>" . count($identifiers_array) . " objects</b><br>",
    'timestamps' => $timestamps,
  ));

  return "<br>check_ingested tested<hr>" . $markup;
}

/**
 * TESTING "something"
 *
 * @param type $identifiers_array
 */
function islandora_digital_workflow_process_mode__something(&$identifiers_array) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');

  $timestamps['start'] = date('Y-m-d H:i:s');
  $contents = islandora_digital_workflow_make_process_set('Not ingested', 'something', $identifiers_array);
  $timestamps['done'] = date('Y-m-d H:i:s');

  $markup = theme('islandora_digital_workflow_text_report_wrapper', array(
    'inner_title' => 'islandora_digital_workflow_process_mode__something',
    'contents' => $contents,
    'footer' => "<b>" . count($identifiers_array) . " objects</b><br>",
    'timestamps' => $timestamps,
  ));

  return "<br>something tested<hr>" . $markup;
}

/**
 * TESTING "somethingelse"
 *
 * @param type $identifiers_array
 */
function islandora_digital_workflow_process_mode__somethingelse(&$identifiers_array) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');

  $timestamps['start'] = date('Y-m-d H:i:s');
  $contents = islandora_digital_workflow_make_process_set('Not ingested', 'somethingelse', $identifiers_array);
  $timestamps['done'] = date('Y-m-d H:i:s');

  $markup = theme('islandora_digital_workflow_text_report_wrapper', array(
    'inner_title' => 'islandora_digital_workflow_process_mode__somethingelse',
    'contents' => $contents,
    'footer' => "<b>" . count($identifiers_array) . " objects</b><br>",
    'timestamps' => $timestamps,
  ));

  return "<br>somethingelse tested<hr>" . $markup;
}

/**
 * TESTING "blah"
 *
 * @param type $identifiers_array
 */
function islandora_digital_workflow_process_mode__blah(&$identifiers_array) {
  $timestamps['start'] = date('Y-m-d H:i:s');
  $contents = array();
  foreach ($identifiers_array as $identifier) {
    $identifier = trim($identifier);
    $contents[] = 'diff for ' . $identifier . "<br>";
  }
  $timestamps['done'] = date('Y-m-d H:i:s');

  $markup = theme('islandora_digital_workflow_text_report_wrapper', array(
    'inner_title' => 'islandora_digital_workflow_process_mode__blah',
    'contents' => implode("\n", $contents),
    'footer' => "<b>" . count($identifiers_array) . " objects</b><br>",
    'timestamps' => $timestamps,
  ));
  return "<br>blah tested<hr>" . $markup;
}

function islandora_digital_workflow_process_set($process_set_id) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');

  islandora_digital_workflow_get_process_set_records($process_set_id, $mode, $items);
  $form_state = array();
  $form_state['values'] = array(
      'mode' => $mode,
      'identifiers' => implode("\n", $items),
  );
  $form_name = 'islandora_digital_workflow_actual_process_form';
  $process_form = drupal_get_form($form_name, $form_state);

  return drupal_render($process_form);
}
