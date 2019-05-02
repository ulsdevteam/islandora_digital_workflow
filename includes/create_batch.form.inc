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
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
  $lookup_form_markup = theme('islandora_digital_workflow_lookup', array('searchterm' => ''));
  $batch_name = arg(3);

  $breadcrumb = array(
    l(t('Home'), '<front>'),
    l(t('Islandora Digital Workflow'), 'islandora/islandora_digital_workflow'),
    ((!$batch_name) ? 'Create' : 'Edit') . ' Batch',
  );
  drupal_set_breadcrumb($breadcrumb);

  $batch_record = islandora_digital_workflow_get_batch_record_by_name($batch_name);
  $batch_items = islandora_digital_workflow_get_batch_items_by_nid($batch_record['nid']);
  $batch_description_markup = (is_array($batch_record)) ?
    theme('islandora_digital_workflow_batch_description', array(
      'batch_record' => $batch_record,
      'collection_review_links' => islandora_digital_workflow_collection_review_links($batch_record),
      'can_ingest_all' => islandora_digital_workflow_can_ingest_all($batch_items),
      'can_publish_all' => islandora_digital_workflow_can_publish_all($batch_items),
      'batch_mapped_from_webformsubmission' => ($batch_record['webform_submission_sid'] ? islandora_digital_workflow_get_webformsubmission($batch_record['webform_submission_sid']) : ''),
    )) : '<h3>not set</h3>';

  $form = (!$batch_name) ?
    drupal_get_form('islandora_digital_workflow_create_empty_batch_form') :
    drupal_get_form('islandora_digital_workflow_create_batch_form', $batch_name);
  $title = 'Islandora Digital Workflow - ' . (($batch_name) ? 'Edit' : 'Create') . ' Batch';
  drupal_set_title(t($title), CHECK_PLAIN);
  $markup = $lookup_form_markup . $batch_description_markup . drupal_render($form);
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
  module_load_include('inc', 'islandora_digital_workflow', 'includes/taxonomy_utilities');
  $accessible_models = islandora_digital_workflow_get_accessible_cmodels();
  $configured = TRUE;

  // 1. Check that at least one model is configured
  if (count($accessible_models) < 2) {
    drupal_set_message(t('There are no <b>Supported Models</b> models configured ' . 
        'for creating batches or your user does not have permission to any models.  ' .
        'Please contact an administrator to ask them to configure the ') .
        l('Supported Models', '/admin/islandora/islandora_digital_workflow') .
        t(' and ') . l(t('set up permissions'), '/admin/people/permissions') .
        t(' to any models.'), 'error');
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

  // $batch_priorities_vocab = taxonomy_vocabulary_machine_name_load('workflow_content_type_vocab');
  $batch_priorities_vocab = taxonomy_vocabulary_machine_name_load(IDW_BATCH_PRIORITY_TAXONOMY_MACHINE_NAME);
  $priority_options = array(0 => 'Select Batch Priority');

  $regular_tid = 0;
  $terms = islandora_digital_workflow_get_all_terms_of_vocab($batch_priorities_vocab);
  foreach ($terms as $term) {
    $priority_options[$term->tid] = $term->name;
    if ($term->name == 'Regular') {
      $regular_tid = $term->tid;
    }
  }

  $batch_name = '';
  $batch_description = '';
  $button_text = SAVE_BUTTON_TEXT;
  $tid_for_cmodel_selected = '';
  $default_type_of_resources = islandora_digital_workflow_get_default_type_of_resource_options(TRUE);

  // If the user can administer site config, then display the link to the
  // supported models here.
  //
  // admin/islandora/islandora_digital_workflow
  $supported_models_description = 'Select the type of objects from the ' .
    'available content models that this batch will contain.' .
    (user_access('administer site configuration') ?
      '  Configuration for ' . l(t('Islandora Digital Workflow'),
      'admin/islandora/islandora_digital_workflow',
      array('attributes'=>array(
        'title' => 'link opens in separate tab',
        'class' => array('link_open_new_tab_tiny'),
        'target' => '_blank'))) . '.  ' : '');

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
    'batch_priority' => array(
      '#title' => t('Batch Priority'),
      '#type' => 'select',
      '#required' => true,
      '#options' => $priority_options,
      '#description' => 'Priority of this batch.',
      '#default_value' => $regular_tid,
    ),
    'batch_description' => array(
      '#type' => 'textarea',
      '#title' => t('Batch description'),
      '#description' => t('Description of the items in this batch.'),
      '#default_value' => '',
    ),

    // Is a request? checkbox
    'meta_defaults_frame' => array(
      '#title' => 'Metadata defaults',
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      'default_type_of_resource' => array(
        '#title' => t('Default type of resource'),
        '#type' => 'select',
        '#required' => true,
        '#options' => $default_type_of_resources,
        '#description' => 'Select the default type of resource for this batch.  ' .
          'Refer to ' . l(t('documentation on the MODS "typeOfResource" field.'),
          'https://www.loc.gov/standards/mods/userguide/typeofresource.html',
          array('attributes'=>array(
            'title' => 'link opens in separate tab',
            'class' => array('link_open_new_tab_tiny'),
            'target' => '_blank'))),
        '#default_value' => 0,
      ),
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
        '#default_value' => 0,
      ),
      // Requestor: textfield
      'batch_requestor' => array(
        '#type' => 'textfield',
        '#title' => t('Requestor'),
        '#default_value' => '',
      ),
      // Request due date: textfield
      'batch_request_due_date' => array(
        '#type' => 'date',
        '#default_value' => array(
          'month' => format_date(time(), 'custom', 'n'),
          'day' => format_date(time(), 'custom', 'j'),
          'year' => format_date(time(), 'custom', 'Y'),
         ),
        '#title' => t('Request Due Date'),
      ),
    ),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t($button_text),
  );
  $form['cancel'] = array(
    '#type' => 'submit',
    '#prefix' => '&nbsp; ',
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
  module_load_include('inc', 'islandora_digital_workflow', 'includes/node_utilities');
  switch ($id) {
    case 'edit-submit':
      $batch_record = islandora_digital_workflow_create_stub_batch_record($form_state);
      $current_time = date('H:i:s');
      drupal_set_message(t('[@date] Created new batch: ', array(
          '@date' => $current_time,
        )) .
        l($batch_record['batch_name'], 'islandora/islandora_digital_workflow/edit_batch/' . $batch_record['batch_name']), 'status');
      drupal_goto('islandora/islandora_digital_workflow/edit_batch/' . $batch_record['batch_name']);
      break;
    case 'edit-cancel':
      drupal_set_message(t('Create Batch has been cancelled.'), 'message_info');
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
 * @param array $form
 * @param array $form_state
 * @param type $batch_name
 * @return array
 *   The Drupal form definition.
 */
function islandora_digital_workflow_create_batch_form(array $form, array &$form_state, $batch_name = NULL) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/access_permissions');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/taxonomy_utilities');
  module_load_include('inc', 'islandora_marc_utility', 'includes/upload_mrc.form');
  drupal_add_js('misc/collapse.js');

  $readable_models = islandora_digital_workflow_get_reduced_readable_cmodels();
  $configured_supported_models = variable_get('islandora_digital_workflow_islandora_models', array());
  $accessible_models = array();
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
  }
  else {
    if (arg(2) === 'edit_batch') {
      drupal_set_message($batch_name . t(' did not exist -- redirected to Create Batch page.'), 'message_info');
      drupal_goto('/islandora/islandora_digital_workflow/create_batch');
    }
    $batch_name = '';
    $batch_description = '';
    $read_only_att = array();
    $button_text = SAVE_BUTTON_TEXT;
  }
  if (!(array_key_exists($batch_record['islandora_model'], $readable_models) === FALSE)) {
    $cmodel = $batch_record['islandora_model'];
    $tid_for_cmodel_selected = islandora_digital_workflow_get_tid_by_name($cmodel, $content_types_vocab);
  }
  else {
    $tid_for_cmodel_selected = '';
  }
  // Check that the model that this batch represents has an islandora_model
  // value that is supported.
  if ($tid_for_cmodel_selected && (array_key_exists($tid_for_cmodel_selected, $accessible_models) === FALSE)) {
    $link_attributes = array('attributes'=>array(
              'title' => 'link opens in separate tab',
              'class' => array('link_open_new_tab_tiny'),
              'target' => '_blank'));
    $admin_link = (user_access(ISLANDORA_DIGITAL_WORKFLOW_MANAGE)) ?
      '  <b>Configure the workflow here: ' .
        l('Islandora Digital Workflow Settings', '/admin/islandora/islandora_digital_workflow', $link_attributes) . '</b>'
      : '';
    $batch_link = l($batch_name, 'islandora/islandora_digital_workflow/edit_batch/' . $batch_name, $link_attributes);
    drupal_set_message(t('"@cmodel" islandora model that is set for ' . $batch_link .
        ' is not configured.  That model must be configured before this batch ' .
        'can be worked on.', array(
      '@cmodel' => $cmodel,
      '@admin_link' => $admin_link,
    )) . $admin_link, 'error');
    drupal_goto('islandora/islandora_digital_workflow');
  }
  $model_name = islandora_digital_workflow_get_name_of_tid($tid_for_cmodel_selected);
  $sequences = islandora_digital_workflow_get_workflow_sequences_for_model($model_name);
  $file_not_allowed_for_model_names = array('islandora:findingAidCModel', 'islandora:newspaperCModel', 'islandora:newspaperIssueCModel');
  $paged_content_model_names = islandora_digital_workflow_retrieve_applicable_paged_content_cmodels();

  $file_rows = $file_header = array();
  $tmp_uploaded_batch_items_file = drupal_realpath(drupal_tempnam('temporary://', 'batch_items'));
  $temp_csv_file = ($batch_record['uploaded_batch_items_file']) ? file_put_contents($tmp_uploaded_batch_items_file, $batch_record['uploaded_batch_items_file']) : '';
  if ($temp_csv_file && file_exists($tmp_uploaded_batch_items_file)) {
    // See what kind of file this is -- CSV, MARC MRC, or MARCXML
    $mimetype = mime_content_type($tmp_uploaded_batch_items_file);
    switch ($mimetype) {
      // 1. MRC "application/marc".
      case "application/marc":
      case "application/octet-stream":
        $marc_and_mods_records = islandora_marc_utility_mrc_split($tmp_uploaded_batch_items_file);
        list($file_header, $file_rows) = islandora_digital_workflow_mrc_results_as_array($marc_and_mods_records);
        break;
      // 2. MARCXML "application/xml".
      case "application/xml":
        $marc_and_mods_records = islandora_marc_utility_marcxml_split($tmp_uploaded_batch_items_file);
        list($file_header, $file_rows) = islandora_digital_workflow_mrc_results_as_array($marc_and_mods_records);
        break;

      default:
        // 3. CSV "text/plain" -- all other cases -- attempt as CSV "text/plain".
        $arr = islandora_digital_workflow_csv_file_as_array($tmp_uploaded_batch_items_file);
        $file_header = (isset($arr['header']) ? $arr['header'] : array());
        $file_rows = (isset($arr['rows']) ? $arr['rows'] : array());
        $csv_header_mappings = islandora_digital_workflow_get_csv_header_xpath_mappings();
        foreach ($file_header as $header) {
          $machine_header = str_replace(" ", "_", strtolower(trim($header)));
          if ($machine_header && !(array_key_exists($machine_header, $csv_header_mappings))) {
            drupal_set_message('CSV ' . t('column "') . $header . t('" not recognized.  Batch ' .
                'may not be able to ingest or process.  Please replace the attached ' .
                'CSV file.'), 'error');
          }
        }
        break;
    }
    @unlink($tmp_uploaded_batch_items_file);
  }
  // Prepare a couple variables for use with file upload options:
  $upload_size = min((int) ini_get('post_max_size'), (int) ini_get('upload_max_filesize'));
  $extensions = array('csv', 'xml', 'mrc');

  // For legacy batches, this array may need to be joined with the legacy
  // collection `c_id` pointers instead of Solr PID values.
  $collections = islandora_digital_workflow_get_solr_options('RELS_EXT_hasModel_uri_ms:info' .
      '\:fedora\/islandora\:collectionCModel -PID:*_review', 'PID', 'fgs_label_s');
  asort($collections);
  $default_type_of_resources = islandora_digital_workflow_get_default_type_of_resource_options(TRUE);
  // $batch_priorities_vocab = taxonomy_vocabulary_machine_name_load('workflow_content_type_vocab');
  $batch_priorities_vocab = taxonomy_vocabulary_machine_name_load(IDW_BATCH_PRIORITY_TAXONOMY_MACHINE_NAME);
  $priority_options = array(0 => 'Select Batch Priority');

  $terms = islandora_digital_workflow_get_all_terms_of_vocab($batch_priorities_vocab);
  foreach ($terms as $term) {
    $priority_options[$term->tid] = $term->name;
  }

  // If the user can administer site config, then display the link to the 
  // supported models here.
  //
  // admin/islandora/islandora_digital_workflow
  $supported_models_description = 'Select the type of objects from the ' .
      'available content models that this batch will contain.' .
      (user_access('administer site configuration') ?
          '  Configuration for ' . l(t('Islandora Digital Workflow'),
          'admin/islandora/islandora_digital_workflow',
          array('attributes'=>array(
              'title' => 'link opens in separate tab',
              'class' => array('link_open_new_tab_tiny'),
              'target' => '_blank'))) . '.  ' : '');

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
      'workflow_sequence_id' => array(
        '#title' => t('Workflow Sequence'),
        '#type' => 'select',
        '#required' => true,
        '#description' => t('The ') . 
          (user_access(ISLANDORA_DIGITAL_WORKFLOW_MANAGE_SEQUENCES) ?
              l(t('workflow sequence configuration'), 
                'admin/islandora/islandora_digital_workflow/workflow_sequences', 
                array('attributes'=>array(
              'title' => 'link opens in separate tab',
              'class' => array('link_open_new_tab_tiny'),
              'target' => '_blank'))) :
            'workflow sequence configuration') .
          t(' relates the actions that are required for this object type.'),
        '#options' => $sequences,
        '#default_value' => ($batch_record && isset($batch_record['workflow_sequence_id'])) ? $batch_record['workflow_sequence_id'] : '',
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
      'batch_priority' => array(
        '#title' => t('Batch Priority'),
        '#type' => 'select',
        '#required' => true,
        '#options' => $priority_options,
        '#description' => 'Priority of this batch.',
        '#default_value' => ($batch_record && isset($batch_record['batch_priority_tid'])) ? $batch_record['batch_priority_tid'] : 0,
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

  // Metadata defaults frame
  $form['batch_edit_frame']['meta_defaults_frame'] = array(
      '#type' => 'fieldset',
      '#title' => 'Metadata defaults',
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    );

  // Default copyright status: select (hard-coded options based off MODS
  // typeOfResource specification).
  $form['batch_edit_frame']['meta_defaults_frame']['default_type_of_resource'] = array(
      '#title' => t('Default type of resource'),
      '#type' => 'select',
      '#required' => true,
      '#options' => $default_type_of_resources,
      '#description' => 'Select the default type of resource for this batch.  ' .
        'Refer to ' . l(t('documentation on the MODS "typeOfResource" field.'),
            'https://www.loc.gov/standards/mods/userguide/typeofresource.html',
            array('attributes'=>array(
              'title' => 'link opens in separate tab',
              'class' => array('link_open_new_tab_tiny'),
              'target' => '_blank'))),
      '#default_value' => ($batch_record && isset($batch_record['default_type_of_resource'])) ? $batch_record['default_type_of_resource'] : '',
    );

  // Default genre: textfield
  $form['batch_edit_frame']['meta_defaults_frame']['default_genre'] = array(
      '#type' => 'textfield',
      '#title' => t('Default Genre'),
      '#default_value' => ($batch_record && isset($batch_record['default_genre'])) ? $batch_record['default_genre'] : '',
    );

    // Default depositor: textfield
  $form['batch_edit_frame']['meta_defaults_frame']['default_depositor'] = array(
        '#type' => 'textfield',
        '#description' => 'This value will only be used if the spreadsheet MODS does not contain a value for the depositor.',
        '#title' => t('Default Depositor'),
        '#default_value' => ($batch_record && isset($batch_record['default_depositor'])) ? $batch_record['default_depositor'] : '',
      );

  // Default copyright status: select (hard-coded options)
  $form['batch_edit_frame']['meta_defaults_frame']['batch_default_CR_status'] = array(
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
  $form['batch_edit_frame']['meta_defaults_frame']['batch_default_pub_status'] = array(
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
  $form['batch_edit_frame']['meta_defaults_frame']['batch_default_CR_holder'] = array(
      '#type' => 'textarea',
      '#title' => t('Default Copyright Holder Name'),
      '#default_value' => ($batch_record && isset($batch_record['batch_default_CR_holder'])) ? $batch_record['batch_default_CR_holder'] : '',
    );


  // Objects frame
  $form['batch_objects_frame'] = array(
      '#type' => 'fieldset',
      '#title' => 'Batch items',
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    );
  if ((array_search($model_name, $file_not_allowed_for_model_names) === FALSE)) {
    // Add a button if there is already a CSV for the batch that will perform the item / MODS
    // creation from the CSV file.
    if ($batch_record && isset($batch_record['uploaded_batch_items_file']) && ($batch_record['uploaded_batch_items_file'] <> '')) {
      // clicking this button should trigger the islandora_digital_workflow_prepare_csv2mods on this batch
      $form['batch_objects_frame']['sync_items_from_csv'] = array(
        '#item' => 'item',
        '#prefix' => '<div class="form-item form-type-textfield"><label>Sync Batch Items from CSV file</label>',
        '#suffix' => '<div class="description">Clicking this button will trigger the CSV to MODS creation routine and will update all of the batch records accordingly.</div></div>',
        '#markup' => l('Sync Batch Items', 'node/' . $batch_record['nid'] . '/sync_batch_items'),
      );
    }
  }

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
        '#description' => t('Select items file to upload.  Uploading a new file ' .
          'here <em>completely replaces any existing items records AND any ' .
          'transactions on them</em>.<br/>Files must be less than <strong>' .
          '@size MB.</strong><br/>Allowed file types: <strong>@ext.</strong>',
            array(
              '@size' => $upload_size,
              '@ext' => implode(", ", $extensions))) .
          (($batch_record && isset($batch_record['uploaded_batch_items_file']) && ($batch_record['uploaded_batch_items_file'] <> '')) ?
          t('<br>For CSV files ONLY, additional files may be ' .
          'merged to add more items, using the "Add items" option on the batch\'s ') .
          l(t('"Batch Items" page'), 'node/' . $batch_record['nid'] . '/items') . '.</em>' : '') .
          ' | ' . l('Download CSV template (minimal)', 'islandora/islandora_digital_workflow/csv_template/minimal') .
          ' | ' . l('Download CSV template (full)', 'islandora/islandora_digital_workflow/csv_template/extended'),
        '#upload_location' => 'temporary://',
        '#upload_validators' => array(
          'file_validate_extensions' => $extensions,
           // Assume its specified in MB.
          'file_validate_size' => array($upload_size * 1024 * 1024),
        ),
        '#prefix' => "<a id='file_upload'> </a>",
        '#suffix' => (count($file_rows) ? '<div class="fieldset_scrollable_div_wrapper"><div class="report_table_wrapper">' .
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
  // NOW, only add this next option for paged content.
  if (!(array_search($model_name, $paged_content_model_names) === FALSE)) {
    $islandora_ocr_tesseract_enabled_languages = variable_get('islandora_ocr_tesseract_enabled_languages');
    $ocr_languages = array();
    foreach ($islandora_ocr_tesseract_enabled_languages as $k => $v) {
      if ($v) {
        $ocr_languages[$k] = $v;
      }
    }
    $form['ingest_options_frame']['ocr_language'] = array(
        '#type' => 'select',
        '#title' => t('OCR Language'),
        '#options' => $ocr_languages,
        '#default_value' => 'eng',
        '#description' => t('Specify language to use for processing OCR.'),
      );
  }

  $form['ingest_options_frame']['ingest_namespace'] = array(
      'ingest_namespace' => array(
        '#type' => 'textfield',
        '#title' => t('Fedora object prefix (ingest namespace)'),
        '#description' => t('The namespace for objects that get ingested for ' .
            'batches by default.  Each batch can override this value.'),
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
  if (user_access('create child collection')) {
    $top_level_collection_uri = rtrim(variable_get('islandora_digital_workflow_top_level_collection', '/islandora/object/islandora:root'), "/");
    $form['ingest_options_frame']['collections_select']['#description'] =
      l(t('Create Collection'),
        $top_level_collection_uri . '/manage/overview/ingest',
        array('attributes'=>array(
          'title' => 'link opens in separate tab',
          'class' => array('link_open_new_tab_tiny'),
          'target' => '_blank'))) .
      ' if a new Islandora collection is needed.  In order to select any ' .
      'newly created collections, this page will need to be reloaded.';
  }


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

  islandora_digital_workflow_remove_model_based_form_elements($model_name, $form);
  $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t($button_text),
    );

  // Add a "Delete Batch" button if the user has permission to use it.
  if (user_access(ISLANDORA_DIGITAL_WORKFLOW_DELETE_ITEMS)) {
    $form['deletebatch'] = array(
        '#type' => 'submit',
        '#prefix' => '&nbsp; ',
        '#value' => t('Delete Batch'),
        '#attributes' => array('class' => array(
          'bad'
        )),
      );
  }

  $form['cancel'] = array(
      '#type' => 'submit',
      '#prefix' => '&nbsp; ',
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
 */
function islandora_digital_workflow_remove_model_based_form_elements($model_name, &$form) {
  switch ($model_name) {
    case "islandora:sp_basic_image": case "islandora:sp_large_image_cmodel":
      unset($form['batch_objects_frame']['default_ead_id']);
      break;

    case "islandora:findingAidCModel":
      unset($form['batch_objects_frame']['default_ead_id']);
      unset($form['batch_objects_frame']['file']);
      break;

    case "islandora:bookCModel":
      unset($form['batch_objects_frame']['default_ead_id']);
      break;

    case "islandora:manuscriptCModel":
      break;

    case "islandora:newspaperCModel":
      unset($form['batch_objects_frame']['file']);
      unset($form['batch_objects_frame']['default_ead_id']);
      break;

    case "islandora:newspaperIssueCModel":
      unset($form['batch_objects_frame']['default_ead_id']);
      break;

    case "islandora:sp_web_archive":
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
 *   Drupal form_state representation as passed to the validate function.
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
    drupal_set_message(t('Edit existing batch "') .
        $edit_this_batch_link . '".', 'warning');
  }
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  $legal_batch_name = islandora_digital_workflow_convert_to_filename($batch_name);
  if ($batch_name <> $legal_batch_name) {
    form_set_error('batch_name', t('The Batch name value contained illegal ' .
        'characters.  These have been replaced with new values.<br>Please review:<br>' .
        '<ul><li><b>Batch name:</b> "@init_batch_name"</li>' .
        '<li><b>Recommended name:</b> "@legal_batch_name"</li></ul>',
        array(
            '@init_batch_name' => $batch_name,
            '@legal_batch_name' => $legal_batch_name,
        )));
    $elements['batch_edit_frame']['batch_name']['#value'] = $legal_batch_name;
  }
}

/**
 * Validates the form input.
 *
 * Checks the uploaded_batch_items_file file to determine whether or not it is valid.
 *
 * @param array $elements
 *   Drupal elements representation as an array.
 * @param array $form_state
 *   Drupal form_state representation as passed to the validate function.
 * @param string $form_id
 *   The form_id that is being validated -- optional.
 */
function islandora_digital_workflow_create_batch_form_validate(&$elements, &$form_state, $form_id = NULL) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
  $batch_name = $form_state['values']['batch_name'];
  if (isset($_FILES['files']) && array_key_exists('tmp_name', $_FILES) && isset($_FILES['tmp_name'])) {
    $batch_record = islandora_digital_workflow_get_batch_record_by_name($batch_name);
    $batch_record_uploaded_csv = array_key_exists('uploaded_batch_items_file', $batch_record) ? $batch_record['uploaded_batch_items_file'] : '';
    if ($batch_record_uploaded_csv == '') {
      // Check the csv file upload.
      if (!islandora_digital_workflow_validate_batch_items_file_upload()) {
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
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/node_utilities');
  switch ($id) {
    case 'edit-submit':
      if (isset($_FILES['files']) && isset($_FILES["files"]["tmp_name"]['file'])) {
        islandora_digital_workflow_validate_batch_items_file_upload();
      }
      $batch_items_filename = (isset($_SESSION['uploaded_batch_items_filename']) ? $_SESSION['uploaded_batch_items_filename'] : '');
      $create_new = ($form_state['clicked_button']['#value'] === SAVE_BUTTON_TEXT);
      $batch_record = islandora_digital_workflow_sync_node_batch_record_info(NULL, $form_state, $batch_items_filename);
      $current_time = date('H:i:s');
      $edit_batch_link = l($batch_record['batch_name'], 'islandora/islandora_digital_workflow/edit_batch/' . $batch_record['batch_name']);
      if ($create_new) {
        drupal_set_message(t('[@date] Created new batch: ', array(
            '@date' => $current_time,
          )) .
          l($batch_record['batch_name'], 'islandora/islandora_digital_workflow/edit_batch/' . $batch_record['batch_name']), 'status');
      }
      else {
        drupal_set_message(t('[@date] Batch updated: ', array(
            '@date' => $current_time,
          )) .
          l($batch_record['batch_name'], 'islandora/islandora_digital_workflow/edit_batch/' . $batch_record['batch_name']), 'status');
      }
      if (file_exists($batch_items_filename)) {
        @unlink($batch_items_filename);
      }
      drupal_goto('node/' . $batch_record['nid']);
      break;
    case 'edit-deletebatch':
      $batch_record = islandora_digital_workflow_get_batch_record_by_name($form_state['values']['batch_name']);
      // Redirect the user to the "Delete Batch" form.
      drupal_goto('islandora/islandora_digital_workflow/delete_batch/' . $batch_record['batch_name']);
      break;
    case 'edit-cancel':
      drupal_set_message(t('Create Batch has been cancelled.'), 'message_info');
      drupal_goto('islandora/islandora_digital_workflow/');
      break;
  }
}

/**
 * Helper function that will scan and sync the MODS files from CSV for all items
 * in the current batch.
 *
 * @return string
 *   Markup for the processed sync of all batch items.
 */
function islandora_digital_workflow_sync_batch_items() {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/lookup.form');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/create_batch.form');

  $nid = arg(1);
  $node = ($nid) ? node_load($nid) : NULL;

  $lookup_form_markup = theme('islandora_digital_workflow_lookup', array('searchterm' => ''));

  $breadcrumb = array(
    l(t('Home'), '<front>'),
    l(t('Islandora Digital Workflow'), 'islandora/islandora_digital_workflow'),
    l($node->title, 'node/' . $node->nid),
  );
  drupal_set_breadcrumb($breadcrumb);

  $batch_name = $node->title;
  $batch_form = drupal_get_form('islandora_digital_workflow_sync_batch_items_confirmation', $batch_name);
  return $lookup_form_markup . drupal_render($batch_form);
}

/**
 * The Drupal form to confirm the "Sync files" procedure.  See the function
 * islandora_digital_workflow_sync_batch_items() above.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @param string $batch_name
 *   The name of the batch - (matches islandora_digital_workflow_batch.name)
 * @return array
 *   Drupal form definition.
 */
function islandora_digital_workflow_sync_batch_items_confirmation(array $form, array &$form_state, $batch_name = NULL) {
  $form = array(
    'confirm-message' => array(
      '#type' => 'item',
      '#markup' => '<b>Clicking this button will trigger the CSV to MODS creation routine and will update all of the batch records accordingly.</b><br>',
    ),
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Ok'),
    ),
    'cancel' => array(
      '#type' => 'submit',
      '#prefix' => '&nbsp; ',
      '#value' => t('Cancel'),
    ),
  );
  return $form;
}

/**
 * Submit form handler for "Sync files" procedure.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_digital_workflow_sync_batch_items_confirmation_submit(array $form, array &$form_state) {
  $id = $form_state['triggering_element']['#id'];
  $nid = arg(1);
  $node = ($nid) ? node_load($nid) : NULL;

  switch ($id) {
    case 'edit-submit':
      drupal_set_message(t('Sync Batch Items submitted.'));
      islandora_digital_workflow_sync_items_process_node($node);
      break;
    case 'edit-cancel':
      drupal_set_message(t('Sync Batch Items has been cancelled.'));
      break;
  }
  if (is_object($node)) {
    drupal_goto('node/' . $nid . '/batch');
  }
  else {
    drupal_set_message(t('The node could not be loaded for arg(1).  Value for nid ' .
        'to node record = @nid.  The system could not redirect to that node\'s ' .
        'batch record page (node/@nid/batch).', array('@nid' => $nid)), 'warning');
  }
}

/**
 * This will process the items related to the node.
 *
 * @param object $node
 *   Drupal node object that is "workflow_batch" content type.
 */
function islandora_digital_workflow_sync_items_process_node($node) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');

  $batch_record = islandora_digital_workflow_get_batch_record_by_nid($node->nid);

  $temp_batch_items_filename = drupal_realpath(drupal_tempnam('temporary://', 'batch_items'));
  if ($bytes_written = file_put_contents($temp_batch_items_filename, $batch_record['uploaded_batch_items_file'])) {
    $_SESSION['uploaded_batch_items_filename'] = $temp_batch_items_filename;
  }
  islandora_digital_workflow_sync_node_to_items($batch_record, $node, $temp_batch_items_filename, TRUE);
}
