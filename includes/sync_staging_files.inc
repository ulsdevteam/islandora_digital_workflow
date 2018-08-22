<?php

/**
 * Displays the form to sync_staging_files from a batch item.
 *
 * This approache is needed in order to display two separate forms on a single
 * page.  The lookup form's action needs to be handled separately from the
 * transaction_form.  Even though the node value is not passed as an argument, it
 * can be derived by using the arg() function.
 *
 * @return string
 *   HTML markup.
 */
function islandora_digital_workflow_sync_staging_files() {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/lookup.form');
  $lookup_form_markup = theme('islandora_digital_workflow_lookup', array('searchterm' => ''));
  $batch_name = arg(3);
  drupal_set_title(t('Islandora Digital Workflow - Sync staging files'), CHECK_PLAIN);
  $form = drupal_get_form('islandora_digital_workflow_sync_staging_files_form', $batch_name);
  $markup = $lookup_form_markup . drupal_render($form);
  return $markup;
}

/**
 * Displays form for sync_staging_files.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @return string
 *   HTML markup.
 */
function islandora_digital_workflow_sync_staging_files_form(array $form, array &$form_state, $batch_name) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');

  // Needed for collapsible fieldsets
  drupal_add_js('misc/collapse.js');
  // For some unknown reason, the admin form does not need the drupal collapse library
  // explicitly loaded like this, but it doesn't work here unless this library is included.
  drupal_add_library('system', 'drupal.collapse');

  $staging_root = variable_get('islandora_digital_workflow_ingest_staging_path', '');
  $batch_staging_root = $staging_root . '/' . $batch_name;
  $batch_staging_folders = islandora_digital_workflow_find_folders_files($staging_root);
  foreach ($batch_staging_folders as $found_staging_item_name => $info) {
    if ($found_staging_item_name <> $batch_name) {
      unset($batch_staging_folders[$found_staging_item_name]);
    }
  }
  $markup = (count($batch_staging_folders)) ?
    theme('islandora_digital_workflow_files_table', array(
      'folders' => $batch_staging_folders,
      'root' => $batch_staging_root,
      'action_link' => '',
      'show_message_status' => FALSE,
      'table_title' => 'Items moved to the "Staging" folder location',
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
 * The sync_staging_files form submit handler.
 *
 * Function that updates a batch transaction with user's input.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_digital_workflow_sync_staging_files_form_submit(array $form, array &$form_state) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');

  $batch_name = $form_state['values']['batch_name'];
  $source_root = variable_get('islandora_digital_workflow_ingest_staging_path', '');
  $batch_staging_folders = islandora_digital_workflow_find_folders_files($source_root);
  $batch_record = islandora_digital_workflow_get_batch_record_by_name($batch_name);

  $destination_path = $batch_record['batch_host'] . '/' . $batch_name;
  $source_path = $source_root . '/' . $batch_name;
  foreach ($batch_staging_folders as $found_staging_item_name => $info) {
    if ($found_staging_item_name <> $batch_name) {
      unset($batch_staging_folders[$found_staging_item_name]);
    }
  }
  $success_moving = FALSE;
  if (array_key_exists($batch_name, $batch_staging_folders)) {
    if (array_key_exists('files', $batch_staging_folders[$batch_name])) {
      $files = $batch_staging_folders[$batch_name]['files'];
      foreach ($files as $file) {
        $from_file = $source_path . '/' . $file;
        $to_file = $destination_path . '/' . $file;
        $from_exists = (file_exists($from_file) ? ' TRUE' : ' FALSE');
        $to_exists = (file_exists($to_file) ? ' TRUE' : ' FALSE');
        if (file_exists($from_file)) {
          if (strstr($from_file, 'problem_notes.txt')) {
            $command = 'rm -f ' . $from_file;
          }
          else {
            $command = 'mv ' . $from_file . ' ' . $to_file;
          }
          $output = $return_var = array();
          exec($command, $output, $return_var);
          $success_moving |= (count($output) < 1);
        }
      }
      if ($success_moving) {
        // Done moving the files, but their sub-folders still exist and need to \
        // be removed.
        foreach ($files as $file) {
          $from_file = $source_path . '/' . $file;
          $file_pathinfo = pathinfo($from_file);
          $from_file_dirname = $file_pathinfo['dirname'];
          if (($source_path <> $from_file_dirname) && file_exists($from_file_dirname)) {
            rmdir($from_file_dirname);
          }
        }
        if (file_exists($source_path)) {
          // Done moving subfolders, now we can remove that empty folder from
          // the staging root.
          rmdir($source_path);
        }
        //TODO: update the "problem_resolved_timestamp" value for the related problem state record of the item.
        
      }
    }
  }
  else {
    drupal_set_message('The staging folder did not contain a folder that matched the batch name of "' . $batch_name . '".', 'warning');
  }
  drupal_set_message('Form submitted');
}
