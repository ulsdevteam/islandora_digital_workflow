<?php

/**
 * @file
 * Create batch form and submit handling for Islandora Digital Workflow module.
 */

if (!defined('UPDATE_BUTTON_TEXT')) {
  define('UPDATE_BUTTON_TEXT', 'Update Batch');
  define('SAVE_BUTTON_TEXT', 'Save Batch');
}

/**
 * Displays the form to either create or edit a workflow batch.
 *
 * Even though the batch_name value is not passed as an argument, it can be
 * derived by using the arg() function.
 *
 * @return string
 *   HTML markup.
 */
function islandora_digital_workflow_create_batch() {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/lookup.form');
  $lookup_form_markup = theme('islandora_digital_workflow_lookup', array('searchterm' => ''));
  $batch_name = arg(3);
  $form = (!$batch_name) ?
    drupal_get_form('islandora_digital_workflow_create_empty_batch_form') :
    drupal_get_form('islandora_digital_workflow_create_batch_form', $batch_name);
  $title = 'Islandora Digital Workflow - ' . (($batch_name) ? 'Edit' : 'Create') . ' Batch';
  drupal_set_title(t($title), CHECK_PLAIN);
  $markup = $lookup_form_markup . drupal_render($form);
  return $markup;
}

/**
 * Create empty batch form for islandora_digital_workflow.
 *
 * @ingroup forms
 *
 * @todo break out non-settings elements into their own form.
 *
 * @return array
 *   The Drupal form definition.
 */
function islandora_digital_workflow_create_empty_batch_form(array $form, array &$form_state) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/access_permissions');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/taxonomy-utilities');

  $readable_models = islandora_digital_workflow_get_reduced_readable_cmodels();

  $configured_supported_models = variable_get('islandora_digital_workflow_islandora_models', array());
  $return_form_elements = array();
  $content_types_vocab = taxonomy_vocabulary_machine_name_load(IDW_CONTENT_TYPE_TAXONOMY_MACHINE_NAME);

  $accessible_models = array('' => 'Select Object Model');
  foreach ($readable_models as $cmodel => $model) {
    if (isset($configured_supported_models[$cmodel]) && $configured_supported_models[$cmodel] == 1) {
      // Now, check that the user has access to this model
      $permission = islandora_digital_workflow_permission_of_model($cmodel);
      if (user_access($permission)) {
        $tid = islandora_digital_workflow_get_tid_by_name($cmodel, $content_types_vocab);
        $accessible_models[$tid] = $cmodel;
      }
    }
  }
  $configured = TRUE;
  // Check whether or not the required configuration settings are provided.
  $batch_hosts = islandora_digital_workflow_get_batch_host_values();
  // 1. Check that the "Available Host Aliases for Batch ingest" is set
  if (count($batch_hosts) < 1) {
    drupal_set_message(t('<b>Available Host Aliases for Batch ingest</b> is ' .
        'not configured.  Please contact an administrator to ask them to ') .
        'configure at least one "' . l('Available Host Aliases for Batch ingest', '/admin/islandora/islandora_digital_workflow') .
        t('" before creating any workflow batches.'), 'error');
    $configured = FALSE;
  }

  // 2. Check that at least one model is configured
  if (count($accessible_models) < 2) {
    drupal_set_message(t('There are no <b>Supported Models</b> models configured ' . 
        'for creating batches or your user does not have permission to any models.  ' .
        'Please contact an administrator to ask them to configure the ') .
          l('Supported Models', '/admin/islandora/islandora_digital_workflow') .
        t(' and ') . l('set up permissions', '/admin/people/permissions') . t(' to any models.'), 'error');
    $configured = FALSE;
  }
  if (!$configured) {
    return array(
      'about' => array(
        '#type' => 'item',
        '#markup' => '<i>It is not possible to create any batches until the module ' .
          'has been configured and permissions granted.</i>',
      )
    );
  }

  $batch_name = '';
  $batch_description = '';
  $button_text = SAVE_BUTTON_TEXT;
  $batch_path = rtrim(variable_get('islandora_digital_workflow_ingest_prepared_path', file_directory_temp()), '/') . '/batch_0_not_named';
  $tid_for_cmodel_selected = '';

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
        '#description' => t('Human readable name for the batch of digital objects.'),
        '#default_value' => $batch_name,
      ),
      'cmodel_select' => array(
        '#title' => t('Destination Islandora Content Model'),
        '#type' => 'select',
        '#required' => true,
        '#options' => $accessible_models,
        '#description' => $supported_models_description,
        '#default_value' => $tid_for_cmodel_selected,
      ),
      'batch_description' => array(
        '#type' => 'textarea',
        '#title' => t('Batch description'),
        '#required' => true,
        '#description' => t('Description of the items in this batch.'),
        '#default_value' => $batch_description,
      ),
    );

  $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t($button_text),
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
function islandora_digital_workflow_create_empty_batch_form_submit(array $form, array &$form_state) {
  $id = $form_state['triggering_element']['#id'];
  module_load_include('inc', 'islandora_digital_workflow', 'includes/node-utilities');
  switch ($id) {
    case 'edit-submit':
      $batch_record = islandora_digital_workflow_create_stub_batch_record($form_state);
      // Only two possible button text values -- edit or save.
      $current_time = date('H:i:s');
      $edit_batch_link = l($batch_record['batch_name'], 'islandora/islandora_digital_workflow/edit_batch/' . $batch_record['batch_name']);
      drupal_set_message(t('[@date] Created new batch: ', array('@date' => $current_time)) . $edit_batch_link, 'status'); //  . date('H:i:s') . '] The new batch "' . . '" has been created.');
      drupal_goto('islandora/islandora_digital_workflow/edit_batch/' . $batch_record['batch_name']);
      break;
    case 'edit-cancel':
      drupal_set_message('Create Batch has been cancelled.');
      drupal_goto('islandora/islandora_digital_workflow/');
      break;
  }
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
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/access_permissions');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/taxonomy-utilities');
  drupal_add_js('misc/collapse.js');

  $readable_models = islandora_digital_workflow_get_reduced_readable_cmodels();
  $configured_supported_models = variable_get('islandora_digital_workflow_islandora_models', array());
  $accessible_models = $return_form_elements = array();
  $content_types_vocab = taxonomy_vocabulary_machine_name_load(IDW_CONTENT_TYPE_TAXONOMY_MACHINE_NAME);

  foreach ($readable_models as $cmodel => $model) {
    if (isset($configured_supported_models[$cmodel]) && $configured_supported_models[$cmodel] == 1) {
      // Now, check that the user has access to this model
      $permission = islandora_digital_workflow_permission_of_model($cmodel);
      if (user_access($permission)) {
        $tid = islandora_digital_workflow_get_tid_by_name($cmodel, $content_types_vocab);
        $accessible_models[$tid] = $model;
      }
    }
  }

  // If this form is processing an existing batch by batch_name, set up some
  // values
  $batch_record = islandora_digital_workflow_get_batch_record_by_name($batch_name);
  if (isset($batch_record['batch_name']) &&
      $batch_record['batch_name']) {
    $batch_name = $batch_record['batch_name'];
    $batch_description = $batch_record['batch_description'];

    $read_only_att = array('readonly' => 'readonly', 'disabled' => 'disabled');
    $button_text = UPDATE_BUTTON_TEXT;
    $batch_path = islandora_digital_workflow_batch_path($batch_record);
  }
  else {
    if (arg(2) === 'edit_batch') {
      drupal_set_message($batch_name . t(' did not exist -- redirected to Create Batch page.'));
      drupal_goto('/islandora/islandora_digital_workflow/create_batch');
    }
    $batch_name = '';
    $batch_description = '';
    $read_only_att = array();
    $button_text = SAVE_BUTTON_TEXT;
    $batch_path = rtrim(variable_get('islandora_digital_workflow_ingest_prepared_path', file_directory_temp()), '/') . '/batch_0_not_named';
  }
  if (!(array_key_exists($batch_record['default_type_of_resource'], $readable_models) === FALSE)) {
    $cmodel = $batch_record['default_type_of_resource'];
    $tid_for_cmodel_selected = islandora_digital_workflow_get_tid_by_name($cmodel, $content_types_vocab);
  }
  else {
    $tid_for_cmodel_selected = '';
  }
  $model_name = islandora_digital_workflow_get_name_of_tid($tid_for_cmodel_selected);
  $file_not_allowed_for_model_names = array('Finding Aid', 'Newspaper');

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
            'will not be able to ingest or process.  Please replace the attached ' .
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
  $cmodel_readonly = $read_only_att;
  $cmodel_readonly['autocomplete'] = 'off';
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
        '#required' => true,
        '#options' => $accessible_models,
        '#attributes' => $cmodel_readonly,
        '#description' => $supported_models_description,
        '#default_value' => $tid_for_cmodel_selected,
      ),
      'batch_description' => array(
        '#type' => 'textarea',
        '#title' => t('Batch description'),
        '#required' => true,
        '#description' => t('Description of the items in this batch.'),
        '#default_value' => $batch_description,
      ),

      // Is batch active? checkbox
      'is_batch_active' => array(
        '#type' => 'checkbox',
        '#title' => t('Active?'),
        '#default_value' => ($batch_record && isset($batch_record['is_batch_active'])) ? $batch_record['is_batch_active'] : 1,
      ),

      // Is a request? checkbox
      'batch_request' => array(
        '#title' => 'Batch Request?',
        '#type' => 'fieldset',
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        'is_batch_request' => array(
          '#type' => 'checkbox',
          '#title' => t('Is a Request?'),
          '#default_value' => ($batch_record && isset($batch_record['is_batch_request'])) ? $batch_record['is_batch_request'] : '',
        ),
        // Requestor: textfield
        'batch_requestor' => array(
          '#type' => 'textfield',
          '#title' => t('Requestor'),
          '#default_value' => ($batch_record && isset($batch_record['batch_requestor'])) ? $batch_record['batch_requestor'] : '',
        ),
        // Request due date: textfield
        'batch_request_due_date' => array(
          '#type' => 'date',
          '#default_value' => array(
            'month' => format_date((($batch_record && isset($batch_record['batch_request_due_date'])) ? $batch_record['batch_request_due_date'] : time()), 'custom', 'n'),
            'day' => format_date((($batch_record && isset($batch_record['batch_request_due_date'])) ? $batch_record['batch_request_due_date'] : time()), 'custom', 'j'),
            'year' => format_date((($batch_record && isset($batch_record['batch_request_due_date'])) ? $batch_record['batch_request_due_date'] : time()), 'custom', 'Y'),
           ),
          '#title' => t('Request Due Date'),
        ),
      ),
    );

  // Objects frame
  $form['batch_objects_frame'] = array(
      '#type' => 'fieldset',
      '#title' => 'Batch objects',
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    );

  // Default EAD id: textfield
  $form['batch_objects_frame']['default_ead_id'] = array(
      '#type' => 'textfield',
      '#title' => t('Default EAD_id'),
      '#default_value' => ($batch_record && isset($batch_record['default_ead_id'])) ? $batch_record['default_ead_id'] : '',
    );
  $form['batch_objects_frame']['object_count'] = array(
      '#title' => t('How many physical objects?'),
      '#type' => 'textfield',
      '#required' => true,
      '#attributes' => array(
          ' type' => 'number', // insert space before attribute name :)
      ),
      '#default_value' => ($batch_record && isset($batch_record['object_count'])) ? $batch_record['object_count'] : 1,
    );

  if ((array_search($model_name, $file_not_allowed_for_model_names) === FALSE)) {
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
        '#suffix' => (count($file_rows) ? '<div class="fieldset_div_wrapper"><div class="report_table_wrapper">' .
            theme('table', array('rows' => $file_rows, 'header' => $file_header, 'attributes' => array('class' => array('report_table')))) .
          '</div></div>'  : ''),
      );
  }

  // Ingest options frame
  $form['ingest_options_frame'] = array(
      '#type' => 'fieldset',
      '#title' => 'Ingest options',
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    );
  // Requestor: textfield
  $form['ingest_options_frame']['batch_host'] = array(
      '#type' => 'select',
      '#title' => t('Ingest files from host'),
      '#required' => true,
      '#options' => $batch_hosts,
      '#default_value' => $current_batch_host_selection,
      '#description' => t('Specify the location of the files that relate to the uploaded spreadsheet.'),
    );
  $form['ingest_options_frame']['ingest_namespace'] = array(
      'ingest_namespace' => array(
        '#type' => 'textfield',
        '#title' => t('Ingest namespace'),
        '#required' => true,
        '#default_value' => ($batch_record && isset($batch_record['ingest_namespace'])) ? $batch_record['ingest_namespace'] : variable_get('islandora_digital_workflow_ingest_namespace', 'islandora'),
      ),
    );
  $form['ingest_options_frame']['collections_select'] = array(
      '#title' => t('Collections'),
      '#type' => 'select',
      '#multiple' => TRUE,
      '#size' => 10,
      '#default_value' => ($batch_record && isset($batch_record['mapto_collections'])) ? explode(",", $batch_record['mapto_collections']) : array(),
      '#attributes' => array('autocomplete' => 'off'),
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
        '#default_value' => ($batch_record && isset($batch_record['mapto_site_id_values'])) ? explode(",", $batch_record['mapto_site_id_values']) : array(),
        '#attributes' => array('autocomplete' => 'off'),
      );
  }

  // Batch defaults frame
  $form['batch_defaults_frame'] = array(
      '#type' => 'fieldset',
      '#title' => 'Batch defaults',
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    );

  // Default depositor: textfield
  $form['batch_defaults_frame']['default_depositor'] = array(
        '#type' => 'textfield',
        '#description' => 'This value will only be used if the spreadsheet MODS does not contain a value for the depositor.',
        '#title' => t('Default Depositor'),
        '#default_value' => ($batch_record && isset($batch_record['default_depositor'])) ? $batch_record['default_depositor'] : '',
      );

  // Default genre: textfield
  $form['batch_defaults_frame']['default_genre'] = array(
      '#type' => 'textfield',
      '#title' => t('Default Genre'),
      '#default_value' => ($batch_record && isset($batch_record['default_genre'])) ? $batch_record['default_genre'] : '',
    );
  
  // Default copyright status: select (hard-coded options)
  $form['batch_defaults_frame']['batch_default_CR_status'] = array(
      '#type' => 'select',
      '#title' => t('Default Copyright Status'),
      '#options' => array(''=>'Select Default Copyright Status',
        'copyrighted' => 'copyrighted',
        'pd' => 'public domain',
        'pd_usfed' => 'public domain - us federal document',
        'pd_holder' => 'public domain - dedicated by rights holder',
        'pd_expired' => 'public domain - expired copyright',
        'unknown' => 'unknown',
      ),
      '#default_value' => ($batch_record && isset($batch_record['batch_default_CR_status'])) ? $batch_record['batch_default_CR_status'] : '',
    );
  // Default publication status: select (hard-coded options)
  $form['batch_defaults_frame']['batch_default_pub_status'] = array(
      '#type' => 'select',
      '#title' => t('Default Publication Status'),
      '#options' => array(''=>'Select Default Publication Status',
        'published' => 'published',
        'unpublished' => 'unpublished',
        'unknown' => 'unknown',
      ),
      '#default_value' => ($batch_record && isset($batch_record['batch_default_pub_status'])) ? $batch_record['batch_default_pub_status'] : '',
    );
  // Default copyright holder name: textarea
  $form['batch_defaults_frame']['batch_default_CR_holder'] = array(
      '#type' => 'textarea',
      '#title' => t('Default Copyright Holder Name'),
      '#default_value' => ($batch_record && isset($batch_record['batch_default_CR_holder'])) ? $batch_record['batch_default_CR_holder'] : '',
    );

  islandora_digital_workflow_remove_model_based_form_elements($model_name, $form);
  $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t($button_text),
    );
  $form['cancel'] = array(
      '#type' => 'submit',
      '#value' => t('Cancel'),
    );
  return $form;
}

/**
 * This will remove elements from the default "create batch" form based on the
 * Islandora model.
 *
 * @param string $model_name
 *   The name of the Islandora model for which the form is being generated.
 * @param array $form
 *   Drupal form representation as an array.
 *
 */
function islandora_digital_workflow_remove_model_based_form_elements($model_name, &$form) {
  switch ($model_name) {
    case 'Large Image' || 'Basic Image' || 'Image':
      unset($form['batch_objects_frame']['default_ead_id']);
      break;

    case 'Finding Aid':
      unset($form['batch_objects_frame']['default_ead_id']);
      unset($form['batch_objects_frame']['file']);
      break;

    case 'Internet Archive Book' || 'Book':
      unset($form['batch_objects_frame']['default_ead_id']);
      break;

    case 'Manuscript':
      break;

    case 'Newspaper':
      unset($form['batch_objects_frame']['file']);
      unset($form['batch_objects_frame']['default_ead_id']);
      break;

    case 'Newspaper Issue' | 'Newspaper/Serial':
      unset($form['batch_objects_frame']['default_ead_id']);
      break;

    case 'Web ARChive':
      unset($form['batch_objects_frame']['default_ead_id']);
      break;

    default:
      break;
  }
}

/**
 * Validates the form input.
 *
 * Checks the batch_name to see if it exists -- displays a link to that batch
 * in the event that the user wanted to view or edit that batch instead.
 *
 * @param array $elements
 *   Drupal elements representation as an array.
 * @param array $form_state
 *   Drupal form_state representation as passed to the validate function. *
 * @param string $form_id
 *   The form_id that is being validated -- optional.
 */
function islandora_digital_workflow_create_empty_batch_form_validate(&$elements, &$form_state, $form_id = NULL) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
  // Check for the batch name already existing.
  $batch_name = $form_state['values']['batch_name'];
  $batch_record = islandora_digital_workflow_get_batch_record_by_name($batch_name);
  if (is_array($batch_record)) {
    form_set_error('batch_name', t('A batch already exists with the name @batch_name.',
        array('@batch_name' => $batch_name)));
    $edit_this_batch_link = l($batch_name, 'islandora/islandora_digital_workflow/edit_batch/' . $batch_name);
    drupal_set_message('Edit existing batch "' . $edit_this_batch_link . '".', 'warning');
  }
}

/**
 * Validates the form input.
 *
 * Checks the uploaded_csv_file file to determine whether or not it is valid.
 *
 * @param array $elements
 *   Drupal elements representation as an array.
 * @param array $form_state
 *   Drupal form_state representation as passed to the validate function. *
 * @param string $form_id
 *   The form_id that is being validated -- optional.
 */
function islandora_digital_workflow_create_batch_form_validate(&$elements, &$form_state, $form_id = NULL) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
  $batch_name = $form_state['values']['batch_name'];
  if (isset($_FILES['files']) && array_key_exists('tmp_name', $_FILES) && isset($_FILES['tmp_name'])) {
    $batch_record = islandora_digital_workflow_get_batch_record_by_name($batch_name);
    $batch_record_uploaded_csv = array_key_exists('uploaded_csv_file', $batch_record) ? $batch_record['uploaded_csv_file'] : '';
    if ($batch_record_uploaded_csv == '') {
      // Check the csv file upload.
      if (!islandora_digital_workflow_validate_csv_file_upload()) {
        form_set_error('file', t('The uploaded CSV file is not valid.'));
      }
    }
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
      if (isset($_FILES['files']) && isset($_FILES["files"]["tmp_name"]['file'])) {
        islandora_digital_workflow_validate_csv_file_upload();
      }
      $csv_filename = (isset($_SESSION['uploaded_csv_filename']) ? $_SESSION['uploaded_csv_filename'] : '');
      $create_new = ($form_state['clicked_button']['#value'] === SAVE_BUTTON_TEXT);
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

/**
 * Helper function to validate the uploaded_csv_file.
 *
 * This operates on the current $_FILES -- so it can only be called on form post
 * and potentially sets a $_SESSION variable for use in the submit function,
 * see: islandora_digital_workflow_create_batch_form_submit() above.
 *
 * @return boolean
 */
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
