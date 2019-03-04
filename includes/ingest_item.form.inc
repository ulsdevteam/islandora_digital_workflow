<?php

/**
 * @file
 * Include file for ingest related functions.
 */

/**
 * Function to ingest an entire batch of items.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @param type $batch_id
 *   The batch_id identifier of a workflow batch record.
 * @return array
 *   The Drupal form definition.
 */
function islandora_digital_workflow_ingest_all_batch(array $form, array &$form_state, $batch_id) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');

  $batch_record = islandora_digital_workflow_get_batch_record_by_batch_id($batch_id);
  $batch_items = islandora_digital_workflow_get_batch_items_by_nid($batch_record['nid']);
  $batch_description_markup = theme('islandora_digital_workflow_batch_description', array(
    'batch_record' => $batch_record,
    'collection_review_links' => islandora_digital_workflow_collection_review_links($batch_record),
    'can_ingest_all' => islandora_digital_workflow_can_ingest_all($batch_items),
    'can_publish_all' => islandora_digital_workflow_can_publish_all($batch_items),
    'batch_mapped_from_webformsubmission' => islandora_digital_workflow_get_webformsubmission($batch_record['webform_submission_sid']),
  ));
  $form = array(
    'confirm-message' => array(
      '#type' => 'item',
      '#markup' => $batch_description_markup .
        '<b>Clicking this button will start the ingest process for ' .
        'all ' . count($batch_items) . ' items in the batch.  Are you sure that ' .
        'you want to ingest "' . $batch_record['batch_name'] . '" now?</b><br>',
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
    'batch_id' => array(
      '#type' => 'hidden',
      '#default_value' => $batch_id,
    )
  );
  return $form;
}

function islandora_digital_workflow_ingest_all_batch_submit(array $form, array &$form_state) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
  $batch_id = $form_state['values']['batch_id'];

  $batch_record = islandora_digital_workflow_get_batch_record_by_batch_id($batch_id);
  $batch_items = islandora_digital_workflow_get_batch_items_by_nid($batch_record['nid']);
  // This is for when the user has submitted that they do want to ingest all.
  $success = TRUE;
  $batch_record = islandora_digital_workflow_get_batch_record_by_batch_id($batch_id);
  $items = islandora_digital_workflow_get_batch_items_by_nid($batch_record['nid']);
  if (count($items) > 0) {
    foreach ($items as $item) {
      drupal_set_message('ingesting batch_item_id = ' . $item->batch_item_id);
      $success |= islandora_digital_workflow_do_ingest_item($item->batch_item_id, $batch_record);
    }
  }
  if (!$success) {
    drupal_set_message('There was a problem attempting to ingest at least one ' .
        'of the items.  Batch name "' . $batch_record['batch_name'] . '".');
  }
  else {
    drupal_set_message('Done ingesting all ' . count($batch_items) . ' items from ' .
        '"' . $batch_record['batch_name'] . '".');
  }
  drupal_goto('node/' . $batch_record['nid']);
}

/**
 * The user-form definition for approving / cancelling the ingest of an item.
 * 
 * @todo check completedness of this item and only display the confirm dialog
 *       if the item is ready for ingest.
 * 
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @param integer $batch_item_id
 *   The batch_item_id identifier of an item's record.
 * @return array
 *   The Drupal form definition.
 */
function islandora_digital_workflow_ingest_item(array $form, array &$form_state, $batch_item_id) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');

  // Get the batch item record --
  $batch_item_arr = islandora_digital_workflow_get_batch_item_record($batch_item_id);
  $batch_item_object = array_pop($batch_item_arr);

  $form = array(
    'confirm-message' => array(
      '#type' => 'item',
      '#markup' => '<b>Clicking this button will start the ingest process for ' .
          'this item.  Are you sure that you want to ingest "' . $batch_item_object->identifier .
          '" now?</b><br>',
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
    'batch_item_id' => array(
        '#type' => 'hidden',
        '#default_value' => $batch_item_id,
    )
  );
  return $form;
}

/**
 * The submit form handler for approving / cancelling the ingest of an item.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_digital_workflow_ingest_item_submit(array $form, array &$form_state) {
  $id = $form_state['triggering_element']['#id'];
  $batch_item_id = $form_state['values']['batch_item_id'];
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
  $batch_item_arr = islandora_digital_workflow_get_batch_item_record($batch_item_id);
  $batch_item_object = array_pop($batch_item_arr);

  switch ($id) {
    case 'edit-submit':
      islandora_digital_workflow_do_ingest_item($batch_item_id);
      drupal_set_message(t('Ingest Item submitted for @batch_identifier.', array(
          '@batch_identifier' => $batch_item_object->identifier,
      )), 'status');
      break;
    case 'edit-cancel':
      drupal_set_message(t('Ingest Item has been cancelled for @batch_identifier.', array(
          '@batch_identifier' => $batch_item_object->identifier,
      )), 'message_info');
      break;
  }
  // For the redirect, load the batch record in order to get the node's nid value.
  $batch_record = islandora_digital_workflow_get_batch_record_by_batch_id($batch_item_object->batch_id);
  drupal_goto('node/' . $batch_record['nid'] . '/item/' . $batch_item_id);
}

/**
 * This will prepare a single batch item for ingest.
 *
 * @global object $user
 *   Drupal user object.
 * @param type $batch_item_id
 *   A batch item identifier.
 * @param type $batch_record
 *   A batch record as an array.
 *
 * @return boolean
 *   Success / failure of the prepare call.
 */
function islandora_digital_workflow_do_ingest_item($batch_item_id, $batch_record = NULL, $do_add_transaction = TRUE) {
  global $user;
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');

  // Add the transaction that the user has queued the item for ingest.
  if ($do_add_transaction) {
    islandora_digital_workflow_insert_transactions_record(IDW_ACTION_USER_INGEST, NULL, $batch_item_id);
  }

  // Get the batch item record -- see what type of object that is
  $batch_item_arr = islandora_digital_workflow_get_batch_item_record($batch_item_id);
  $batch_item_object = array_pop($batch_item_arr);
  if (is_null($batch_record)) {
    $batch_record = islandora_digital_workflow_get_batch_record_by_batch_id($batch_item_object->batch_id);
  }
  $drush_prepare_command = islandora_digital_workflow_make_drush_prepare_statement($batch_item_object, $batch_record);

  $output = array();
  $return_val = 1;
  $ingest_set_fields = NULL;
  try {
    $had_exception = FALSE;
    // Now, make the array of field values that will be saved
    $fields = array(
        'batch_id' => $batch_item_object->batch_id,
        'batch_item_id' => $batch_item_id,
        'drupal_uid' => $user->uid,
        'drush_command' => $drush_prepare_command,
        'output' => serialize(array('output' => 'There was an exception when this command was called.')),
        'return_val' => $return_val,
    );
    // Call switched from exec() to passthru() so that the web server is given
    // control right after issuing the ingest command... as a result, there
    // can not be any debug output value for ingest prepare calls.
    //   exec($drush_prepare_command, $output, $return_val);
    passthru($drush_prepare_command, $return_val);
    $output = '';
    // Try to ingest that set now too...
    $ingest_namespace = islandora_digital_workflow_get_ingest_namespace($batch_record);
    $ingest_namespace_nocolon = rtrim($ingest_namespace, ':');
    $item_assign_pid = (isset($batch_item_object->assigned_pid) && $batch_item_object->assigned_pid <> '') ? $batch_item_object->assigned_pid :
            $ingest_namespace_nocolon . ':' . trim($batch_item_object->identifier);
    $set = islandora_digital_workflow_get_set($item_assign_pid);
    if ($set) {
      $this_domain = 'http://' . $_SERVER['HTTP_HOST'];
      $ingest_set_drush_command = 'drush -v -u ' . $user->uid . ' --uri=' . $this_domain . ' islandora_batch_ingest --ingest_set=' . $set;

      // As above, the call has been changed from exec() to passthru() - no
      // debug output is possible.
      //  exec($ingest_set_drush_command, $ingest_set_output, $ingest_set_return_var);
      passthru($ingest_set_drush_command, $return_val);
      $ingest_set_output = '';
      $ingest_set_fields = $fields;
      $ingest_set_fields['output'] = serialize($ingest_set_output);
      $ingest_set_fields['return_val'] = $ingest_set_return_var;
    }
  }
  catch (Exception $ex) {
    $had_exception = TRUE;
  }
  $fields['output'] = serialize($output);
  $fields['return_val'] = $return_val;
  db_insert('islandora_digital_workflow_drush_log')
      ->fields($fields)
      ->execute();
  if (!is_null($ingest_set_fields)) {
    // The set was also ingested -- add the record for that as well.
    db_insert('islandora_digital_workflow_drush_log')
        ->fields($ingest_set_fields)
        ->execute();
  }
  if (($return_val == -1 || $return_val == 255) || $had_exception) {
    drupal_set_message(t('There was an error calling the drush command : @drush_command.', array('@drush_command' => $drush_prepare_command)), 'error');
    return FALSE;
  }
  return TRUE;
}

/**
 * This helper function will create the drush command from the batch item.
 *
 * @todo Newspaper issue handling is special.. it needs parent to be a newspaper
 *       object -- AND a collection relationship is separate.  The relationship
 *       for newspaper issues is different... use the
 *       relationship/relationship_pred/relationship_namespace parameters to specify
 *       the relationship between this issue and the parent newspaper object:
 *
 *            $param_and_vals[] = 'relationship=' . {PARENT_NEWSPAPER_PID};
 *            $param_and_vals[] = 'relationship_pred=isMemberOf';
 *            $param_and_vals[] = 'relationship_namespace=http://digital.library.pitt.edu/ontology/relations#';
 *
 *
 * @global object $user
 *   Drupal user object.
 * @param object $batch_item_object
 *   A batch item record as a stdObject.
 * @param type $batch_record
 *   A batch record as an array.
 *
 * @return string
 *   The actual drush command that would be used for preparing an object for ingest.
 */
function islandora_digital_workflow_make_drush_prepare_statement($batch_item_object, $batch_record) {
  global $user;
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  $this_domain = 'http://' . $_SERVER['HTTP_HOST'];
  $drush_command_prefix = 'drush -v -u ' . $user->uid . ' --uri=' . $this_domain . ' ';

  $content_model = (isset($batch_item_object->islandora_model) ?
      $batch_item_object->islandora_model : $batch_record['islandora_model']);
  $raw_item_path = islandora_digital_workflow_batch_path($batch_record, 'working') . '/' . $batch_item_object->identifier;
  $filepath = islandora_digital_workflow_filename_sanitize($raw_item_path);
  $batch_preprocessor = islandora_digital_workflow_get_preprocessor($content_model);

  $ingest_namespace = islandora_digital_workflow_get_ingest_namespace($batch_record);
  $item_assign_pid = $batch_item_object->assigned_pid;
  if ($item_assign_pid == '') {
    // use the identifier and namespace to get the assigned_pid value
    $item_assign_pid  = $ingest_namespace . $batch_item_object->identifier;
  }
  if (strstr($item_assign_pid, ":") == ":") {
    @list($ingest_namespace, $identifier_junk) = explode(":", $item_assign_pid);
  }
  $ingest_namespace_nocolon = rtrim($ingest_namespace, ':');

  // From the batch record:
  // For the --parent parameter use the collection mappings
  // For the -- use the site_id mapping based on config for islandora_digital_workflow_use_isMemberOfSite.

  // Switch all collections over to use the "_review" instances of the
  // collections -- when the object is set to "Publish the item", then the
  // relationship to the intended collections will be set.
  if (variable_get('islandora_digital_workflow_use_reviewCollections', 0)) {
    $parent = $batch_record['mapto_collections'];
    $collections_arr = explode(",", $parent);
    $review_collections = array();
    foreach ($collections_arr as $collection_pid) {
      $collection_pid = trim($collection_pid);
      if ($collection_pid) {
        $review_collections[] = str_replace("_review", "", $collection_pid) . '_review';
      }
    }
    if (count($review_collections) > 0) {
      islandora_digital_workflow_create_review_collections($review_collections);
    }
    $parent = implode(",", $review_collections);
  }
  else {
    $parent = $batch_record['mapto_collections'];
  }

  // For the --assign_pid value, use the items' identifier value prefixed by
  //   config variable_get('islandora_digital_workflow_ingest_namespace', 'islandora')
  //   ELSE islandora_pid value
  $param_and_vals = array();
  $param_and_vals[] = 'content_models=' . $content_model;
  $param_and_vals[] = 'namespace=' . $ingest_namespace_nocolon;
  $param_and_vals[] = 'type=directory';
  if (variable_get('islandora_digital_workflow_assign_PIDs', 0)) {
    $param_and_vals[] = 'assign_pid=' . $item_assign_pid;
  }
  // Relationship parameter as "isMemberOfSite" object
  if (variable_get('islandora_digital_workflow_use_isMemberOfSite', 0) && $batch_record['mapto_site_id_values']) {
    $param_and_vals[] = 'relationship=' . $batch_record['mapto_site_id_values'];
    $param_and_vals[] = 'relationship_pred=isMemberOfSite';
    $param_and_vals[] = 'relationship_namespace=http://digital.library.pitt.edu/ontology/relations#';
  }
  // Parent parameter as mapto_site_id_values value.  If this is not set, the
  // object will be related to the top-level collection islandora:root.
  if ($batch_record['mapto_collections']) {
    $param_and_vals[] = 'parent=' . $parent;
  }
  else {
    // we should set some collection as the parent --
    $default_parent_collection_pid = islandora_digital_workflow_get_default_parent_collection($ingest_namespace_nocolon);
    drupal_set_message(t('The object did not have a parent collection selected.  ' .
        'The default parent collection of @default_parent has been used.', array(
            '@default_parent' => $default_parent_collection_pid,
        )), 'message_info');
    $param_and_vals[] = 'parent=' . $default_parent_collection_pid;
  }
  $param_and_vals[] = 'target=' . $filepath;
  return $drush_command_prefix . ' ' . $batch_preprocessor . ' --' . implode(" --", $param_and_vals);
}

/**
 * Helper function to lookup and return the correponding "batch_ingest" module
 * for a content model.
 *
 * @param string $fedora_model
 *   The name of a fedora model.
 * @return string
 *   When a model match is found, the name of the drush prepare command for the
 * correponding "batch_ingest" module for that content model.
 */
function islandora_digital_workflow_get_preprocessor($fedora_model) {
  // perhaps want to use this function call  islandora_digital_workflow_model_ingest_module_info
  $mapping = array(
      'islandora:sp_basic_image' => 'islandora_batch_preprocess',
      'islandora:sp_large_image_cmodel' => 'islandora_batch_preprocess',
      'islandora:findingAidCModel' => 'islandora_newspaper_batch_preprocess',
      'islandora:bookCModel' => 'islandora_book_batch_preprocess',
      'islandora:manuscriptCModel' => 'islandora_manuscript_batch_preprocess',
      'islandora:newspaperCModel ' => 'islandora_newspaper_batch_preprocess',
      'islandora:newspaperIssueCModel' => 'islandora_newspaper_batch_preprocess',
      'islandora:sp_web_archive' => 'islandora_newspaper_batch_preprocess',
  );
  return (array_key_exists($fedora_model, $mapping) ? $mapping[$fedora_model] :
    '');
}
