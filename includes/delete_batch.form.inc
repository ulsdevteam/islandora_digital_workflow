<?php

/**
 * @file
 * Delete batch form and submit handling for Islandora Digital Workflow module.
 */

/**
 * The Drupal delete batch form representation.
 *
 * @return array
 *   The Drupal form definition.
 */
function islandora_digital_workflow_delete_batch() {
  $batch_name = arg(3);
  $form = drupal_get_form('islandora_digital_workflow_delete_batch_form', $batch_name);
  $markup = drupal_render($form);
  return $markup;
}

/**
 * Create batch form for islandora_digital_workflow.
 *
 * @ingroup forms
 *
 * @todo break out non-settings elements into their own form.
 *
 * @return array
 *   The Drupal form definition.
 */
function islandora_digital_workflow_delete_batch_form(array $form, array &$form_state, $batch_name = NULL) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/access_permissions');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/taxonomy-utilities');
  drupal_add_js('misc/collapse.js');

  $readable_models = islandora_digital_workflow_get_reduced_readable_cmodels();
  $configured_supported_models = variable_get('islandora_digital_workflow_islandora_models', array());
  $accessible_models = $return_form_elements = array();
  $content_types_vocab = taxonomy_vocabulary_machine_name_load(IDW_CONTENT_TYPE_TAXONOMY_MACHINE_NAME);

  // If this form is processing an existing batch by batch_name, set up some
  // values
  $batch_record = islandora_digital_workflow_get_batch_record_by_name($batch_name);
  if (isset($batch_record['batch_name']) &&
      $batch_record['batch_name']) {
    $batch_name = $batch_record['batch_name'];
    $batch_description = $batch_record['batch_description'];

    $read_only_att = array('readonly' => 'readonly', 'disabled' => 'disabled');
    $button_text = '';
    $batch_path = islandora_digital_workflow_batch_path($batch_record);
  }
  else {
    drupal_set_message('That is strange.  The batch record by that name does not exist and can not be deleted.');
    drupal_goto('islandora/islandora_digital_workflow/');
  }
  if (!(array_search($batch_record['default_type_of_resource'], $readable_models) === FALSE)) {
    $cmodel = array_search($batch_record['default_type_of_resource'], $readable_models);
    $tid_for_cmodel_selected = islandora_digital_workflow_get_tid_by_name($cmodel, $content_types_vocab);
  }
  else {
    $tid_for_cmodel_selected = '';
  }
  $model_name = islandora_digital_workflow_get_name_of_tid($tid_for_cmodel_selected);
  $file_not_allowed_for_model_names = array('islandora:findingAidCModel', 'islandora:newspaperCModel', 'islandora:newspaperIssueCModel');

  $current_batch_host_selection = ($batch_record && isset($batch_record['batch_host'])) ? $batch_record['batch_host'] : '';
  $batch_hosts = islandora_digital_workflow_get_batch_host_values();

  $file_rows = $file_header = array();
  $tmp_uploaded_csv_file = drupal_realpath(drupal_tempnam('temporary://', 'batch_objects.csv'));
  $temp_csv_file = ($batch_record['uploaded_csv_file']) ? file_put_contents($tmp_uploaded_csv_file, $batch_record['uploaded_csv_file']) : '';
  if ($temp_csv_file && file_exists($tmp_uploaded_csv_file)) {
    $arr = islandora_digital_workflow_csv_file_as_array($tmp_uploaded_csv_file);
    $file_header = (isset($arr['header']) ? $arr['header'] : array());
    $file_rows = (isset($arr['rows']) ? $arr['rows'] : array());
    $csv_header_mappings = islandora_digital_workflow_get_csv_header_xpath_mappings();
    foreach ($file_header as $header) {
      $machine_header = str_replace(" ", "_", strtolower(trim($header)));
      if ($machine_header && !(array_key_exists($machine_header, $csv_header_mappings))) {
        drupal_set_message('CSV column "' . $header . '" not recognized.  Batch ' . 
            'may not be able to ingest or process.  Please replace the attached ' .
            'CSV file.', 'error');
      }
    }
    @unlink($tmp_uploaded_csv_file);
  }
  // Prepare a couple variables for use with file upload options:
  $upload_size = min((int) ini_get('post_max_size'), (int) ini_get('upload_max_filesize'));
  $extensions = array('csv');

  // For legacy batches, this array may need to be joined with the legacy
  // collection `c_id` pointers instead of Solr PID values.
  $collections = islandora_digital_workflow_get_solr_options('RELS_EXT_hasModel_uri_ms:info' .
      '\:fedora\/islandora\:collectionCModel -PID:*_review', 'PID', 'fgs_label_s');
  asort($collections);

  // If the user can administer site config, then display the link to the 
  // supported models here.
  //
  // admin/islandora/islandora_digital_workflow
  $supported_models_description = 'Select the type of objects from the ' .
      'available content models that this batch will contain.' .
      (user_access('administer site configuration') ?
          '  Configuration for ' . l(t('Islandora Digital Workflow'),
          'admin/islandora/islandora_digital_workflow',
          array('attributes'=>array('target' => '_blank'))) . '.  ' : '');

  $form['batch_edit_frame'] = array(
      '#type' => 'fieldset',
      '#title' => 'Batch details',
      'batch_name' => array(
        '#type' => 'textfield',
        '#title' => t('Batch name'),
        '#required' => true,
        '#attributes' => $read_only_att,
        '#description' => t('Human readable name for the batch of digital objects.'),
        '#default_value' => $batch_name,
      ),
      'cmodel_select' => array(
        '#title' => t('Destination Islandora Content Model'),
        '#type' => 'select',
        '#options' => $accessible_models,
        '#attributes' => $read_only_att,
        '#description' => $supported_models_description,
        '#default_value' => $tid_for_cmodel_selected,
      ),
      'batch_description' => array(
        '#type' => 'textarea',
        '#title' => t('Batch description'),
        '#attributes' => $read_only_att,
        '#description' => t('Description of the items in this batch.'),
        '#default_value' => $batch_description,
      ),

      // Is batch active? checkbox
      'is_batch_active' => array(
        '#type' => 'checkbox',
        '#title' => t('Active?'),
        '#attributes' => $read_only_att,
        '#default_value' => ($batch_record && isset($batch_record['is_batch_active'])) ? $batch_record['is_batch_active'] : 1,
      ),
    );

  $form['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
    );
  $form['cancel'] = array(
      '#type' => 'submit',
      '#value' => t('Cancel'),
    );
  return $form;
}

/**
 * Function that sets the Drupal variables with user's input.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_digital_workflow_delete_batch_form_submit(array $form, array &$form_state) {
  $id = $form_state['triggering_element']['#id'];
  module_load_include('inc', 'islandora_digital_workflow', 'includes/node-utilities');
  switch ($id) {
    case 'edit-submit':
      $batch_record = islandora_digital_workflow_sync_node_batch_record_info(NULL, $form_state, $csv_filename);
      // Only two possible button text values -- edit or save.
      $current_time = date('H:i:s');
      $edit_batch_link = l($batch_record['batch_name'], 'islandora/islandora_digital_workflow/edit_batch/' . $batch_record['batch_name']);
      if ($create_new) {
        drupal_set_message(t('[@date] Created new batch: ', array('@date' => $current_time)) . $edit_batch_link, 'status'); //  . date('H:i:s') . '] The new batch "' . . '" has been created.');
      }
      else {
        drupal_set_message(t('[@date] Batch updated: ', array('@date' => $current_time)) . $edit_batch_link, 'status'); //  . date('H:i:s') . '] The batch record has been updated.');
      }
      if (file_exists($csv_filename)) {
        @unlink($csv_filename);
      }
      drupal_goto('node/' . $batch_record['nid']);
      break;
    case 'edit-cancel':
      drupal_set_message('Create Batch has been cancelled.');
      drupal_goto('islandora/islandora_digital_workflow/');
      break;
  }
}