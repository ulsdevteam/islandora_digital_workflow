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
  $digitization_request_webforms = islandora_digitization_requests_get_digitization_request_webforms($webforms);

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

/**
 * Returns only the configuration fields related to Webform submissions that are
 * "digitization requests".
 *
 * @return string
 *   Array that makes up the Drupal form definition.
 */
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
    $form_components = islandora_digitization_requests_webform_components($nid);
    $searchable_fields = islandora_digital_workflow_searchable_fields($nid);
    $search_options = array();
    if (!isset($webform_digitization_requests[$nid]) || !$webform_digitization_requests[$nid]['selected']) {
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
      $copy_form_components =array();
      foreach ($form_components as $key => $value) {
        $copy_form_components[$key] = $value;
        $extra_as_arr = unserialize($value['extra']);
        $copy_form_components[$key]['extra'] = $extra_as_arr;
        if (array_key_exists('items', $extra_as_arr)) {
          $copy_form_components[$key]['extra']['items'] = explode("\n", $extra_as_arr['items']);
        }
      }
      $javascript_variables =  json_encode($copy_form_components);
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
          '#description' => '<span>Configured search fields: ' . implode(', ', $searchable_fields) . '</span><p>Select which fields from this Webform are include in the search operation.',
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
      $form_elements['webform_options']['webform_' . $nid]['webform_' . $nid . '_message_info_wrapper'] = array(
          '#type' => 'fieldset',
          '#title' => 'Submission page prefix',
          '#description' => 'Enter HTML message info that you would like to appear above "' . $webform_arr['title'] . '" submissions.',
          '#collapsible' => TRUE,
          '#collapsed' => FALSE,
          'message_info_' . $nid => array(
            '#type' => 'textarea',
            '#default_value' => ((array_key_exists($nid, $webform_options) && array_key_exists('message_info_' . $nid, $webform_options[$nid])) ? $webform_options[$nid]['message_info_' . $nid] : ''),
          ),
      );

      module_load_include('install', 'islandora_digital_workflow', 'islandora_digital_workflow');
      // Search fields
      foreach ($form_components as $cid => $components_arr) {
        $form_elements['webform_options']['webform_' . $nid]['webform_' . $nid . '_searchfields_wrapper']['webform_' . $nid . '_searchfields_' . $cid] = array(
          '#type' => 'checkbox',
          '#title' => $components_arr['name'],
          '#default_value' => ((array_key_exists($nid, $webform_options) && array_key_exists('search_fields', $webform_options[$nid])) ? $webform_options[$nid]['search_fields'][$cid]: 0),
        );
      }

      $allowed_field_types_for_text_mapping = array('textarea', 'textfield', 'select');
      $tmp_dr_text_options = $tmp_dr_numeric_options = array();
      $map_dr_text_options = $map_dr_numeric_options = array('' => 'Select field');
      foreach ($form_components as $cid => $components_arr) {
        if ((!(array_search($components_arr['type'], $allowed_field_types_for_text_mapping) === FALSE))) {
          $is_text = TRUE;
          // The select boxes can be considered numeric if they are not multiple and
          // all of the items values are numeric.
          if ($components_arr['type'] == 'select') {
            $select_input_extra_arr = unserialize($components_arr['extra']);
            if ((array_key_exists('multiple', $select_input_extra_arr)) && ($select_input_extra_arr['multiple'] < 1)) {
              if ((array_key_exists('items', $select_input_extra_arr))) {
                $all_numeric = TRUE;
                if (is_array($select_input_extra_arr['items']) && (count($select_input_extra_arr['items']) > 0)) {
                  foreach ($select_input_extra_arr['items'] as $val => $text) {
                    $all_numeric &= is_numeric($text);
                  }
                }
                $is_text = !$all_numeric;
              }
            }
          }
        }
        if ($is_text) {
          $tmp_dr_text_options[$cid] = $components_arr['name'];
        }
        else {
          $tmp_dr_numeric_options[$cid] = $components_arr['name'];
        }
      }
      arsort($tmp_dr_text_options);
      arsort($tmp_dr_numeric_options);
      foreach ($tmp_dr_text_options as $k => $v) {
        $map_dr_text_options[$k] = $v;
      }
      foreach ($tmp_dr_numeric_options as $k => $v) {
        $map_dr_numeric_options[$k] = $v;
      }

      // Mappings (string fields)
      $form_elements['webform_options']['webform_' . $nid]['webform_' . $nid . '_mappings_wrapper']['stringfields'] = array(
          '#type' => 'fieldset',
          '#title' => t('Batch fields that are alphanumeric'),
          '#attributes' => array('class' => array(
            'multi_input_line'
          )),
      );
      $toggle = FALSE;
      foreach ($mapping_to_fieldnames_for_textfields as $fieldname => $value) {
        $form_elements['webform_options']['webform_' . $nid]['webform_' . $nid . '_mappings_wrapper']['stringfields']['webform_' . $nid . '_wrapwrap_' . $fieldname] = array(
          '#type' => 'fieldset',
        );

        $form_elements['webform_options']['webform_' . $nid]['webform_' . $nid . '_mappings_wrapper']['stringfields']['webform_' . $nid . '_wrapwrap_' . $fieldname]['webform_' . $nid . '_mappings_' . $fieldname] = array(
          '#type' => 'select',
          '#title' => $value,
          '#options' => $map_dr_text_options,
          '#default_value' => ((array_key_exists($nid, $webform_options) && array_key_exists('mapped_fields', $webform_options[$nid]) &&
                array_key_exists($fieldname, $webform_options[$nid]['mapped_fields'])) ?
                $webform_options[$nid]['mapped_fields'][$fieldname]: ''),
          '#attributes' => array('class' => array(($toggle) ? 'odd_child' : 'even_child')),
        );

        @list($field_description_only, $junk) = explode(" (", strip_tags($value));
        $form_elements['webform_options']['webform_' . $nid]['webform_' . $nid . '_mappings_wrapper']['stringfields']['webform_' . $nid . '_wrapwrap_' . $fieldname]['webform_' . $nid . '_default_' . $fieldname] = array(
          '#type' => 'textfield',
          '#title' => '<i class="disabled_text">default value</i>',
          '#default_value' => '',
          '#attributes' => array(
              'title' => 'When there is no mapping, provide a default value for the batch record\'s "' . $field_description_only . '"',
              'class' => array(($toggle) ? 'odd_child' : 'even_child')),
          '#default_value' => ((array_key_exists($nid, $webform_options) && array_key_exists('default_values_fields', $webform_options[$nid]) &&
                array_key_exists($fieldname, $webform_options[$nid]['default_values_fields'])) ?
                $webform_options[$nid]['default_values_fields'][$fieldname]: ''),
        );
        if ($fieldname == 'islandora_model') {
          // modify the #attributes for this element to add a javascript callback.
          $form_elements['webform_options']['webform_' . $nid]['webform_' . $nid . '_mappings_wrapper']['stringfields']['webform_' . $nid . '_wrapwrap_' . $fieldname]['webform_' . $nid . '_mappings_' . $fieldname]['#attributes']['onchange'] = 'model_mapping_selected(this,' . $javascript_variables . ')';
          // render all of the possible mapping fields now and keep them all hidden
          // unless that mapping is already configured (persistant relationships
          // should be displayed). Make the whole fieldset visible and persist
          // the configuration values by setting default_value in the foreach
          // loop below.
          $configured_supported_models = islandora_digital_workflow_get_accessible_cmodels(FALSE);
          $islandora_model_select_options = array('' => 'Select model');
          $islandora_model_select_options = array_merge($islandora_model_select_options, $configured_supported_models);
          $show_fieldset = FALSE;
          foreach ($form_components as $cid => $components_arr) {
            // If the value is already set, make the whole fieldset visible and
            // persist the configuration values by setting default_value
            $islandora_model_map_field = (array_key_exists($nid, $webform_options) &&
                array_key_exists('islandora_model_mappings', $webform_options[$nid]) &&
                is_array($webform_options[$nid]['islandora_model_mappings']) &&
                array_key_exists($cid, $webform_options[$nid]['islandora_model_mappings']) &&
                is_array($webform_options[$nid]['islandora_model_mappings'][$cid])) ?
                    $webform_options[$nid]['islandora_model_mappings'][$cid] : NULL;

            $this_field_extra_values = (array_key_exists('extra', $components_arr)) ?
              unserialize($components_arr['extra']) : '';
            $this_field_items = ($this_field_extra_values) ? 
                (array_key_exists('items', $this_field_extra_values) ? explode("\r\n", trim($this_field_extra_values['items'])) : array()) : array();
            $show_fieldset = (is_array($islandora_model_map_field));
            if (count($this_field_items) > 0) {
              $form_elements['webform_options']['webform_' . $nid]['webform_' . $nid . '_mappings_wrapper']['stringfields']['webform_' . $nid . '_wrapwrap_' . $fieldname]['webformfield_fieldset_'. $nid . '_' . $cid] = array(
                '#type' => 'fieldset',
                '#title' => $components_arr['name'],
                '#description' => t('<b>"@webform_title" mappings for Islandora Model</b>' .
                    '<p>The mappings from the selected field into Islandora Model is ' .
                    'special may require explicit mapping of the field values and could ' .
                    'result in the abillity to create more than one batch from a ' .
                    'digitization request.</p>', array(
                      '@webform_title' => $webform_arr['title'],
                    )),
                '#attributes' => array(
                    'class' => array('idw_special_mapping_fieldset',
                        ($show_fieldset) ? 'dr_field_show' : 'dr_field_hide')
                  ),
              );

              foreach ($this_field_items as $webfield_value_label) {
                @list($value, $label) = explode("|", $webfield_value_label);
                $form_elements['webform_options']['webform_' . $nid]['webform_' . $nid . '_mappings_wrapper']['stringfields']['webform_' . $nid . '_wrapwrap_' . $fieldname]['webformfield_fieldset_'. $nid . '_' . $cid]['dr_field_'. $nid . '_' . $cid . '_' . $value] = array(
                  '#type' => 'select',
                  '#title' => $label,
                  '#options' => $islandora_model_select_options,
                  '#default_value' => (is_array($islandora_model_map_field) &&
                    array_key_exists($label, $islandora_model_map_field) ?
                      $islandora_model_map_field[$label] : ''),
                );
              }
            }

          }
        }
        $toggle = !$toggle;
      }
      // Mappings (numeric fields)
      $form_elements['webform_options']['webform_' . $nid]['webform_' . $nid . '_mappings_wrapper']['numericfields'] = array(
          '#type' => 'fieldset',
          '#title' => t('Batch fields that are numeric'),
          '#attributes' => array('class' => array(
            'multi_input_line'
          )),
      );
      $toggle = FALSE;
      foreach ($mapping_to_fieldnames_for_intfields as $fieldname => $value) {
        $form_elements['webform_options']['webform_' . $nid]['webform_' . $nid . '_mappings_wrapper']['numericfields']['webform_' . $nid . '_wrapwrap_' . $fieldname] = array(
          '#type' => 'fieldset',
        );
        $form_elements['webform_options']['webform_' . $nid]['webform_' . $nid . '_mappings_wrapper']['numericfields']['webform_' . $nid . '_wrapwrap_' . $fieldname]['webform_' . $nid . '_mappings_' . $fieldname] = array(
          '#type' => 'select',
          '#title' => $value,
          '#options' => $map_dr_numeric_options,
          '#default_value' => ((array_key_exists($nid, $webform_options) && array_key_exists('mapped_fields', $webform_options[$nid]) &&
                array_key_exists($fieldname, $webform_options[$nid]['mapped_fields'])) ?
                $webform_options[$nid]['mapped_fields'][$fieldname]: ''),
          '#attributes' => array('class' => array(($toggle) ? 'odd_child' : 'even_child')),
        );
        @list($field_description_only, $junk) = explode(" (", strip_tags($value));
        $form_elements['webform_options']['webform_' . $nid]['webform_' . $nid . '_mappings_wrapper']['numericfields']['webform_' . $nid . '_wrapwrap_' . $fieldname]['webform_' . $nid . '_default_' . $fieldname] = array(
          '#type' => 'textfield',
          '#title' => '<i class="disabled_text">default value</i>',
          '#default_value' => '',
          '#suffix' => '<br class="break_float">',
          '#attributes' => array('title' => 'When there is no mapping, provide a default value for the batch record\'s "' . $field_description_only . '"',
              'class' => array(($toggle) ? 'odd_child' : 'even_child')),
          '#default_value' => ((array_key_exists($nid, $webform_options) && array_key_exists('default_values_fields', $webform_options[$nid]) &&
                array_key_exists($fieldname, $webform_options[$nid]['default_values_fields'])) ?
                $webform_options[$nid]['default_values_fields'][$fieldname]: ''),
        );
        $toggle = !$toggle;
      }
    }
  }
  return $form_elements;
}

/**
 * Function to return field mappings that are possible for the
 * islandora_digitial_workflow_batch table somethines "text" or "numeric" grouped.
 *
 * @param boolean $text_fields
 *   Whether or not this call is returning "numeric" fields (any schema field
 * where type = "varchar" or "text").
 * @param boolean $integer_fields
 *   Whether or not this call is returning "numeric" fields (any schema field
 * where type = "int").
 * @return array
 *   Array of mappable field names.
 */
function islandora_digital_workflow_batch_field_mappings($text_fields = TRUE, $integer_fields = TRUE) {
  module_load_include('install', 'islandora_digital_workflow', 'islandora_digital_workflow');

  $never_map_fieldnames = array('batch_name', 'uploaded_batch_items_file', 'nid', 'workflow_sequence_id', 'webform_submission_sid', 'user', 'lastmod');
  $schema = islandora_digital_workflow_schema();
  $allowed_field_types_for_mapping = $mapping_to_fieldnames = array();
  if ($text_fields) {
    $allowed_field_types_for_mapping = array('varchar', 'text');
  }
  if ($integer_fields) {
    $allowed_field_types_for_mapping = array('int');
  }
  uasort($schema['islandora_digital_workflow_batch']['fields'], "islandora_digital_workflow_field_sorting_compare");

  foreach ($schema['islandora_digital_workflow_batch']['fields'] as $fieldname => $field) {
    if (!(array_search($field['type'], $allowed_field_types_for_mapping) === FALSE) &&
        (array_search($fieldname, $never_map_fieldnames) === FALSE)) {
      $desc = array_key_exists('description', $field) ? '<span class="larger_font">' . $field['description'] . '</span>' : '';
      $mapping_to_fieldnames[$fieldname] = $desc . '<span class="nonbold "> (' . $field['type'] . ')</span>';
    }
  }
  // If this is for $text_fields, make sure the islandora_model values are first.
  if ($text_fields) {
    $tmp = $mapping_to_fieldnames;
    $mapping_to_fieldnames = array();
    $mapping_to_fieldnames['islandora_model'] = $tmp['islandora_model'];
    foreach ($tmp as $fieldname => $val) {
      if ($fieldname <> 'islandora_model') {
        $mapping_to_fieldnames[$fieldname] = $tmp[$fieldname];
      }
    }
  }
  
  return $mapping_to_fieldnames;
}

/**
 * Helper function so that the associative array can be searched based on the
 * elements' ['description'] value.
 *
 * @param array $a
 * @param array $b
 * @return int
 *   Determines the sorted item direction.  0 = no change, 1 = "a" is biggest
 * 2 = "b" is biggest
 */
function islandora_digital_workflow_field_sorting_compare($a, $b) {
  $a_desc = array_key_exists('description', $a) ? $a['description'] : '';
  $b_desc = array_key_exists('description', $b) ? $b['description'] : '';
  if ($a_desc == $b_desc) {
      return 0;
  }
  return ($a_desc < $b_desc) ? -1 : 1;
}

/**
 * Helper function to get the configured fields for the given Webform (by $nid)
 * that are set to be searchable.
 *
 * @param integer $nid
 *   nid value for a Drupal node object.
 * @param boolean $return_form_key
 *   Whether or not to return the form_key field.  When set to TRUE, this call is
 * coming from the SQL code that is building an `IN` statement.
 * @return array
 *   Array of searchable fields for the given webform.
 */
function islandora_digital_workflow_searchable_fields($nid, $return_form_key = FALSE) {
  module_load_include('inc', 'islandora_digitization_requests', 'includes/db');
  $webform_options = variable_get('islandora_digital_workflow_digitization_requests', array());

  $searchable_fields = array();
  $components = islandora_digitization_requests_webform_components($nid);
  foreach ($webform_options as $options_nid => $options) {
    if ($nid == $options_nid) {
      if (array_key_exists('search_fields', $options)) {
        foreach ($options['search_fields'] as $cid => $setting) {
          if ($setting) {
            $searchable_fields[$cid] = ($return_form_key ? $components[$cid]['form_key'] : $components[$cid]['name']);
            // ONLY wrap with quotes if not $return_form_key.
            if (!$return_form_key && strstr($searchable_fields[$cid], ' ')) {
              $searchable_fields[$cid] = '"<b>' . $searchable_fields[$cid] . '</b>"';
            }
            elseif (!$return_form_key) {
              $searchable_fields[$cid] = '<b>' . $searchable_fields[$cid] . '</b>';
            }
          }
        }
      }
    }
  }
  return $searchable_fields;
}

/**
 * Form to handle "Webform to batch record" mapping.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_digital_workflow_convert_digitization_request_to_batch(array $form, array &$form_state) {
  module_load_include('inc', 'islandora_digitization_requests', 'includes/db');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');

  $nid = arg(3);
  $sid = arg(4);
  // Need the extra->items from the components of this webform that map to the
  // islandora_model for this webform's configuration.
  $digitization_requests_config = variable_get('islandora_digital_workflow_digitization_requests');
  $islandora_model_mappings_config = (is_array($digitization_requests_config) && array_key_exists($nid, $digitization_requests_config) &&
      array_key_exists('islandora_model_mappings', $digitization_requests_config[$nid])) ?
      $digitization_requests_config[$nid]['islandora_model_mappings'] : array();
  // assume no mapping to the islandora_model.
  $islandora_model_mapped_to_cid = NULL;
  $submission_field_mappings = array();
  foreach($islandora_model_mappings_config as $islandora_model_mapped_to_cid => $submission_field_mappings) {
  }

  $results = islandora_digital_workflow_map_submission($nid, $sid, $islandora_model_mappings_config);

//  $form_components = islandora_digitization_requests_webform_components($nid);
  $submission_data = islandora_digitization_requests_get_webform_submission($sid);

  $webforms = islandora_digitization_requests_get_webforms();
  $digitization_request_webforms = islandora_digitization_requests_get_digitization_request_webforms($webforms);

  // # take the submission_data element that corresponds to the field that is
  // mapped to the islandora_model.
  $set_submission_values_that_map_to_islandora_model = array_key_exists($islandora_model_mapped_to_cid, $submission_data) ?
      $submission_data[$islandora_model_mapped_to_cid] : '';
  $submission_values_that_map_to_islandora_model_arr = array();
  foreach ($set_submission_values_that_map_to_islandora_model as $submission_values_that_map_to_islandora_model) {
    if (array_key_exists('data', $submission_values_that_map_to_islandora_model)) {
      $submission_values_that_map_to_islandora_model_arr[] = $submission_values_that_map_to_islandora_model['data'];
    }
  }

  $mappings = $results['mappings'];
  $multiple_fields_mappedto_islandora_model = ((count($results['islandora_models_maptobatch']) > 0) ?
    reset($results['islandora_models_maptobatch'])['data'] : '');
  $islandora_model_mapped_to_cidfield = '';
  foreach ($mappings as $m => $mapping_html) {
    if (strstr($mapping_html, '.islandora_model')) {
      $islandora_model_mapped_to_cidfield = strip_tags($mapping_html, '<b>');
    }
  }
  @list($islandora_model_mapped_to_cidfield, $junk) = explode(" to islandora_digital_workflow_batch.islandora_model", $islandora_model_mapped_to_cidfield);

  $islandora_digital_workflow_batch_fields = $results['islandora_digital_workflow_batch_fields'];
  $return_to_submission_link = l(t('Return to "@digitization_request_name" submission #@sid',
      array('@sid' => $sid,
            '@digitization_request_name' => $digitization_request_webforms[$nid]['title'])), 'node/' . $nid . '/submission/' . $sid);
  $confirm_mapping_form = array();
  $confirm_mapping_form['nothing'] = array(
    '#type' => 'item',
    '#markup' => '<div class="messages warning">' . $return_to_submission_link . '</div>' .
      ((count($mappings) > 0) ?
        '<div class="big_font bad">' .
          t('<b>THIS CAN NOT BE UNDONE!</b><br>Are you SURE that you want to ' .
            'map this digitization request into a batch record?') .
      '</div><h4>Mappings for this request:</h4><div class="indent-item">' . implode("<br>", $mappings) . '</div>' : '<b>There were no fields in the ' .
        'digitization request that can map to any batch record fields.</b>')
  );

  $batch_name_inputfield_suffix = '';
  if ($multiple_fields_mappedto_islandora_model) {
    $batch_name_inputfield_suffix = '<span class="pseudo_textfield">_</span>';
  }
  $confirm_mapping_form['batch_name'] = array(
    '#type' => 'textfield',
    '#title' => 'Batch name' .
     (($multiple_fields_mappedto_islandora_model) ? ' prefix' : ''),
    '#description' =>
      (($multiple_fields_mappedto_islandora_model) ?
        'Since there are multiple field values that are mapped to Islandora Model ' .
        'for the scan request field ' . $islandora_model_mapped_to_cidfield . ', ' .
        'multiple batches will be created with batch name values that begin ' .
        'with the batch name prefix specified above appended to the value that ' .
        'maps to each of the mapped islandora models.' : ''),
    '#default_value' => '',
  );
  $confirm_mapping_form['sid'] = array(
    '#type' => 'hidden',
    '#default_value' => $sid,
  );
  $confirm_mapping_form['nid'] = array(
    '#type' => 'hidden',
    '#default_value' => $nid,
  );

  $create_what_text = ($multiple_fields_mappedto_islandora_model) ?
    $multiple_fields_mappedto_islandora_model . ' Batch records' : 'batch record';

  $confirm_mapping_form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Create @create_what_text', array('@create_what_text' => $create_what_text)),
  );

  $confirm_mapping_form['cancel'] = array(
    '#type' => 'submit',
    '#prefix' => '&nbsp; ',
    '#value' => t('Cancel'),
  );
  return $confirm_mapping_form;
}

/**
 * Validation function to check for duplicate batch_name values in a new
 * "Webform to batch record" mapping.
 *
 * @param array $elements
 *   Drupal elements representation as an array.
 * @param array $form_state
 *   Drupal form_state representation as passed to the validate function.
 * @param string $form_id
 *   The form_id that is being validated -- optional.
 */
function islandora_digital_workflow_convert_digitization_request_to_batch_validate(&$elements, &$form_state, $form_id = NULL) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
  // Check for the batch name already existing.
  $batch_name = $form_state['values']['batch_name'];
  $batch_record = islandora_digital_workflow_get_batch_record_by_name($batch_name);
  if ($form_state['triggering_element']['#value'] <> 'Cancel' && is_array($batch_record)) {
    form_set_error('batch_name', t('A batch already exists with the name @batch_name.',
        array('@batch_name' => $batch_name)));
    $link_options = array('attributes'=>array(
        'target' => '_blank'));
    $edit_this_batch_link = l($batch_name, 'islandora/islandora_digital_workflow/edit_batch/' . $batch_name, $link_options);
    drupal_set_message(t('Edit that existing batch "') . $edit_this_batch_link . '" (link will open in a new tab).', 'warning');
  }
}

/**
 * Submit handler for the map to batch record functionality.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_digital_workflow_convert_digitization_request_to_batch_submit(array $form, array &$form_state) {
  $nid = $form_state['values']['nid'];
  $sid = $form_state['values']['sid'];
  $batch_name = $form_state['values']['batch_name'];

  if ($form_state['triggering_element']['#value'] == 'Cancel') {
    drupal_set_message('Mapping of this digitization request submission to an ' .
        'islandora_digital_workflow_batch record has been canceled.');
    drupal_goto('node/' . $nid . '/submission/' . $sid);
  }

  $islandora_digital_workflow_batch_stub_fields = array(
    'batch_name' => $batch_name,
    'default_type_of_resource' => '',
    'islandora_model' => '',
    'is_batch_active' => 0,
    'is_batch_request' => 0,
    'object_count' => 0);

  $results = islandora_digital_workflow_map_submission($nid, $sid);
  $mappings = $results['mappings'];
  $islandora_digital_workflow_batch_fields = array_merge(
      $islandora_digital_workflow_batch_stub_fields, 
      $results['islandora_digital_workflow_batch_fields']);

  // Add the current timestamp for the 'created' batch fields value.
  $islandora_digital_workflow_batch_fields['created'] = date("Y-m-d H:i:s");

  // Insert this record into the batch table.
  $batch_id = db_insert('islandora_digital_workflow_batch')
    ->fields($islandora_digital_workflow_batch_fields)
    ->execute();

//  // Delete the submission record.
//  db_delete('webform_submissions')->condition('sid', $sid)
//    ->execute();
//  db_delete('webform_submitted_data')->condition('sid', $sid)
//    ->execute();

  drupal_set_message(t('Mapping to an islandora_digital_workflow_batch record ' .
      'complete.  Submission record has been deleted.  There are more fields ' .
      'to a batch than there are to a digitization request.  Please enter the ' .
      'known values for this batch record.'));

  drupal_goto('islandora/islandora_digital_workflow/edit_batch/' . $batch_name);
}

/**
 * This will dynamically map the requested Webform submission and any configured
 * field defaults to an islandora_digital_workflow_batch record.
 *
 * @global object $user
 *   Drupal user object.
 * @param integer $nid
 *   nid value for a Drupal node object.
 * @param type $sid
 *   sid value for a Webform submission object.
 * @param array $islandora_model_mappings_config
 *   OPTIONAL array of fields mapped between the items of a submission field
 * and the possible islandora models.
 * @return type
 */
function islandora_digital_workflow_map_submission($nid, $sid, $islandora_model_mappings_config = array()) {
  global $user;

  $submission_data = islandora_digitization_requests_get_webform_submission($sid);
  // Use the configuration to set field values for a islandora_digital_workflow_batch
  // record based on the field values for the given submission.
  $webform_options = variable_get('islandora_digital_workflow_digitization_requests', array());
  $mappings = $unmapped_fields = $islandora_models_maptobatch = array();
  $components = islandora_digitization_requests_webform_components($nid);
  $islandora_digital_workflow_batch_fields = array();
  foreach ($webform_options as $options_nid => $options) {
    if ($nid == $options_nid) {
      $webform_node = node_load($nid);
      $webform_title = (is_object($webform_node) ? $webform_node->title :
          t('(unknown webform with nid = @nid)', array('@nid' => $nid)));
      // Set up an array to keep track of submitted data that does not get mapped
      foreach ($components as $cid => $webform_field) {
        if (count($submission_data[$cid]) > 1) {
          $values = array();
          foreach ($submission_data[$cid] as $num => $submission_field_arr) {
            if (array_key_exists('data', $submission_field_arr)) {
              $values[] = $submission_field_arr['data']; 
            }
          }
          $data_point = array('data' => implode(", ", $values));
        }
        else {
          $data_point = reset($submission_data[$cid]);
        }
        if (array_key_exists($cid, $submission_data) && 
          array_key_exists('data', $data_point) &&
          $data_point['data']) {
          $unmapped_fields[$cid] = array(
              'webform_field_label' => strtoupper($webform_field['name']),
              'value' => $data_point['data'],
            );
        }
      }
      $data_points = array();
      foreach ($options['mapped_fields'] as $fieldname => $setting) {
        if ($setting) {
          if (array_key_exists($setting, $submission_data)) {
            if (count($submission_data[$setting]) > 1) {
              $values = array();
              foreach ($submission_data[$setting] as $num => $submission_field_arr) {
                if (array_key_exists('data', $submission_field_arr)) {
                  $values[] = $submission_field_arr['data'];
                }
              }
              $data_points[$setting] = array('data' => implode(", ", $values));
            }
            else {
              $data_points[$setting] = reset($submission_data[$setting]);
            }
            if ($fieldname == 'islandora_model') {
              $islandora_models_maptobatch = $data_points;
            }
            $file_truncated_ellipsis = (strlen($data_points[$setting]['data']) > 600) ? ' ... ' : '';
            $mappings[] = '<div><b>' . $components[$setting]['name'] . '</b> to ' .
                '<span class="idw_fieldname">islandora_digital_workflow_batch.' . $fieldname . '</span><br>' .
                '<span class="small_font">' . substr($data_points[$setting]['data'], 0, 600) .
                $file_truncated_ellipsis . '</span></div>';
            $islandora_digital_workflow_batch_fields[$fieldname] = $data_points[$setting]['data'];
            unset($unmapped_fields[$setting]);
          }
          else {
            $mappings[] = '<div><b>' . $components[$setting]['name'] . '</b> could ' .
                'not be mapped because the submission did not have a value for the ' .
                'data element ' . $setting . '</div>';
          }
        }
      }

      // Set any values for the batch record that are coming only from the 
      // config "defaults" field values.
      foreach ($options['default_values_fields'] as $fieldname => $fieldvalue) {
        if ($fieldvalue && array_search($fieldname, $islandora_digital_workflow_batch_fields) === FALSE) {
          $mappings[] = '<div><b>' . $fieldname . '</b> is set to a configuration default value of <br><b><i>"' . $fieldvalue . '"</i></b></div>';
          $islandora_digital_workflow_batch_fields[$fieldname] = $fieldvalue;
        }
      }

      // Set the value for $uid_cid if that $cid key is the same as the $components
      // that has a form_key value of "uid".  Only if there is at least one
      // submission, set the user value from that submission.
      // NOTE: this $cid value was the final index for the loop through the
      // webform's fields - each submission_data element will have an element
      // for each of the $cid values, so using $cid here is safe to access the
      // last data point's drupal_username.
      if (count($submission_data) > 0) {
        $last_datapoint = reset($submission_data[$cid]);

        $islandora_digital_workflow_batch_fields['user'] = $last_datapoint['drupal_username'];
        $mappings[] = '<div>"<b>' . $last_datapoint['drupal_username'] . '</b>" to ' .
            '<span class="idw_fieldname">islandora_digital_workflow_batch.user</span></div>';
      }
      // Stuff any unmapped submission values into the batch_description field
      $lines = $infolines = array();

      // Add all of the info about the unmapped fields.
      if (count($unmapped_fields) > 0) {
        $lines[] = '--------------';
        $lines[] = '"' . strtoupper($webform_title) . '" SUBMISSION VALUES THAT WERE NOT MAPPED TO ANY BATCH RECORD FIELD.  THESE ARE ALL REPRESENTED BELOW:';
        $lines[] = '';
        
        foreach ($unmapped_fields as $cid => $label_value_arr) {
          $lines[] = $label_value_arr['webform_field_label'] . ' = "' . $label_value_arr['value'] . '"';
        }
        $mappings[] = '<div><b>REMAINING UNMAPPED SUBMISSION FIELD VALUES HAVE ALL ' .
            'BEEN ADDED TO THE DESCRIPTION</b><br>' .
            '<span class="idw_fieldname">islandora_digital_workflow_batch.batch_description</span> ' .
            '<hr><pre class="small_font left_padded">' . nl2br(implode("\n", $lines)) . '</pre></div>';
      }
      // Add a line to say who and when this was converted to a batch.
      $infolines[] = '--------------';
      $infolines[] = t('Batch created from "@webform" submission by @user at [@time]', array(
          '@user' => $user->name,
          '@webform' => $webform_title,
          '@time' => date('Y-m-d H:i:s'))
      );
      // If there is a description, append the $lines to that, else set the
      // description to what we have in $lines.
      if (array_key_exists('batch_description', $islandora_digital_workflow_batch_fields) === FALSE) {
        $islandora_digital_workflow_batch_fields['batch_description'] = implode("\n", $lines) . '
' . implode("\n", $infolines);
      }
      else {
        $islandora_digital_workflow_batch_fields['batch_description'] =
            $islandora_digital_workflow_batch_fields['batch_description'] . '
-----------------------------------------------------------------------------------------------------------------
' . implode("\n", $lines) . '
' . implode("\n", $infolines);
      }
    }
  }
  return array(
      'mappings' => $mappings,
      'islandora_digital_workflow_batch_fields' => $islandora_digital_workflow_batch_fields,
      'islandora_models_maptobatch' => $islandora_models_maptobatch,
  );
}