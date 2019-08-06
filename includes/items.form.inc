<?php

/**
 * @file
 * Form and functions related to displaying all of the items in a batch.
 */

/**
 * Displays items related to a workflow_batch node.
 *
 * @return string
 *   HTML markup.
 */
function islandora_digital_workflow_batch_items() {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/lookup.form');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
  $lookup_form_markup = theme('islandora_digital_workflow_lookup', array('searchterm' => ''));
  $nid = arg(1);
  $node = ($nid) ? node_load($nid) : NULL;
  $breadcrumb = array(
    l(t('Home'), '<front>'),
    l(t('Islandora Digital Workflow'), 'islandora/islandora_digital_workflow'),
  );
  if ($node) {
    $breadcrumb[] = l($node->title, 'node/' . $node->nid);
  }
  // Need to load the batch record related to the node so that we can access the
  // batch's description.
  $batch_record = islandora_digital_workflow_get_batch_record_by_nid($node->nid);

  drupal_set_breadcrumb($breadcrumb);
  $transactions_form = drupal_get_form('islandora_digital_workflow_batch_items_form', $node);
  $add_item_form = drupal_get_form('islandora_digital_workflow_add_item_form', $node);
  $add_items_form = drupal_get_form('islandora_digital_workflow_add_items_form', $node);
  $batch_items = islandora_digital_workflow_get_batch_items_by_nid($batch_record['nid']);
  $markup = $lookup_form_markup .
      theme('islandora_digital_workflow_batch_description', array(
        'batch_record' => $batch_record,
        'collection_review_links' => islandora_digital_workflow_collection_review_links($batch_record),
        'can_ingest_all' => islandora_digital_workflow_can_ingest_all($batch_items),
        'can_publish_all' => islandora_digital_workflow_can_publish_all($batch_items),
        'batch_mapped_from_webformsubmission' => islandora_digital_workflow_get_webformsubmission($batch_record['webform_submission_sid']),
      )) .
      drupal_render($add_item_form) .
      drupal_render($add_items_form) .
      drupal_render($transactions_form);
  return $markup;
}

/**
 * Form to allow user to add manually one item to the batch.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @param object $node
 *   Drupal node object
 * @return array
 *   Drupal form definition.
 */
function islandora_digital_workflow_add_item_form(array $form, array &$form_state, $node) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');

  $batch_name = $node->title;
  $results = islandora_digital_workflow_find_batch_records_by_name($batch_name, TRUE);
  $batch_record = array_pop($results);
  $batch_record_arr = (array)$batch_record;

  $namespace = islandora_digital_workflow_get_ingest_namespace($batch_record_arr);
  $form = array(
    'wrapper' => array(
      '#type' => 'fieldset',
      '#title' => t('Add item'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#attributes' => array('class' => array('inline_form')),
      'namespace' => array(
        '#type' => 'textfield',
        '#title' => t('Ingest Namespace'),
        '#default_value' => $namespace,
        '#size' => 8,
        '#attributes' => array('readonly' => 'readonly', 'disabled' => 'disabled'),
      ),
      'identifier' => array(
        '#type' => 'textfield',
        '#title' => t('Identifier'),
        '#required' => true,
        '#default_value' => '',
      ),
      'title' => array(
        '#type' => 'textfield',
        '#title' => t('Title'),
        '#required' => true,
        '#default_value' => '',
      ),
      'submit' => array(
        '#type' => 'submit',
        '#value' => t('Add item'),
      ),
    ),
    'batch_id' => array(
      '#type' => 'hidden',
      '#value' => $batch_record->batch_id,
    ),
  );
  return $form;
}

/**
 * Form submit handler for the "Add item" form.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_digital_workflow_add_item_form_submit(array $form, array &$form_state) {
  $batch_id = $form_state['values']['batch_id'];
  $identifier = $form_state['values']['identifier'];
  $namespace = $form_state['values']['namespace'];
  $title = $form_state['values']['title'];
  $mods = '<?xml version="1.0" encoding="UTF-8"?>';
  $marc = '<?xml version="1.0" encoding="UTF-8"?>';
  $fields = array(
      'batch_id' => $batch_id,
      'identifier' => $identifier,
      'title' => $title,
      'filename' => '',
      'mods' => $mods,
      'marc' => $marc,
  );
  if (variable_get('islandora_digital_workflow_assign_PIDs', 0)) {
    $fields['assigned_pid'] = rtrim($namespace, ':') . ':' . $identifier;
  }

  // Each MODS is inserted into the item record - this will insert the record:
  $batch_item_id = islandora_digital_workflow_insert_batch_item($fields);
}

/**
 * Form to allow user to upload CSV to add multiple items to the batch.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @param object $node
 *   Drupal node object.
 * @return type
 *   Drupal form definition.
 */
function islandora_digital_workflow_add_items_form(array $form, array &$form_state, $node) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/taxonomy_utilities');

  $batch_name = $node->title;
  $results = islandora_digital_workflow_find_batch_records_by_name($batch_name, TRUE);
  $batch_record = array_pop($results);
  $batch_record_arr = (array)$batch_record;

  // This is needed to determine whether or not to allow a CSV file upload.
  $readable_models = islandora_digital_workflow_get_reduced_readable_cmodels();
  $content_types_vocab = taxonomy_vocabulary_machine_name_load(IDW_CONTENT_TYPE_TAXONOMY_MACHINE_NAME);
  if (!(array_key_exists($batch_record_arr['default_type_of_resource'], $readable_models) === FALSE)) {
    $cmodel = $batch_record_arr['default_type_of_resource'];
    $tid_for_cmodel_selected = islandora_digital_workflow_get_tid_by_name($cmodel, $content_types_vocab);
  }
  else {
    $tid_for_cmodel_selected = '';
  }
  $model_name = islandora_digital_workflow_get_name_of_tid($tid_for_cmodel_selected);
  $file_not_allowed_for_model_names = array('islandora:findingAidCModel', 'islandora:newspaperCModel', 'islandora:newspaperIssueCModel');

  if ((array_search($model_name, $file_not_allowed_for_model_names) === FALSE)) {
    // Prepare a couple variables for use with file upload options:
    $file_rows = $file_header = array();
    $tmp_uploaded_batch_items_file = drupal_realpath(drupal_tempnam('temporary://', 'batch_items'));
    $temp_csv_file = ($batch_record_arr['uploaded_batch_items_file']) ? file_put_contents($tmp_uploaded_batch_items_file, $batch_record_arr['uploaded_batch_items_file']) : '';
    if ($temp_csv_file && file_exists($tmp_uploaded_batch_items_file)) {
      $mimetype = mime_content_type($tmp_uploaded_batch_items_file);
      switch ($mimetype) {
        // 1. MRC "application/marc".
        case "application/marc":
        case "application/octet-stream":
          if (module_exists('islandora_marc_utility')) {
              module_load_include('inc', 'islandora_marc_utility', 'includes/upload_mrc.form');
              $marc_and_mods_records = islandora_marc_utility_mrc_split($tmp_uploaded_batch_items_file);
              list($file_header, $file_rows) = islandora_digital_workflow_results_as_array($marc_and_mods_records);
          }
          break;
        // 2. MARCXML "application/xml".
        case "application/xml":
            if ($model_name == 'islandora:manuscriptCModel' && module_exists('islandora_ead_utility')) {
                module_load_include('inc', 'islandora_ead_utility', 'includes/upload_ead.form');
                $namespace = islandora_digital_workflow_get_ingest_namespace($batch_record);
                $bibid_and_mods_records = islandora_ead_utility_eadxml_split($tmp_uploaded_batch_items_file, '', $namespace);
                list($file_header, $file_rows) = islandora_digital_workflow_results_as_array($bibid_and_mods_records);
            }
            elseif (module_exists('islandora_marc_utility')) {
                module_load_include('inc', 'islandora_marc_utility', 'includes/upload_mrc.form');
                $marc_and_mods_records = islandora_marc_utility_marcxml_split($tmp_uploaded_batch_items_file);
                list($file_header, $file_rows) = islandora_digital_workflow_results_as_array($marc_and_mods_records);
            }
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

    $upload_size = min((int) ini_get('post_max_size'), (int) ini_get('upload_max_filesize'));
    $extensions = array('csv');

    $form = array(
      'wrapper' => array(
        '#type' => 'fieldset',
        '#title' => t('Add items'),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        'file' => array(
          '#type' => 'file',
          '#title' => t('File'),
          '#description' => t('Select CSV file to append to current CSV for batch.<br/>' .
              'Files must be less than <strong>@size MB.</strong><br/>Allowed file types: ' .
              '<strong>@ext.</strong>', array('@size' => $upload_size, '@ext' => $extensions[0])) .
              l('Download CSV template (minimal)', 'islandora/islandora_digital_workflow/csv_template/minimal') .
                ' | ' . l('Download CSV template (full)', 'islandora/islandora_digital_workflow/csv_template/extended'),
          '#upload_location' => 'temporary://',
          '#upload_validators' => array(
            'file_validate_extensions' => $extensions,
             // Assume its specified in MB.
            'file_validate_size' => array($upload_size * 1024 * 1024),
          ),
        ),
        'submit' => array(
          '#type' => 'submit',
          '#value' => t('Add items'),
          '#prefix' => "<a id='file_upload'> </a>",
          '#suffix' => (count($file_rows) ? '<div class="fieldset_scrollable_div_wrapper"><h3>Current CSV for batch</h3>' .
              '<div class="report_table_wrapper">' . theme('table', array(
                  'rows' => $file_rows,
                  'header' => $file_header,
                  'attributes' => array('class' => array('report_table')))) .
            '</div></div>'  : ''),
        ),
      ),
      'batch_id' => array(
        '#type' => 'hidden',
        '#value' => $batch_record->batch_id,
      ),
    );
  }
  return $form;
}

/**
 * Validates the form input.
 *
 * Checks if the uploaded CSV file is valid -- also checks that the CSV does
 * not contain any identifiers that already exist for the batch.
 *
 * @param array $elements
 *   Drupal elements representation as an array.
 * @param array $form_state
 *   Drupal form_state representation as passed to the validate function. *
 * @param string $form_id
 *   The form_id that is being validated -- optional.
 */
function islandora_digital_workflow_add_items_form_validate(&$elements, &$form_state, $form_id = NULL) {
  if (isset($_FILES['files']) && array_key_exists('tmp_name', $_FILES['files']) && isset($_FILES['files']['tmp_name'])) {
    // Check the csv file upload.
    if (!islandora_digital_workflow_validate_batch_items_file_upload()) {
      form_set_error('file', t('The uploaded batch items file is not valid.'));
    }
    $existing_batch_identifiers = islandora_digital_workflow_get_existing_identifiers($form_state['values']['batch_id']);
    $batch_record = islandora_digital_workflow_get_batch_record_by_batch_id($form_state['values']['batch_id']);

    $batch_items_filename = $_SESSION['uploaded_batch_items_filename'];
    $csv_file_sources = islandora_digital_workflow_prepare_csv2mods($batch_items_filename, $batch_record, FALSE);
    $all_ok = TRUE;
    foreach ($csv_file_sources as $csv_file_source) {
      $all_ok &= (array_search($csv_file_source['identifier'], $existing_batch_identifiers) === FALSE);
    }
    if (count($csv_file_sources) > 0 && !$all_ok) {
      form_set_error('file', t('At least one identifier value from the new CSV file already exists for this batch.'));
    }
  }
}

/**
 * Form submit handler for the "Add items" form.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_digital_workflow_add_items_form_submit(array $form, array &$form_state) {
  // Parse the uploaded file CSV to create the batch_items records.  If
  // successful import, merge this CSV file with the batch record's
  // uploaded_batch_items_file
  if (isset($_FILES['files']) && isset($_FILES["files"]["tmp_name"]['file'])) {
    $batch_id = $form_state['values']['batch_id'];
    $batch_items_filename = (isset($_SESSION['uploaded_batch_items_filename']) ? $_SESSION['uploaded_batch_items_filename'] : '');
    $upload_success = islandora_digital_workflow_validate_batch_items_file_upload($batch_items_filename);

    if ($batch_items_filename) {
      module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
      module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
      $batch_record = islandora_digital_workflow_get_batch_record_by_batch_id($batch_id);
      // To merge the new CSV upload with the existing record's uploaded_batch_items_file,
      // it must first be sanitized and the first row removed.
      $new_uploaded_batch_items_file = ($batch_items_filename) ? file_get_contents(drupal_realpath($batch_items_filename)) : FALSE;

      $new_uploaded_batch_items_file = islandora_digital_workflow_convert_smart_quotes($new_uploaded_batch_items_file);
      if ($batch_items_filename) {
        file_put_contents($batch_items_filename, $new_uploaded_batch_items_file);
      }
      $node = node_load($batch_record['nid']);
      // The merge CSV call will actually create batch_items records.
      islandora_digital_workflow_merge_uploaded_csv_files($batch_record, $node, $batch_items_filename);
      $temp_batch_items_filename = drupal_realpath(drupal_tempnam('temporary://', 'batch_objects.csv'));
      if ($bytes_written = file_put_contents($temp_batch_items_filename, $batch_record['uploaded_batch_items_file'])) {
        $_SESSION['uploaded_batch_items_filename'] = $temp_batch_items_filename;
      }
      islandora_digital_workflow_sync_node_to_items($batch_record, $node, $temp_batch_items_filename, FALSE);
      // Safe to delete the $batch_items_filename file -- done using it.
      if (file_exists($batch_items_filename)) {
        unlink($batch_items_filename);
      }
      if (file_exists($temp_batch_items_filename)) {
        unlink($temp_batch_items_filename);
      }
    }
    drupal_goto('/node/' . $batch_record['nid'] . '/items');
  }
}

/**
 * Form to display options to add / remove transaction to apply to checked items
 * from the tableselect of all items in the batch.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @param object $node
 *   Drupal node object.
 * @return type
 *   Drupal form definition.
 */
function islandora_digital_workflow_batch_items_form(array $form, array &$form_state, $node) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');

  $batch_name = $node->title;
  $results = islandora_digital_workflow_find_batch_records_by_name($batch_name, TRUE);
  $batch_record = (array) array_pop($results);

  // Prepare various objects for the dynamic form generation...
  $items = islandora_digital_workflow_get_batch_items_by_nid($node->nid);
  $options = array();
  $default_type_of_resources = islandora_digital_workflow_get_default_type_of_resource_options(TRUE);

  $item_transactions_count = ((array_key_exists(0, $items) && is_object($items[0])) ? count($items[0]->transactions) : 0);
  // Based on the $item_transactions_count, set a class for the "Transactions" table
  // cell that is wide enough to display all of the actions.
  $transactions_cell_class = (($item_transactions_count < 7) ? 'td_wide_normal' :
          (($item_transactions_count < 9) ? 'td_wide_180' :
          ($item_transactions_count < 11) ? 'td_wide_240' :
          (($item_transactions_count < 16) ? 'td_wide_335' : 'td_wide_400')));

  $is_batch_paged_content = islandora_digital_workflow_is_paged_object($batch_record['islandora_model']);
  $all_items_are_paged_content = TRUE;
  $an_item_exists_in_other_batch = FALSE;
  foreach ($items as $item) {
    $all_items_are_paged_content &= islandora_digital_workflow_is_paged_object($item->islandora_model);
    $item_exists_in_other_batch = islandora_digital_workflow_get_batch_item_id_by_identifier($item->identifier, NULL, FALSE, $item->batch_id);
    $an_item_exists_in_other_batch |= $item_exists_in_other_batch;
    $identifier_cell_text = ($item_exists_in_other_batch) ?
        '<span class="small_font alert" title="This identifier exists in another batch">' . l($item->identifier, '/node/' . $node->nid . '/item/' . $item->batch_item_id) . '</span>' :
        '<span class="small_font">' . l($item->identifier, '/node/' . $node->nid . '/item/' . $item->batch_item_id) . '</span>';
    $options[$item->identifier] = array(
      'id' => $item->identifier,
      t('Transactions') => array('data' => theme('islandora_digital_workflow_transactions_glyphs', array('transaction_actions' => $item->transactions)),
          'class' => array($transactions_cell_class)),
      t('Title') => array('data' => l($item->title, '/node/' . $node->nid . '/item/' . $item->batch_item_id),
          'class' => array('small_font', 'not_too_wide')),
      t('Identifier') => $identifier_cell_text,
      t('Filename') => '<span class="small_font">' . $item->master_filename_basename . '</span>',
      t('Progress') => '<span class="small_font progress_' .
        strtolower(str_replace(" ", "_", $item->progress)) . '">' . $item->progress . '</span>',
      t('Ready for') => '<span class="small_font">' . $item->ready_for . '</span>',
      t('Last action') => '<span class="small_font">' . $item->last_action . '</span>',
      t('When') => '<span class="small_font">' . $item->when . '</span><br>' . 
        '<span class="small_lt_text">' . islandora_digital_workflow_timeago_from_timestamp($item->when) . '</span>',
    );
  }
  $items_header = array(
    'Transactions' => t('Transactions'),
    'Title' => array('data' => t('Title'),
          'class' => array('not_too_wide')), //  'Title',
    'Identifier' => t('Identifier'),
    'Filename' => t('Filename'),
    'Progress' => t('Progress'),
    'Ready for' => t('Ready for'),
    'Last action' => t('Last action'),
    'When' => t('When'),
  );

  // For paged content, remove the filename column -- only if the entire set of
  // items are paged as well because a mixed batch could have some paged items
  // and some that are not, but we'd still want to display that column
  if ($all_items_are_paged_content || $is_batch_paged_content) {
    unset($items_header['Filename']);
    foreach ($options as $ident => $option) {
      unset($options[$ident]['Filename']);
    }
  }
  $_SESSION['an_item_exists_in_other_batch'] = $an_item_exists_in_other_batch;

  $required_batch_actions = islandora_digital_workflow_get_required_batch_actions($batch_record['batch_id']);
  // Easy to loop through the set of $required_batch_actions to set the ones that
  // are valid for this batch's sequence.
  $batch_items_actions = array('' => 'Select an action');
  foreach ($required_batch_actions as $required_batch_action) {
    $batch_items_actions[$required_batch_action['action_id']] = $required_batch_action['batch_action_description'];
  }

  if (is_array($required_batch_actions)) {
    $copy_required_batch_actions = $required_batch_actions;
    $first_required_batch_actions = array_shift($copy_required_batch_actions);
    $sequence_name = $first_required_batch_actions['sequence_name'];
    // Make this into a link for those with access permissions to the sequence.
    $sequence_name = ((user_access(ISLANDORA_DIGITAL_WORKFLOW_MANAGE_SEQUENCES)) ?
            l($sequence_name, '/admin/islandora/islandora_digital_workflow/workflow_sequences',
                array(
                    'fragment' => $sequence_name,
                    'external' => TRUE,
                    'attributes'=>array(
                      'title' => 'link opens in separate tab',
                      'class' => array('link_open_new_tab_tiny'),
                      'target' => '_blank'),
                )
            ) :
            $sequence_name);
    $workflow_sequence_text =
        theme('islandora_digital_workflow_transactions_glyphs', array(
            'transaction_actions' => $required_batch_actions,
            'display_as_requirements' => TRUE,
            'display_actions_key' => FALSE,
            'sequence_name' => $sequence_name));
  }
  else {
    $workflow_sequence_text = '<em>Either there are no actions configured for this ' . 
        'batch\'s "Workflow Sequence", or this batch does not have a workflow ' . 
        'sequence associated with it.</em>  In order to enforce that certain transactions ' .
        'occur for this batch\'s objects, ' . 
        l(t('select a "Workflow Sequence" for this batch'), '/node/' . $node->nid . '/batch') . '.';
  }

  $form = array();
  $form['wrapper'] = array(
      '#type' => 'fieldset',
      '#title' => t('Add / Remove Transactions'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
  );

  // Add a "Delete Items" button if the user has permission to use it.
  if (user_access(ISLANDORA_DIGITAL_WORKFLOW_DELETE_ITEMS)) {
    $form['wrapper_deleteitems'] = array(
        '#type' => 'fieldset',
        '#title' => t('Delete Item/s'),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
    );
    $form['wrapper_deleteitems']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Delete'),
            '#attributes' => array('class' => array(
          'bad'
        )),
        '#suffix' => '&nbsp;' . t(' <b>NOTE: </b>This can not be undone.'),

    );
  }

  if (user_access(ISLANDORA_DIGITAL_WORKFLOW_EDIT_ITEM_META))  {
    $form['wrapper_update'] = array(
        '#type' => 'fieldset',
        '#title' => t('Force Update MODS'),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
    );
    // Metadata defaults frame
    $form['wrapper_update']['meta_defaults_frame'] = array(
        '#type' => 'fieldset',
        '#description' => t('Force the values from the batch record into selected ' .
            'items\' MODS (using the checkboxes in the table below).  Select which ' .
            'fields to overwrite any values; alternate values can be set using each ' .
            'of the field prompts.'),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
      );

    // Default copyright status: select (hard-coded options based off MODS
    // typeOfResource specification).
    $form['wrapper_update']['meta_defaults_frame']['force_default_type_of_resource'] = array(
      '#title' => 'Force type of resource?',
      '#type' => 'checkbox',
      'default_value' => FALSE,
    );
    $form['wrapper_update']['meta_defaults_frame']['default_type_of_resource'] = array(
        '#type' => 'select',
        '#required' => true,
        '#attributes' => array(
          'title' => 'Default type of resource',
          'class' => array('indent-item')
        ),
        '#options' => $default_type_of_resources,
        '#default_value' => ($batch_record && isset($batch_record['default_type_of_resource'])) ? $batch_record['default_type_of_resource'] : '',
      );

    // Default genre: textfield
    $form['wrapper_update']['meta_defaults_frame']['force_default_genre'] = array(
      '#title' => 'Force Genre?',
      '#type' => 'checkbox',
      'default_value' => FALSE,
    );
    $form['wrapper_update']['meta_defaults_frame']['default_genre'] = array(
        '#type' => 'textfield',
        '#attributes' => array(
          'title' => 'Default Genre',
          'class' => array('indent-item')
        ),
        '#default_value' => ($batch_record && isset($batch_record['default_genre'])) ? $batch_record['default_genre'] : '',
      );

      // Default depositor: textfield
    $form['wrapper_update']['meta_defaults_frame']['force_default_depositor'] = array(
      '#title' => 'Force Depositor?',
      '#type' => 'checkbox',
      'default_value' => FALSE,
    );
    $form['wrapper_update']['meta_defaults_frame']['default_depositor'] = array(
        '#type' => 'textfield',
        '#attributes' => array(
          'title' => 'Default Depositor',
          'class' => array('indent-item')
        ),
        '#default_value' => ($batch_record && isset($batch_record['default_depositor'])) ? $batch_record['default_depositor'] : '',
      );

    // Default copyright status: select (hard-coded options)
    $form['wrapper_update']['meta_defaults_frame']['force_batch_default_CR_status'] = array(
      '#title' => 'Force Copyright Status?',
      '#type' => 'checkbox',
      'default_value' => FALSE,
    );
    $form['wrapper_update']['meta_defaults_frame']['batch_default_CR_status'] = array(
        '#type' => 'select',
        '#attributes' => array(
          'title' => 'Default Copyright Status',
          'class' => array('indent-item')
        ),
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
    $form['wrapper_update']['meta_defaults_frame']['force_batch_default_pub_status'] = array(
      '#title' => 'Force Publication Status?',
      '#type' => 'checkbox',
      'default_value' => FALSE,
    );
    $form['wrapper_update']['meta_defaults_frame']['batch_default_pub_status'] = array(
        '#type' => 'select',
        '#attributes' => array(
          'title' => 'Default Publication Status',
          'class' => array('indent-item')
        ),
        '#options' => array(''=>'Select Default Publication Status',
          'published' => 'published',
          'unpublished' => 'unpublished',
          'unknown' => 'unknown',
        ),
        '#default_value' => ($batch_record && isset($batch_record['batch_default_pub_status'])) ? $batch_record['batch_default_pub_status'] : '',
      );

    // Default copyright holder name: textarea
    $form['wrapper_update']['meta_defaults_frame']['force_batch_default_CR_holder'] = array(
      '#title' => 'Force Copyright Holder Name?',
      '#type' => 'checkbox',
      'default_value' => FALSE,
    );
    $form['wrapper_update']['meta_defaults_frame']['batch_default_CR_holder'] = array(
        '#type' => 'textarea',
        '#attributes' => array(
          'title' => 'Default Copyright Holder Name',
          'class' => array('indent-item')
        ),
        '#rows' => 1,
        '#default_value' => ($batch_record && isset($batch_record['batch_default_CR_holder'])) ? $batch_record['batch_default_CR_holder'] : '',
      );

    $form['wrapper_update']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Force Update'),
            '#attributes' => array('class' => array(
          'bad'
        )),
        '#suffix' => '&nbsp;' . t(' <b>NOTE: </b>This can not be undone.'),
    );
  }

  $form['wrapper']['notes_wrapper'] = array(
      '#type' => 'fieldset',
      '#title' => 'Problem notes',
      '#attributes' => array('class' => array('inline_fieldset', 'display_none')),
      'problem_notes' => array(
          '#type' => 'textarea',
          '#description' => 'Enter the instructions to resolve the problem/s for the selected item/s.',
          '#rows' => 3,
      ),
  );
  $form['wrapper']['add_batch_action'] = array(
      '#title' => t('Add Transaction'),
      '#type' => 'select',
      '#options' => $batch_items_actions,
      '#description' => 'Select the transaction to add to the selected batch items below.',
      '#default_value' => '',
      '#attributes' => array(
        'onchange' => 'add_action_selection(this)',
      ),
  );
  if (user_access(ISLANDORA_DIGITAL_WORKFLOW_EDIT_DELETE_TRANSACTIONS)) {
    $form['wrapper']['remove_batch_action'] = array(
        '#title' => t('Remove Transaction'),
        '#type' => 'select',
        '#options' => $batch_items_actions,
        '#description' => 'Select the transaction to remove from the selected batch items below.',
        '#default_value' => '',
    );
  }
  $form['wrapper']['using__batch_id'] = array(
      '#type' => 'hidden',
      '#default_value' => $batch_record['batch_id'],
  );
  $form['wrapper']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Modify transactions'),
  );
  $no_sequence_warning = ($batch_record['workflow_sequence_id'] && count($items) > 0) ? '' :
      t('<div class="messages error">This batch does not have a <b>' .
      l(t('Workflow Sequence'), 'node/' . $node->nid . '/batch') .
      '</b> associated with it.  Any actions for the items below ' .
      'would not display until that sequence is configured.  Click individual ' .
      'items below to view their transactions.</div>');
  $form['items'] = array(
      '#type' => 'tableselect',
      '#header' => $items_header,
      '#options' => $options,
      '#empty' => 'There are no items in the batch.',
      '#prefix' => $no_sequence_warning .
          '<small><i>Use the checkboxes in the table below to select ' .
          'item/s for "Add / Remove Transactions" ' .
          (user_access(ISLANDORA_DIGITAL_WORKFLOW_DELETE_ITEMS) ?
           'or "Delete Items" ' : '' ) . 'above</i></small>',
      '#suffix' => '<hr>' . $workflow_sequence_text,
    );
  return $form;
}

/**
 * Function that updates the items' transactions with user's input.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @return string
 *   HTML markup.
 */
function islandora_digital_workflow_batch_items_form_submit(array $form, array &$form_state) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');

  $selected_identifiers = array_filter($form_state['values']['items']);
  $using__batch_id = $form_state['values']['using__batch_id'];
  $add_action_id = $form_state['values']['add_batch_action'];
  $problem_notes = $form_state['values']['problem_notes'];
  $remove_action_id = $form_state['values']['remove_batch_action'];
  $batch_item_ids = array();

  switch ($form_state['values']['op']) {
    case t('Delete'):
      foreach ($selected_identifiers as $identifier) {
        $batch_item_id = islandora_digital_workflow_get_batch_item_id_by_identifier($identifier, $using__batch_id, TRUE);
        $batch_item_ids[] = $batch_item_id;
      }
      $_SESSION['ids_by_session'] = $batch_item_ids;
      drupal_goto('islandora/islandora_digital_workflow/delete_batch_item/ids_by_session');
      break;

    case t('Force Update'):
      $nid = arg(1);
      $node = node_load($nid);
      $batch_name = $node->title;
      $results = islandora_digital_workflow_find_batch_records_by_name($batch_name, TRUE);
      $batch_record = (array) array_pop($results);
      $default_type_of_resources = islandora_digital_workflow_get_default_type_of_resource_options(TRUE);

      // Prepare various objects for the dynamic form generation...
      $items = islandora_digital_workflow_get_batch_items_by_nid($node->nid);
      foreach ($selected_identifiers as $identifier) {
        $batch_item_id = islandora_digital_workflow_get_batch_item_id_by_identifier($identifier, $using__batch_id, TRUE);
        $batch_item_ids[] = $batch_item_id;
      }
      $mods_arr = islandora_digital_workflow_save_batch_items_mods($batch_item_ids);


      foreach ($mods_arr as $batch_item_id => $mods_size_filename) {
        $filename = (array_key_exists('MODS', $mods_size_filename) ?  $mods_size_filename['MODS'] : '');
        if (file_exists($filename)) {
          @unlink($filename);
        }
      }
      break;

    default:
      // "Submit"
      if ($remove_action_id == '' && $add_action_id == '') {
        drupal_set_message(t('No transactions were added or removed'), 'message_info');
      }
      else {
        $could_not_be_processed = array();
        foreach ($selected_identifiers as $identifier) {
          $batch_item_id = islandora_digital_workflow_get_batch_item_id_by_identifier($identifier, $using__batch_id, TRUE);

          if ($add_action_id) {
            if (!islandora_digital_workflow_insert_transactions_record($add_action_id, NULL, $batch_item_id, NULL, $problem_notes)) {
              $could_not_be_processed[] = $identifier;
            }
          }
          else {
            if (!islandora_digital_workflow_remove_transactions_record($remove_action_id, NULL, $batch_item_id)) {
              $could_not_be_processed[] = $identifier;
            }
          }
        }
        if (count($could_not_be_processed) > 0) {
          drupal_set_message(t('Some of the selected items could not be updated. ') .
            implode(", ", $could_not_be_processed));
        } elseif (count($selected_identifiers) > 0) {
          drupal_set_message(t('All selected items have been updated.'));
        } elseif (count($selected_identifiers) < 1) {
          drupal_set_message(t('No items were selected so there was nothing to update.'));
        }
      }
      break;
  }
}
