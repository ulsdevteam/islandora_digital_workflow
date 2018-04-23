<?php

/**
 * @file
 * Create batch form and submit handling for Islandora Digital Workflow module.
 */

function islandora_digital_workflow_create_batch() {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/lookup.form');
  $markup = theme('islandora_digital_workflow_lookup', array('searchterm' => ''));
  $form = drupal_get_form('islandora_digital_workflow_create_batch_form');
  $markup .= drupal_render($form);
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
function islandora_digital_workflow_create_batch_form(array $form, array &$form_state, $batch_name = NULL) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/access_permissions');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/taxonomy-utilities');

  $readable_models = islandora_digital_workflow_get_reduced_readable_cmodels();
  $configured_supported_models = variable_get('islandora_digital_workflow_islandora_models', array());
  $accessible_models = $return_form_elements = array();
  $content_types_vocab = taxonomy_vocabulary_machine_name_load(IDW_CONTENT_TYPE_TAXONOMY_MACHINE_NAME);

  foreach ($readable_models as $cmodel => $model) {
    if (isset($configured_supported_models[$cmodel]) && $configured_supported_models[$cmodel] == 1) {
      // Now, check that the user has access to this model
      $permission = islandora_digital_workflow_permission_of_model($model);
      if (user_access($permission)) {
        $tid = islandora_digital_workflow_get_tid_by_name($model, $content_types_vocab);
        $accessible_models[$tid] = $model;
      }
    }
  }
  if (count($accessible_models) < 1) {
    drupal_set_message('You do not have access to create batches for any ' .
        'Islandora Object models.  Please contact an administrator to ask them ' .
        'to update permissions if your user or user role needs to have access ' .
        'to any models.', 'error');
  }

  // If this form is processing an existing batch by batch_name, set up some
  // values
  $batch_record = islandora_digital_workflow_get_batch_record_by_name($batch_name);
  if (isset($batch_record['batch_external_id']) && 
      $batch_record['batch_external_id']) {
    $batch_name = $batch_record['batch_external_id'];
    $batch_description = $batch_record['batch_description'];
    $read_only_att = array('readonly' => 'readonly', 'disabled' => 'disabled');
    $save_button_text = 'Update Batch';
    $batch_path = islandora_digital_workflow_batch_path($batch_record);
  }
  else {
    $batch_name = '';
    $batch_description = '';
    $read_only_att = array();
    $save_button_text = 'Save Batch';
    $batch_path = rtrim(variable_get('islandora_digital_workflow_ingest_prepared_path', '/ingest/islandora_ingest'), '/') . '/batch_0_not_named';
  }

  $file_rows = $file_header = array();
  if (file_exists($batch_record['file'])) {
    $arr = islandora_digital_workflow_csv_file_as_array($batch_record['file']);
    $file_header = (isset($arr['header']) ? $arr['header'] : array());
    $file_rows = (isset($arr['rows']) ? $arr['rows'] : array());
    $csv_header_mappings = islandora_digital_workflow_get_csv_header_xpath_mappings();
    foreach ($file_header as $header) {
      $machine_header = str_replace(" ", "_", strtolower(trim($header)));
      if ($machine_header && !(array_key_exists($machine_header, $csv_header_mappings))) {
        drupal_set_message('CSV column "' . $header . '" not recognized.  Batch ' . 
            'will not be able to ingest or process.  Please replace the attached ' .
            'CSV file "' . $batch_record['file'] . '".', 'error');
      }
    }
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
          '  Configuration for ' . l('Islandora Digital Workflow',
          'admin/islandora/islandora_digital_workflow',
          array('attributes'=>array('target' => '_blank'))) . '.  ' : '');

  $form = array('about' => array(
      '#type' => 'item',
      '#markup' => '<i>A batch should <strong>ALWAYS</strong> be a group of similar ' .
        'objects that need to be tracked through the workflow together.  They are ' .
        'usually added to the <strong>same collection</strong>, <strong>use the ' .
        'same object model</strong>, and usually have many of the <strong>same ' .
        'properties</strong>.</i>',
    ));
  $form['batch_edit_frame'] = array(
      '#type' => 'fieldset',
      '#title' => 'Create Batch',
      'batch_name' => array(
        '#type' => 'textfield',
        '#title' => t('Batch name'),
        '#required' => true,
        '#attributes' => $read_only_att,
        '#description' => t('Human readable name for the batch of digital objects.'),
        '#default_value' => $batch_name,
      ),
      'batch_description' => array(
        '#type' => 'textarea',
        '#title' => t('Batch description'),
        '#required' => true,
        '#description' => t('Description of the items in this batch.'),
        '#default_value' => $batch_description,
      ),
    );

  // Objects frame
  $form['batch_objects_frame'] = array(
      '#type' => 'fieldset',
      '#title' => 'Batch objects');
  $form['batch_objects_frame']['object_count'] = array(
      '#title' => t('How many physical objects?'),
      '#type' => 'textfield',
      '#required' => true,
      '#attributes' => array(
          ' type' => 'number', // insert space before attribute name :)
      ),
      '#default_value' => 1,
    );
  $form['batch_objects_frame']['file'] = array(
      '#type' => 'file',
      '#title' => t('File'),
      '#description' => t('Select CSV file to upload.<br/>Files must be less than <strong>@size MB.</strong><br/>Allowed file types: <strong>@ext.</strong>', array('@size' => $upload_size, '@ext' => $extensions[0])) .
                        (($batch_record && isset($batch_record['file']) && ($batch_record['file'] <> '')) ? '<br>Upload file will be stored: <em>' . str_replace($batch_path, '', $batch_record['file']) . '</em>' : ''),
      '#upload_location' => 'temporary://',
      '#upload_validators' => array(
        'file_validate_extensions' => $extensions,
         // Assume its specified in MB.
        'file_validate_size' => array($upload_size * 1024 * 1024),
      ),
      '#prefix' => "<a id='file_upload'> </a>",
      '#suffix' => (count($file_rows) ? '<div class="report_table_wrapper">' .
          theme('table', array('rows' => $file_rows, 'header' => $file_header, 'attributes' => array('class' => array('report_table')))) .
        '</div>'  : ''),
    );

  // Ingest options frame
  $form['ingest_options_frame'] = array(
      '#type' => 'fieldset',
      '#title' => 'Ingest options');
  $form['ingest_options_frame']['ingest_namespace'] = array(
      'ingest_namespace' => array(
        '#type' => 'textfield',
        '#title' => t('Ingest namespace'),
        '#required' => true,
        '#attributes' => $read_only_att,
        '#default_value' => variable_get('islandora_digital_workflow_ingest_namespace', 'islandora'),
      ),
    );
  $form['ingest_options_frame']['cmodel_select'] = array(
      '#title' => t('Destination Islandora Content Model'),
      '#type' => 'select',
      '#required' => true,
      '#options' => $accessible_models,
      '#description' => $supported_models_description,
    );
  $form['ingest_options_frame']['collections_select'] = array(
      '#title' => t('Collections'),
      '#type' => 'select',
      '#multiple' => TRUE,
      '#size' => 10,
      '#options' => $collections,
    );

  // This is a value that is only only used when configured.
  if (variable_get('islandora_digital_workflow_use_isMemberOfSite', 0)) {
    $sites = islandora_digital_workflow_get_sites_options();
    $form['ingest_options_frame']['sites_select'] = array(
        '#title' => t('Sites'),
        '#type' => 'select',
        '#multiple' => TRUE,
        '#options' => $sites,
      );
  }

  $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t($save_button_text),
    );
  $form['cancel'] = array(
      '#type' => 'submit',
      '#value' => t('Cancel'),
    );
  return $form;
}

function islandora_digital_workflow_create_batch_form_validate(&$elements, &$form_state, $form_id = NULL) {
  // Check the csv file upload.
  if (!islandora_digital_workflow_validate_csv_file_upload()) {
    form_set_error('file', t('The uploaded CSV file is not valid.'));
    return;
  }
  $batch_name = $form_state['values']['batch_name'];
  $batch_record = islandora_digital_workflow_get_batch_record_by_name($batch_name);
  if (is_array($batch_record)) {
    form_set_error('batch_name', t('A batch already exists with the name @batch_name.',
        array('@batch_name' => $batch_name)));

  }
}

/**
 * Function that sets the Drupal variables with user's input.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_digital_workflow_create_batch_form_submit(array $form, array &$form_state) {
  $id = $form_state['triggering_element']['#id'];
  module_load_include('inc', 'islandora_digital_workflow', 'includes/node-utilities');
  switch ($id) {
    case 'edit-submit':
      $csv_filename = (isset($_SESSION['uploaded_csv_filename']) ? $_SESSION['uploaded_csv_filename'] : '');
      $batch_record = islandora_digital_workflow_create_node_from_form_data($form_state, $csv_filename);
      drupal_set_message('[' . date('H:i:s') . '] The new batch has been created.');
      drupal_goto('islandora/islandora_digital_workflow');
      break;
    case 'edit-cancel':
      drupal_set_message('Create Batch has been cancelled.');
      drupal_goto('islandora/islandora_digital_workflow/');
      break;
  }
}

// This operates on the current $_FILES -- so it can only be called on form post.
function islandora_digital_workflow_validate_csv_file_upload() {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  $valid_upload = FALSE;

  if (isset($_FILES['files']) && isset($_FILES["files"]["tmp_name"]['file'])) {
    $dest = drupal_tempnam('temporary://', 'batch_objects.csv');
    if ($_FILES['files']['error']['file'] == UPLOAD_ERR_OK) {
      $tmp_name = $_FILES["files"]["tmp_name"]['file'];
      if (move_uploaded_file($tmp_name, $dest)) {
        $valid_upload = islandora_digital_workflow_validate_csv($dest);
        $_SESSION['uploaded_csv_filename'] = $dest;
      }
      else {
        drupal_set_message(t('File move operation from "' . $tmp_name . '" to "' . $dest . '" failed'), 'error');
      }
    }
  }
  return $valid_upload;
}
