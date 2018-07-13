<?php

/**
 * Displays items related to a workflow_batch node.
 *
 * @return string
 *   HTML markup.
 */
function islandora_digital_workflow_batch_items() {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/lookup.form');
  $lookup_form_markup = theme('islandora_digital_workflow_lookup', array('searchterm' => ''));
  $nid = arg(1);
  $node = ($nid) ? node_load($nid) : NULL;
  $transactions_form = drupal_get_form('islandora_digital_workflow_batch_items_form', $node);
  $add_item_form = drupal_get_form('islandora_digital_workflow_add_item_form', $node);
  $add_items_form = drupal_get_form('islandora_digital_workflow_add_items_form', $node);
  $markup = $lookup_form_markup .
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
  $results = islandora_digital_workflow_find_batch_records_by_name($batch_name);
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
      'identifier' => array(
        '#type' => 'textfield',
        '#title' => t('Identifier'),
        '#required' => true,
        '#default_value' => $namespace,
      ),
      'title' => array(
        '#type' => 'textfield',
        '#title' => t('Title'),
        '#required' => true,
        '#default_value' => '',
      ),
      'filename' => array(
        '#type' => 'textfield',
        '#title' => t('Filename'),
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
  $title = $form_state['values']['title'];
  $filename = $form_state['values']['filename'];
  $mods = '<?xml version="1.0" encoding="UTF-8"?>';
  $marc = '<?xml version="1.0" encoding="UTF-8"?>';
  $fields = array(
      'batch_id' => $batch_id,
      'identifier' => $identifier,
      'title' => $title,
      'filename' => $filename,
      'mods' => $mods,
      'marc' => $marc,
  );
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
  module_load_include('inc', 'islandora_digital_workflow', 'includes/taxonomy-utilities');

  $batch_name = $node->title;
  $results = islandora_digital_workflow_find_batch_records_by_name($batch_name);
  $batch_record = array_pop($results);
  $batch_record_arr = (array)$batch_record;
  $batch_path = islandora_digital_workflow_batch_path($batch_record);

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
  $file_not_allowed_for_model_names = array('Finding Aid', 'Newspaper');

  if ((array_search($model_name, $file_not_allowed_for_model_names) === FALSE)) {
    // Prepare a couple variables for use with file upload options:
    $file_rows = $file_header = array();
    $tmp_uploaded_csv_file = drupal_realpath(drupal_tempnam('temporary://', 'batch_objects.csv'));
    $temp_csv_file = ($batch_record_arr['uploaded_csv_file']) ? file_put_contents($tmp_uploaded_csv_file, $batch_record_arr['uploaded_csv_file']) : '';
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
                (($batch_record && isset($batch_record_arr['file']) && ($batch_record_arr['file'] <> '')) ?
                    '<br>Upload file will be stored: <em>' . str_replace($batch_path, '', $batch_record_arr['file']) . '</em>' :
                    ''),
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
    if (!islandora_digital_workflow_validate_csv_file_upload()) {
      form_set_error('file', t('The uploaded CSV file is not valid.'));
    }
    $existing_batch_identifiers = islandora_digital_workflow_get_existing_identifiers($form_state['values']['batch_id']);
    $batch_record = islandora_digital_workflow_get_batch_record_by_batch_id($form_state['values']['batch_id']);

    $csv_filename = $_SESSION['uploaded_csv_filename'];
    $csv_file_sources = islandora_digital_workflow_prepare_csv2mods($csv_filename, $batch_record, FALSE);
    $all_ok = TRUE;
    foreach ($csv_file_sources as $csv_file_source) {
      $all_ok &= (array_search($csv_file_source['identifier'], $existing_batch_identifiers) === FALSE);
    }
    if (!$all_ok) {
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
  // uploaded_csv_file
  if (isset($_FILES['files']) && isset($_FILES["files"]["tmp_name"]['file'])) {
    $batch_id = $form_state['values']['batch_id'];
    $upload_success = islandora_digital_workflow_validate_csv_file_upload();
    $csv_filename = (isset($_SESSION['uploaded_csv_filename']) ? $_SESSION['uploaded_csv_filename'] : '');

    if ($csv_filename) {
      module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
      module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
      $batch_record = islandora_digital_workflow_get_batch_record_by_batch_id($batch_id);
      // To merge the new CSV upload with the existing record's uploaded_csv_file,
      // it must first be sanitized and the first row removed.
      $new_uploaded_csv_file = ($csv_filename) ? file_get_contents(drupal_realpath($csv_filename)) : FALSE;

      $new_uploaded_csv_file = islandora_digital_workflow_convert_smart_quotes($new_uploaded_csv_file);
      if ($csv_filename) {
        file_put_contents($csv_filename, $new_uploaded_csv_file);
      }
      $node = node_load($batch_record['nid']);
      islandora_digital_workflow_merge_uploaded_csv_files($batch_record, $node, $csv_filename);
      $temp_csv_filename = drupal_realpath(drupal_tempnam('temporary://', 'batch_objects.csv'));
      if ($bytes_written = file_put_contents($temp_csv_filename, $batch_record['uploaded_csv_file'])) {
        $_SESSION['uploaded_csv_filename'] = $temp_csv_filename;
      }
      islandora_digital_workflow_sync_node_to_items($batch_record, $node, $temp_csv_filename, FALSE);
      // Safe to delete the $csv_filename file -- done using it.
      if (file_exists($csv_filename)) {
        unlink($csv_filename);
      }
      if (file_exists($temp_csv_filename)) {
        unlink($temp_csv_filename);
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
  $results = islandora_digital_workflow_find_batch_records_by_name($batch_name);
  $batch_record = array_pop($results);

  // Prepare various objects for the dynamic form generation...
  $items = islandora_digital_workflow_get_batch_items_by_nid($node->nid);
  $options = array();
  foreach ($items as $item) {
    $options[$item->identifier] = array(
      'id' => $item->identifier,
      'Transactions' => theme('islandora_digital_workflow_transactions_glyphs', array('transaction_actions' => $item->transactions)),
      'Identifier' => l($item->identifier, '/node/' . $node->nid . '/item/' . $item->batch_item_id),
      'Title' => $item->title,
      'Filename' => $item->filename_basename,
    );
  }
  $items_header = array(
    'Transactions' => 'Transactions',
    'Identifier' => 'Identifier',
    'Title' => 'Title',
    'Filename' => 'Filename',
  );
  $required_batch_actions = islandora_digital_workflow_get_required_batch_actions($batch_record->batch_id);
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
                    'attributes' => array('target' => '_blank'),
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
        l('select a "Workflow Sequence" for this batch', '/node/' . $node->nid . '/batch') . '.';
  }

  $form = array();
  $form['wrapper'] = array(
      '#type' => 'fieldset',
      '#title' => t('Add / Remove Transactions'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
  );

  $form['wrapper']['add_batch_action'] = array(
      '#title' => t('Add Transaction'),
      '#type' => 'select',
      '#options' => $batch_items_actions,
      '#description' => 'Select the transaction to add to the selected batch items below.',
      '#default_value' => '',
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
      '#default_value' => $batch_record->batch_id,
  );
  $form['wrapper']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Modify transactions'),
  );
  $form['items'] = array(
      '#type' => 'tableselect',
      '#header' => $items_header,
      '#options' => $options,
      '#empty' => 'There are no items in the batch.',
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
  $selected_identifiers = array_filter($form_state['values']['items']);
  $using__batch_id = $form_state['values']['using__batch_id'];
  $add_action_id = $form_state['values']['add_batch_action'];
  $remove_action_id = $form_state['values']['remove_batch_action'];
  if ($remove_action_id == '' && $add_action_id == '') {
    drupal_set_message('No transactions were added or removed');
  }
  else {
    $could_not_be_processed = array();
    foreach ($selected_identifiers as $identifier) {
      $batch_item_id = islandora_digital_workflow_get_batch_item_record_by_identifier($identifier, $using__batch_id);
      if ($add_action_id) {
        if (!islandora_digital_workflow_insert_transactions_record($add_action_id, NULL, $batch_item_id)) {
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
    } else {
      drupal_set_message(t('All selected items have been updated.'));
    }
  }
}