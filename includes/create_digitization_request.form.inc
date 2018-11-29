<?php

/**
 * @file
 * Form and functions related to create_digitization_request.
 */

/**
 * Displays the form to create_digitization_request from a batch item.
 *
 * This approache is needed in order to display two separate forms on a single
 * page.  The lookup form's action needs to be handled separately from the
 * transaction_form.  Even though the node value is not passed as an argument, it
 * can be derived by using the arg() function.
 *
 * @return string
 *   HTML markup.
 */
function islandora_digital_workflow_create_digitization_request() {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/lookup.form');
  $lookup_form_markup = theme('islandora_digital_workflow_lookup', array('searchterm' => ''));
  drupal_set_title(t('Islandora Digital Workflow - Submit a Digitization Request'), CHECK_PLAIN);
  $form = drupal_get_form('islandora_digital_workflow_create_digitization_request_form');
  $markup = $lookup_form_markup . drupal_render($form);
  return $markup;
}

/**
 * Displays form for create_digitization_request_form.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @return string
 *   HTML markup.
 */
function islandora_digital_workflow_create_digitization_request_form(array $form, array &$form_state) {
  module_load_include('inc', 'islandora_digitization_requests', 'includes/db');

  // Load all of the available Webforms and if there is only one, redirect to the
  // URL that handles the creation of a submission to that form, else display
  // a form with links to Submit a Digitization Request to each of the Webforms
  // that exist.
  $webforms = islandora_digitization_requests_get_webforms();
  $digitization_request_webforms = islandora_digitization_requests_get_scan_request_webforms($webforms);

  if (count($digitization_request_webforms) == 1) {
    reset($digitization_request_webforms);
    $nid = key($digitization_request_webforms);
    drupal_goto('node/' . $nid);
  }
  if (count($digitization_request_webforms) < 1) {
    drupal_set_message(t('There are no Webforms created or none have been configured as a "Digitization Request" Webform.  Click ') .
        l('Content | Add content | Webform', 'node/add/webform') . 
        t(' to create a Webform.  Click ') .
        l('Islandora Digitization Requests - Configuration', 'admin/islandora/tools/islandora_digitization_requests') .
        t(' to configure which Webforms are "Digitization Request".'), 'error');
  }
  else {
    // The "Submission" page for any of the Webforms is simply the node/{nid} handler.
    $links = array();
    foreach ($digitization_request_webforms as $nid => $webform_title) {
      $links[] = l($webform_title['title'], 'node/' . $nid);
    }

    $form['links_wrapper'] = array(
        '#type' => 'fieldset',
        '#title' => 'Select Request Form',
        '#type' => 'item',
        '#markup' => '<ul>' . implode("</li><li>", $links) . '</ul>',
    );
  }
  $form['Cancel'] = array(
      '#type' => 'submit',
      '#value' => t('Cancel'),
  );

  return $form;
}

/**
 * The create_digitization_request form submit handler.
 *
 * Function that updates a batch transaction with user's input.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_digital_workflow_create_digitization_request_form_submit(array $form, array &$form_state) {
  // Send the user back to the dashboard page.
  drupal_goto('islandora/islandora_digital_workflow');
}
