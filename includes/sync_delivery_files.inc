<?php

/**
 * @file
 * Form and functions related to syncing the files from the "Delivery" folder
 * back to system as well as moving them back into the working directory.
 */

/**
 * Displays the form to sync_delivery_files from a batch item.
 *
 * This approache is needed in order to display two separate forms on a single
 * page.  The lookup form's action needs to be handled separately from the
 * transaction_form.  Even though the node value is not passed as an argument, it
 * can be derived by using the arg() function.
 *
 * @return string
 *   HTML markup.
 */
function islandora_digital_workflow_sync_delivery_files() {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/lookup.form');
  $lookup_form_markup = theme('islandora_digital_workflow_lookup', array('searchterm' => ''));
  $batch_name = arg(3);
  drupal_set_title(t('Islandora Digital Workflow - Sync Delivery files'), CHECK_PLAIN);
  $form = drupal_get_form('islandora_digital_workflow_sync_delivery_files_form', $batch_name);
  $markup = $lookup_form_markup . drupal_render($form);
  return $markup;
}

/**
 * Displays form for sync_delivery_files.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @return string
 *   HTML markup.
 */
function islandora_digital_workflow_sync_delivery_files_form(array $form, array &$form_state, $batch_name) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');

  // Needed for collapsible fieldsets
  drupal_add_js('misc/collapse.js');
  // For some unknown reason, the admin form does not need the drupal collapse library
  // explicitly loaded like this, but it doesn't work here unless this library is included.
  drupal_add_library('system', 'drupal.collapse');

  $delivery_root = variable_get('islandora_digital_workflow_ingest_delivery_path', '');
  $batch_delivery_root = $delivery_root . '/' . $batch_name;
  $batch_delivery_folders = islandora_digital_workflow_find_folders_files($delivery_root);
  foreach ($batch_delivery_folders as $found_delivery_item_name => $info) {
    if ($found_delivery_item_name <> $batch_name) {
      unset($batch_delivery_folders[$found_delivery_item_name]);
    }
  }
  $markup = (count($batch_delivery_folders)) ?
    theme('islandora_digital_workflow_files_table', array(
      'folders' => $batch_delivery_folders,
      'root' => $batch_delivery_root,
      'action_link' => '',
      'show_message_status' => FALSE,
      'table_title' => 'Items moved to the "Delivery" folder location',
      'table_description' => 'Sync the files listed below back to the ingest working directory:',
    )) : '';

  $form['batch_name'] = array(
      '#type' => 'hidden',
      '#default_value' => $batch_name,
  );
  $form['submit'] = array(
      '#type' => 'submit',
      '#prefix' => $markup,
      '#value' => t('Sync these files'),
  );

  return $form;
}

/**
 * The sync_delivery_files form submit handler.
 *
 * Function that updates a batch transaction with user's input.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_digital_workflow_sync_delivery_files_form_submit(array $form, array &$form_state) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');

  $batch_name = $form_state['values']['batch_name'];
  $source_root = variable_get('islandora_digital_workflow_ingest_delivery_path', '');
  $batch_delivery_folders = islandora_digital_workflow_find_folders_files($source_root);
  $batch_record = islandora_digital_workflow_get_batch_record_by_name($batch_name);

  $source_path = $source_root . '/' . $batch_name;
  foreach ($batch_delivery_folders as $found_delivery_item_name => $info) {
    if ($found_delivery_item_name <> $batch_name) {
      unset($batch_delivery_folders[$found_delivery_item_name]);
    }
  }

  if (array_key_exists($batch_name, $batch_delivery_folders)) {
    if (array_key_exists('files', $batch_delivery_folders[$batch_name])) {
      $files = $batch_delivery_folders[$batch_name]['files'];
      // Need to find the batch_item_id value for each of the folders found
      // in the given batch folder.
      $items = islandora_digital_workflow_get_batch_items_by_nid($batch_record['nid']);
      $success_moving = TRUE;
      foreach ($items as $item_obj) {
        // Use the item's islandora_model value in the event that it is overriding
        // the value at the batch level, but use the batch value if the item value
        // is not set -- just to be safe.
        $content_model = (isset($item_obj->islandora_model) &&  $item_obj->islandora_model) ?
                $item_obj->islandora_model : $batch_record['islandora_model'];
        $success_moving &= islandora_digital_workflow_copy_delivery_files($item_obj->batch_item_id, FALSE, $content_model);
      }
      if ($success_moving) {
        drupal_set_message(t('The files have been copied over from the delivery folder for "@batch_name".',
                array('@batch_name' => $batch_record['batch_name'])), 'message_info');
        // Update the "problem_resolved_timestamp" value for the related
        // problem state record of the item (if there is one).
        islandora_digital_workflow_unset_problems_records($batch_record['batch_id']);
        drupal_goto('node/' . $batch_record['nid'] . '/files');
      }
    }
  }
  else {
    drupal_set_message(t('The Delivery folder did not contain a folder that matched the batch name of "@batch_name".', array(
        '@batch_name' => $batch_name
    )), 'warning');
  }
}
