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
  foreach ($batch_staging_folders as $found_staging_batch_name => $info) {
    if ($found_staging_batch_name <> $batch_name) {
      unset($batch_staging_folders[$found_staging_batch_name]);
    }
  }
  $markup = (count($batch_staging_folders)) ?
    theme('islandora_digital_workflow_files_table', array(
      'folders' => $batch_staging_folders,
      'root' => $batch_staging_root,
      'action_link' => '',
      'table_title' => 'Items moved to the "Staging" folder location',
      'table_description' => '!!!!!!Please resolve by using the Sync operation for each of the folders listed below:',
    )) : '';
  
  $form = array(
      'foo' => array(
          '#type' => 'item',
          '#title' => $batch_name,
          '#prefix' => $markup,
      ),
  );
  $form['submit'] = array(
      '#type' => 'submit',
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
  drupal_set_message('form submitted');
}
