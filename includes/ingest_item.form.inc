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
function islandora_digital_workflow_ingest_batch(array $form, array &$form_state, $batch_id) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');

  $success = TRUE;
  $batch_record = islandora_digital_workflow_get_batch_record_by_batch_id($batch_id);
  $items = islandora_digital_workflow_get_batch_items_by_nid($batch_record['nid']);
  if (count($items) > 0) {
    foreach ($items as $item) {
      $success |= islandora_digital_workflow_do_ingest_item($item->batch_item_id, $batch_record);
    }
  }
  return $success;
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
      )), 'status');
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
function islandora_digital_workflow_do_ingest_item($batch_item_id, $batch_record = NULL) {
  global $user;
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');

  // Get the batch item record -- see what type of object that is
  $batch_item_arr = islandora_digital_workflow_get_batch_item_record($batch_item_id);
  $batch_item_object = array_pop($batch_item_arr);
  if (is_null($batch_record)) {
    $batch_record = islandora_digital_workflow_get_batch_record_by_batch_id($batch_item_object->batch_id);
  }
  $drush_prepare_command = islandora_digital_workflow_make_drush_prepare_statement($batch_item_object, $batch_record);

  $output = array();
  $return_val = 1;

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
    // For development purposes for now... do not issue an actual drush command here
    //  -- just use the "pwd" command to execute so that nothing is prepared right now
    exec('pwd', $output, $return_val);  //  exec($drush_prepare_command, $output, $return_val);

  } catch (Exception $ex) {
    $had_exception = TRUE;
  }
  $fields['output'] = serialize($output);
  $fields['return_val'] = $return_val;
  db_insert('islandora_digital_workflow_drush_log')
      ->fields($fields)
      ->execute();

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
  $this_domain = 'http://' . $_SERVER['HTTP_HOST'];
  $drush_command_prefix = 'drush -v -u ' . $user->uid . ' --uri=' . $this_domain . ' ';

  $type_of_resource = (isset($batch_item_object->type_of_resource) ?
      $batch_item_object->type_of_resource : $batch_record['default_type_of_resource']);
  $batch_item_path = $batch_item_object->filename;
  $path_info = pathinfo($batch_item_path);
  $filepath = $path_info['dirname'];
  $batch_preprocessor = islandora_digital_workflow_get_preprocessor($type_of_resource);

  $ingest_namespace = islandora_digital_workflow_get_ingest_namespace($batch_record);
  $ingest_namespace_nocolon = rtrim($ingest_namespace, ':');
  // from the batch record:
  // for the --parent parameter use the collection mappings
  // for the -- use the site_id mapping based on config for islandora_digital_workflow_use_isMemberOfSite.

  // for the --assign_pid value, use the items' identifier value prefixed by
  //   config variable_get('islandora_digital_workflow_ingest_namespace', 'islandora')
  //   ELSE islandora_pid value
  $param_and_vals = array();
  $param_and_vals[] = 'content_models=' . $type_of_resource;
  $param_and_vals[] = 'namespace=' . $ingest_namespace_nocolon;
  $param_and_vals[] = 'type=directory';
  if (variable_get('islandora_digital_workflow_assign_PIDs', 0)) {
    $param_and_vals[] = 'assign_pid=' . $ingest_namespace . $batch_item_object->identifier;
  }
  // relationship parameter as "isMemberOfSite" object
  if (variable_get('islandora_digital_workflow_use_isMemberOfSite', 0) && $batch_record['mapto_site_id_values']) {
    $param_and_vals[] = 'relationship=' . $batch_record['mapto_site_id_values'];
    $param_and_vals[] = 'relationship_pred=isMemberOfSite';
    $param_and_vals[] = 'relationship_namespace=http://digital.library.pitt.edu/ontology/relations#';
  }
  // parent parameter as mapto_site_id_values value
  if ($batch_record['mapto_collections']) {
    $param_and_vals[] = 'parent=' . $batch_record['mapto_collections'];
  }
  $param_and_vals[] = 'target=' . $filepath;
  return $drush_command_prefix . implode(" --", $param_and_vals);
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
  // could load all models from a mysql table?
  
  // perhaps want to use this function call  islandora_digital_workflow_model_ingest_module_info
  $mapping = array(
      'islandora:sp_basic_image' => 'islandora_newspaper_batch_preprocess',
      'islandora:sp_large_image_cmodel' => 'islandora_newspaper_batch_preprocess',
      'islandora:findingAidCModel' => 'islandora_newspaper_batch_preprocess',
      'islandora:bookCModel' => 'islandora_newspaper_batch_preprocess',
      'islandora:manuscriptCModel' => 'islandora_newspaper_batch_preprocess',
      'islandora:newspaperCModel ' => 'islandora_newspaper_batch_preprocess',
      'islandora:newspaperIssueCModel' => 'islandora_newspaper_batch_preprocess',
      'islandora:sp_web_archive' => 'islandora_newspaper_batch_preprocess',
  );
  return ((!array_search($fedora_model, $mapping) === FALSE) ? $mapping[$fedora_model] :
    '');
}
