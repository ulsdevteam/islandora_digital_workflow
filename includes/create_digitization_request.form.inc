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

function islandora_digital_workflow_get_digitization_requests_config_form() {
  module_load_include('inc', 'islandora_digitization_requests', 'includes/db');

  $mapping_to_fieldnames_for_textfields = islandora_digital_workflow_batch_field_mappings(TRUE, FALSE);
  $mapping_to_fieldnames_for_intfields = islandora_digital_workflow_batch_field_mappings(FALSE, TRUE);

  $form_elements = array();
  $webforms = islandora_digitization_requests_get_webforms();
  $webform_digitization_requests = variable_get('islandora_digitization_requests_webform_forms', array());
  $webform_options = variable_get('islandora_digital_workflow_digitization_requests', array());

  $link_attributes = array('attributes'=>array('target' => '_blank'));
  foreach ($webforms as $nid => $webform_arr) {
    $searchable_fields = islandora_digital_workflow_searchable_fields($nid);
    $search_options = array();
    $form_components = islandora_digitization_requests_webform_components($nid);
    if (!$webform_digitization_requests[$nid]['selected']) {
      $form_elements['webform_options']['webform_' . $nid] = array(
        'webform_' . $nid . '_is_digitization_request' => array(
          '#type' => 'fieldset',
          '#title' => '<b>' . $webform_arr['title'] . '</b> Options and mappings',
          'webform_info_' . $nid => array(
            '#type' => 'item',
            '#markup' => '"' . $webform_arr['title'] . '" is not configured to be ' .
              'a Digitization Request.  That can be configured in ' .
              l('Islandora Digitization Requests - Settings', 'admin/islandora/tools/islandora_digitization_requests') . '.',
          ),
        ),
      );
    }
    else {
      $form_elements['webform_options']['webform_' . $nid] = array(
        '#type' => 'fieldset',
        '#title' => '<b>' . $webform_arr['title'] . '</b> Options and mappings',
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        'webform_info_' . $nid => array(
          '#markup' => '<ul class="action-links"><li>' . l('Create new "' . $webform_arr['title'] . '"', '/node/' . $nid, $link_attributes) . '</li></ul>',
          '#type' => 'item',
        ),
        'webform_' . $nid . '_searchfields_wrapper' => array(
          '#type' => 'fieldset',
          '#title' => 'Search fields',
          '#description' => '<p>Select which fields from this Webform are include in the search operation. ' .
            '</p><span><b>Configured search fields: </b>' . implode(', ', $searchable_fields) . '</span>',
          '#collapsible' => TRUE,
          '#collapsed' => FALSE,
          // The individual form field components are enumerated in the loop below.
        ),
      );
      if (module_exists('islandora_digital_workflow')) {
        $form_elements['webform_options']['webform_' . $nid]['webform_' . $nid . '_mappings_wrapper'] = array(
          '#type' => 'fieldset',
          '#title' => 'Mappings',
          '#description' => 'Select the mappings for fields converted into a Digital Workflow batch records.',
          '#collapsible' => TRUE,
          '#collapsed' => FALSE,
          // The individual form field components are enumerated in the loop below.
        );
      }

      $allowed_field_types_for_text_mapping = array('textarea', 'textfield');

      foreach ($form_components as $cid => $components_arr) {
        $form_elements['webform_options']['webform_' . $nid]['webform_' . $nid . '_searchfields_wrapper']['webform_' . $nid . '_searchfields_' . $cid] = array(
          '#type' => 'checkbox',
          '#title' => $components_arr['name'],
          '#default_value' => ((array_key_exists($nid, $webform_options) && array_key_exists('search_fields', $webform_options[$nid])) ? $webform_options[$nid]['search_fields'][$cid]: 0),
        );
        if (module_exists('islandora_digital_workflow')) {
          $form_elements['webform_options']['webform_' . $nid]['webform_' . $nid . '_mappings_wrapper']['webform_' . $nid . '_mappings_' . $cid] = array(
            '#type' => 'select',
            '#title' => $components_arr['name'] . '<span class="light_text"> - (' . $components_arr['type'] . ')</span>',
            '#options' => ((!(array_search($components_arr['type'], $allowed_field_types_for_text_mapping) === FALSE))
                ? $mapping_to_fieldnames_for_textfields : array_merge($mapping_to_fieldnames_for_textfields, $mapping_to_fieldnames_for_intfields)),
            '#default_value' => ((array_key_exists($nid, $webform_options) && array_key_exists('mapped_fields', $webform_options[$nid])) ? $webform_options[$nid]['mapped_fields'][$cid]: ''),
          );
        }
      }
    }
  }

  return $form_elements;
}

function islandora_digital_workflow_batch_field_mappings($text_fields = TRUE, $integer_fields = TRUE) {
  module_load_include('install', 'islandora_digital_workflow', 'islandora_digital_workflow');

  $schema = islandora_digital_workflow_schema();
  $allowed_field_types_for_mapping = $mapping_to_fieldnames = array();
  $mapping_to_fieldnames[] = 'Select field';
  if ($text_fields) {
    $allowed_field_types_for_mapping = array('varchar', 'text');
  }
  if ($integer_fields) {
    $allowed_field_types_for_mapping = array('int');
  }

  foreach ($schema['islandora_digital_workflow_batch']['fields'] as $fieldname => $field) {
    if (!(array_search($field['type'], $allowed_field_types_for_mapping) === FALSE) ) {
      $desc = array_key_exists('description', $field) ? ' - ' . $field['description'] : '';
      $mapping_to_fieldnames[$fieldname] = $fieldname . ' (' . $field['type'] . ')' . $desc;
    }
  }
  return $mapping_to_fieldnames;
}

function islandora_digital_workflow_searchable_fields($nid, $return_form_key = FALSE) {
  module_load_include('inc', 'islandora_digitization_requests', 'includes/db');
  $webform_options = variable_get('islandora_digital_workflow_digitization_requests', array());

  $searchable_fields = array();
  $components = islandora_digitization_requests_webform_components($nid);
  dpm($components);
  foreach ($webform_options as $options_nid => $options) {
    if ($nid == $options_nid) {
      foreach ($options['search_fields'] as $cid => $setting) {
        if ($setting) {
          $searchable_fields[$cid] = ($return_form_key ? $components[$cid]['form_key'] : $components[$cid]['name']);
        }
      }
    }
  }
  return $searchable_fields;
}

function islandora_digital_workflow_convert_digitization_request_to_batch(array $form, array &$form_state) {
  // Take the submission record and process that according to the configuration
  // settings for the $digitization_request_options[$nid]['mapped_fields']
  $nid = arg(3);
  $sid = arg(4);
  return array('nothing' => array(
      '#type' => 'item',
      '#markup' => 'nid = ' . $nid . '<br>' . 'sid = ' . $sid));
}