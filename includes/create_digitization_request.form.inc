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

/**
 * Returns only the configuratoin fields related to Webform submissions that are
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
      $form_components = islandora_digitization_requests_webform_components($nid);

      // Search fields
      foreach ($form_components as $cid => $components_arr) {
        $form_elements['webform_options']['webform_' . $nid]['webform_' . $nid . '_searchfields_wrapper']['webform_' . $nid . '_searchfields_' . $cid] = array(
          '#type' => 'checkbox',
          '#title' => $components_arr['name'],
          '#default_value' => ((array_key_exists($nid, $webform_options) && array_key_exists('search_fields', $webform_options[$nid])) ? $webform_options[$nid]['search_fields'][$cid]: 0),
        );
      }

      $allowed_field_types_for_text_mapping = array('textarea', 'textfield');
      $map_dr_text_options = $map_dr_numeric_options = array('' => 'Select field');
      foreach ($form_components as $cid => $components_arr) {
        if ((!(array_search($components_arr['type'], $allowed_field_types_for_text_mapping) === FALSE))) {
          $map_dr_text_options[$cid] = $components_arr['name'];
        }
        else {
          $map_dr_numeric_options[$cid] = $components_arr['name'];
        }
      }

      // Mappings (string fields)
      $form_elements['webform_options']['webform_' . $nid]['webform_' . $nid . '_mappings_wrapper']['stringfields'] = array(
          '#type' => 'fieldset',
          '#prefix' => '<b>Text batch record fields</b>',
          '#attributes' => array('class' => array(
            'multi_input_line'
          )),
      );
      $toggle = FALSE;
      foreach ($mapping_to_fieldnames_for_textfields as $fieldname => $value) {
        $form_elements['webform_options']['webform_' . $nid]['webform_' . $nid . '_mappings_wrapper']['stringfields']['webform_' . $nid . '_mappings_' . $fieldname] = array(
          '#type' => 'select',
          '#title' => $value,
          '#options' => $map_dr_text_options,
          '#default_value' => ((array_key_exists($nid, $webform_options) && array_key_exists('mapped_fields', $webform_options[$nid]) &&
                array_key_exists($fieldname, $webform_options[$nid]['mapped_fields'])) ?
                $webform_options[$nid]['mapped_fields'][$fieldname]: ''),
          '#attributes' => array('class' => array(($toggle) ? 'odd_child' : 'even_child')),
        );
        @list($field_description_only, $junk) = explode(" (", strip_tags($value));
        $form_elements['webform_options']['webform_' . $nid]['webform_' . $nid . '_mappings_wrapper']['stringfields']['webform_' . $nid . '_default_' . $fieldname] = array(
          '#type' => 'textfield',
          '#title' => '<i class="disabled_text">default value</i>',
          '#default_value' => '',
          '#suffix' => '<br class="break_float">',
          '#attributes' => array(
              'title' => 'When there is no mapping, provide a default value for the batch record\'s "' . $field_description_only . '"',
              'class' => array(($toggle) ? 'odd_child' : 'even_child')),
          '#default_value' => ((array_key_exists($nid, $webform_options) && array_key_exists('default_values_fields', $webform_options[$nid]) &&
                array_key_exists($fieldname, $webform_options[$nid]['default_values_fields'])) ?
                $webform_options[$nid]['default_values_fields'][$fieldname]: ''),
        );
        $toggle = !$toggle;
      }
      // Mappings (numeric fields)
      $form_elements['webform_options']['webform_' . $nid]['webform_' . $nid . '_mappings_wrapper']['numericfields'] = array(
          '#type' => 'fieldset',
          '#prefix' => '<b>Numeric batch record fields</b>',
          '#attributes' => array('class' => array(
            'multi_input_line'
          )),
      );
      $toggle = FALSE;
      foreach ($mapping_to_fieldnames_for_intfields as $fieldname => $value) {
        $form_elements['webform_options']['webform_' . $nid]['webform_' . $nid . '_mappings_wrapper']['numericfields']['webform_' . $nid . '_mappings_' . $fieldname] = array(
          '#type' => 'select',
          '#title' => $value,
          '#options' => $map_dr_numeric_options,
          '#default_value' => ((array_key_exists($nid, $webform_options) && array_key_exists('mapped_fields', $webform_options[$nid]) &&
                array_key_exists($fieldname, $webform_options[$nid]['mapped_fields'])) ?
                $webform_options[$nid]['mapped_fields'][$fieldname]: ''),
          '#attributes' => array('class' => array(($toggle) ? 'odd_child' : 'even_child')),
        );
        @list($field_description_only, $junk) = explode(" (", strip_tags($value));
        $form_elements['webform_options']['webform_' . $nid]['webform_' . $nid . '_mappings_wrapper']['numericfields']['webform_' . $nid . '_default_' . $fieldname] = array(
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

  $never_map_fieldnames = array('batch_name', 'uploaded_batch_items_file', 'nid', 'workflow_sequence_id', 'user', 'lastmod');
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
  // Take the submission record and process that according to the configuration
  // settings for the $digitization_request_options[$nid]['mapped_fields']
  $nid = arg(3);
  $sid = arg(4);

  $results = islandora_digital_workflow_map_submission($nid, $sid);
  $mappings = $results['mappings'];
  $islandora_digital_workflow_batch_fields = $results['islandora_digital_workflow_batch_fields'];
  $confirm_mapping_form = array();
  $confirm_mapping_form['nothing'] = array(
    '#type' => 'item',
    '#markup' => 
      ((count($mappings) > 0) ?
        '<div class="big_font bad">' .
          t('<b>THIS CAN NOT BE UNDONE!</b><br>Are you SURE that you want to ' .
            'map this digitization request into a batch record?') .
      '</div><h4>Mappings for this request:</h4><div class="indent-item">' . implode("<br>", $mappings) . '</div>' : '<b>There were no fields in the ' .
        'digitization request that can map to any batch record fields.</b>')
  );

  $confirm_mapping_form['batch_name'] = array(
    '#type' => 'textfield',
    '#title' => 'Batch name',
    '#required' => TRUE,
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
  $confirm_mapping_form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Create batch record'),
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
  if (is_array($batch_record)) {
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
    drupal_set_message('Mapping to an islandora_digital_workflow_batch record has been canceled.');
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
 * @return type
 */
function islandora_digital_workflow_map_submission($nid, $sid) {
  global $user;

  $submission_data = islandora_digital_requests_get_webform_submission($sid);
  // Use the configuration to set field values for a islandora_digital_workflow_batch
  // record based on the field values for the given submission.
  $webform_options = variable_get('islandora_digital_workflow_digitization_requests', array());
  $mappings = $unmapped_fields = array();
  $components = islandora_digitization_requests_webform_components($nid);
  $islandora_digital_workflow_batch_fields = array();
  foreach ($webform_options as $options_nid => $options) {
    if ($nid == $options_nid) {
      $webform_node = node_load($nid);
      $webform_title = (is_object($webform_node) ? $webform_node->title :
          t('(unknown webform with nid = @nid)', array('@nid' => $nid)));
      // Set up an array to keep track of submitted data that does not get mapped
      foreach ($components as $cid => $webform_field) {
        if (array_key_exists($cid, $submission_data) && $submission_data[$cid]['data']) {
          $unmapped_fields[$cid] = array(
              'webform_field_label' => strtoupper($webform_field['name']),
              'value' => $submission_data[$cid]['data'],
            );
        }
      }

      foreach ($options['mapped_fields'] as $fieldname => $setting) {
        if ($setting) {
          if (array_key_exists($setting, $submission_data)) {
            $file_truncated_ellipsis = (strlen($submission_data[$setting]['data']) > 600) ? ' ... ' : '';
            $mappings[] = '<div><b>' . $components[$setting]['name'] . '</b> to ' .
                '<span class="idw_fieldname">islandora_digital_workflow_batch.' . $fieldname . '</span><br>' .
                '<span class="small_font">' . substr($submission_data[$setting]['data'], 0, 600) .
                $file_truncated_ellipsis . '</span></div>';
            $islandora_digital_workflow_batch_fields[$fieldname] = $submission_data[$setting]['data'];
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
      // NOTE: the array starts at 1 instead of 0.
      if (count($submission_data) > 0) {
        $islandora_digital_workflow_batch_fields['user'] = $submission_data[1]['drupal_username'];
        $mappings[] = '<div>"<b>' . $submission_data[1]['drupal_username'] . '</b>" to ' .
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
  );
}