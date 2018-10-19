<?php

/**
 * @file
 * Administration forms and submit handling for Islandora Digital Workflow module.
 */

/**
 * Administration and configuration form for islandora_digital_workflow.
 *
 * @ingroup forms
 *
 * @todo break out non-settings elements into their own form.
 *
 * @return array
 *   The Drupal form definition.
 */
function islandora_digital_workflow_admin_form() {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');

  islandora_digital_workflow_admin_form_check_private_files();
  islandora_digital_workflow_check_and_deploy_forena_reports();
  // Needed for collapsible fieldsets
  drupal_add_js('misc/collapse.js');
  $about_islandora_digital_workflow_markup = '<p>These are the settings for ' .
      'the <strong>' . l(t('Islandora Digital Workflow'), '/islandora/islandora_digital_workflow') . '</strong> system.</p>';
  $models_table = islandora_digital_workflow_models_table();
  $title = 'Islandora Digital Workflow - Configuration';
  drupal_set_title(t($title), CHECK_PLAIN);
  
  $form = array(
    'about' => array(
      '#type' => 'item',
      '#markup' => $about_islandora_digital_workflow_markup,
    ),
    // Islandora Objects
    'islandora_frame' => array(
      '#type' => 'fieldset',
      '#title' => t('Supported Models'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      'user_roles_frame' => $models_table,
    ),

    // Stages
    'stages_frame' => array(
      '#type' => 'fieldset',
      '#title' => t('Stages'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      'taxonomy_manage_link' => array(
        '#type' => 'item',
        '#title' => l(t('Islandora Digital Workflow Stage vocabulary'), 'admin/structure/taxonomy/workflow_stage_vocab'),
        '#description' => t('These terms use the Drupal core Taxonomy and should only ' .
          'be changed by advanced users.'),
      ),
    ),

    // Rules
    'rules_frame' => array(
      '#type' => 'fieldset',
      '#title' => t('Workflow Actions, Sequences, and Triggering "Stage" changes'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      'link_1' => array(
        '#type' => 'item',
        '#title' => l(t('Islandora Digital Workflow - Workflow Sequences'), 'admin/islandora/islandora_digital_workflow/workflow_sequences'),
        '#description' => t('Manage Workflow sequence actions related to each islandora model.'),
      ),
      'link_2' => array(
        '#type' => 'item',
        '#title' => l(t('Islandora Digital Workflow - Actions Configuration'), 'admin/islandora/islandora_digital_workflow/workflow_sequence_actions'),
        '#description' => t('Configure the actions that are used in workflow sequences.'),
      ),
      'link_3' => array(
        '#type' => 'item',
        '#title' => l(t('Islandora Digital Workflow rules'), 'admin/config/workflow/rules', array(
            'query' => array(
                'event' => 0,
                'tag' => 'Islandora Digital Workflow'
            ))),
        '#description' => t('Any new rules should be tagged with "Islandora Digital Workflow" ' .
          'so that they appear in this filtered list of rules for ease of management'),
      ),
    ),

    // Ingest Options
    'ingest_fields' => array(
      '#type' => 'fieldset',
      '#title' => t('Ingest options'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      'ingest_namespace' => array(
        '#type' => 'textfield',
        '#title' => t('Fedora object prefix (ingest namespace)'),
        '#description' => t('The namespace for objects that get ingested for ' .
            'batches by default.  Each batch can override this value.'),
        '#default_value' => variable_get('islandora_digital_workflow_ingest_namespace', 'islandora'),
      ),
      'ingest_assign_PIDs' => array(
        '#type' => 'checkbox',
        '#title' => t('Assign PID values'),
        '#description' => t('When set, ingested objects are created using the ' .
            '{Ingest namespace}:{item IDENTIFIER}, else the system will generate ' .
            'fedora PID values and update the items\' "islandora_pid" value.  ' .
            '<b>NOTE:</b> Using this option requires that all of the "batch ingest" '.
            'modules support the `--assign_pid=### parameters.'),
        '#default_value' => variable_get('islandora_digital_workflow_assign_PIDs', 0),
      ),
      'top_level_collection' => array(
        '#type' => 'textfield',
        '#title' => t('Top level collection'),
        '#description' => t('For the purpose of creating a valid link to make new ' . 
            'collections from the dashboard, this value represents the path the ' . 
            'parent object of all Islandora Collection objects.'),
        '#required' => true,
        '#default_value' => variable_get('islandora_digital_workflow_top_level_collection', '/islandora/object/islandora:root'),
      ),
      'ingest_delivery_path' => array(
        '#type' => 'textfield',
        '#title' => t('Incoming Delivery Directory'),
        '#description' => t('The delivery directory is where the files will be after they are scanned.'),
        '#required' => true,
        '#default_value' => variable_get('islandora_digital_workflow_ingest_delivery_path', ''),
      ),
      'ingest_prepared_path' => array(
        '#type' => 'textfield',
        '#title' => t('Ingest Working Directory'),
        '#description' => t('The working directory for prepared ingest batch files.  This path
                             should be configured to a location that has plenty of hard space.'),
        '#required' => true,
        '#default_value' => variable_get('islandora_digital_workflow_ingest_prepared_path', file_directory_temp()),
      ),
      'ingest_problems_path' => array(
        '#type' => 'textfield',
        '#title' => t('Problems Directory'),
        '#description' => t('The problems directory is needed to allow files to be moved out of the ingest path so that they can be replaced, renamed, etc.'),
        '#required' => true,
        '#default_value' => variable_get('islandora_digital_workflow_ingest_problems_path', ''),
      ),
      'ingest_ignore_filenames' => array(
        '#type' => 'textarea',
        '#title' => t('Ignore Specific Filenames'),
        '#default_value' => variable_get('islandora_digital_workflow_ingest_ignore_filenames'),
        '#attributes' => array(
            'placeholder' => 'ignored_filename_*
*_ignored_filenames
*_ignored_filenames_*
??_ignored_filenames'),
        '#description' => t('List filenames or wildcard filenames (supports *, ? characters) <b>ONE per line</b> that the ingest process should ignore when preparing the files that exist in the batch folder for ingest.
                             <p>When the files are managed in various environments, there are sometimes extra files left behind that are not intended as part of the ingest process (for example color calibration files such as "Target.tif", or hidden files or folders that might still cause a problem for the ingest).</p>'),
      ),
      'batch_hosts' => array(
        '#type' => 'textarea',
        '#title' => t('Available Host Aliases for Batch ingest'),
        '#default_value' => variable_get('islandora_digital_workflow_hosts_for_ingest', ''),
        '#description' => t('Server locations for prepared ingest batch files. Eacho of these should likely be configured to a location that has plenty of hard space.
                             <p>If the ingestable files are located on a different server, configure an alias
                             that can direct the batch to use that server.</p>
                             <ul>
                               <li>For any batch that does not specify which host alias to use, this "Ingest Working Directory" above would be used.</li>
                               <li>Values should each be entered on separate lines in a "name=value" format.  Username, password values are optional.  For example:
                                 <ul>
                                   <li>The value (part after the "=" character) should not be longer than 255 characters.</li>
                                   <li><code>ALIAS=username:password@machine_name:/path</code></li>
                                   <li><code>ALIAS=machine_name:/path</code></li>
                                 </ul>
                               </li>
                             </ul>'),
      ),
    ),

    'extra_options' => array(
      '#type' => 'fieldset',
      '#title' => t('Extra options'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      'batch_isMemberOfSite_property' => array(
        '#type' => 'item',
        '#title' => t('Batch "isMemberOfSite" property'),
        'batch_use_isMemberOfSite' => array(
          '#type' => 'checkbox',
          '#title' => 'Use the isMemberOfSite property?',
          '#description' => 'When this is checked, batch creation allows selecting ' .
            '"Sites" and passes the "sites" parameter to the islandora_batch drush ' .
            'prepare commands.<br /><em>NOTE: This feature requires that each site is ' .
            'ingested into fedora as pitt:siteCModel where their PID value is used for ' .
            '"object to site" relationships as well as that the instances of the various ' .
            'islandora_*_batch modules in order to handle the "sites" parameter.</em>',
          '#default_value' => variable_get('islandora_digital_workflow_use_isMemberOfSite', 0),
        ),
      ),
      'delete_delivery_when_scanned_property' => array(
        '#type' => 'item',
        '#title' => t('Delete files after scan copies made'),
        'delete_delivery_when_scanned' => array(
          '#type' => 'checkbox',
          '#title' => 'Delete delivery file after scan copy is made?',
          '#description' => 'When this is checked, the code will try to clean up ' .
            'the delivery folders as the items are marked as "scanned".  Additionally, ' .
            'when the final item is marked as "scanned", the containing folder will ' .
            'also be deleted.  <em>NOTE: This feature requires the file permissions ' .
            'to allow write from the web server.</em>',
          '#default_value' => variable_get('islandora_digital_workflow_delete_delivery_files', 0),
        ),
      ),
      'islandora_hook' => array(
        '#type' => 'item',
        '#title' => t('Handle the Islandora Ingested hooks'),
        'handle_islandora_hook' => array(
          '#type' => 'checkbox',
          '#title' => 'Handle the Islandora Object Ingested and Datastream Ingested hooks?',
          '#description' => 'When this is checked, the code will try to add a ' .
            'transaction for that batch item for "Ingest item into repository" or ' .
            '"System made derivatives".',
          '#default_value' => variable_get('islandora_digital_workflow_handle_islandora_hook', 0),
        ),
      ),
    ),

    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Save Settings'),
    ),
    'reset' => array(
      '#type' => 'submit',
      '#value' => t('Reset'),
    ),
  );
  return $form;
}

/**
 * To store the Drupal variables based on user's input.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_digital_workflow_admin_form_submit(array $form, array &$form_state) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/access_permissions');
  $id = $form_state['triggering_element']['#id'];
  switch ($id) {
    case 'edit-submit':
      islandora_digital_workflow_save_models($form_state['values']);
      variable_set('islandora_digital_workflow_ingest_namespace', $form_state['values']['ingest_namespace']);
      variable_set('islandora_digital_workflow_assign_PIDs', $form_state['values']['ingest_assign_PIDs']);
      variable_set('islandora_digital_workflow_top_level_collection', $form_state['values']['top_level_collection']);
      variable_set('islandora_digital_workflow_ingest_delivery_path', $form_state['values']['ingest_delivery_path']);
      variable_set('islandora_digital_workflow_ingest_prepared_path', $form_state['values']['ingest_prepared_path']);
      variable_set('islandora_digital_workflow_ingest_problems_path', $form_state['values']['ingest_problems_path']);
      variable_set('islandora_digital_workflow_ingest_ignore_filenames', $form_state['values']['ingest_ignore_filenames']);
      variable_set('islandora_digital_workflow_hosts_for_ingest', $form_state['values']['batch_hosts']);
      variable_set('islandora_digital_workflow_use_isMemberOfSite', $form_state['values']['batch_use_isMemberOfSite']);
      variable_set('islandora_digital_workflow_delete_delivery_files', $form_state['values']['delete_delivery_when_scanned']);
      variable_set('islandora_digital_workflow_handle_islandora_hook', $form_state['values']['handle_islandora_hook']);
      drupal_set_message('[' . date('H:i:s') . '] The settings have been saved.');
      // drupal_goto('admin/islandora/islandora_digital_workflow');
      break;
    case 'edit-reset':
      variable_del('islandora_digital_workflow_islandora_models');
      variable_del('islandora_digital_workflow_ingest_namespace');
      variable_del('islandora_digital_workflow_assign_PIDs');
      variable_del('islandora_digital_workflow_top_level_collection');
      variable_del('islandora_digital_workflow_ingest_delivery_path');
      variable_del('islandora_digital_workflow_ingest_prepared_path');
      variable_del('islandora_digital_workflow_ingest_problems_path');
      variable_del('islandora_digital_workflow_ingest_ignore_filenames');
      variable_del('islandora_digital_workflow_hosts_for_ingest');
      variable_del('islandora_digital_workflow_use_isMemberOfSite');
      variable_del('islandora_digital_workflow_delete_delivery_files');
      variable_del('islandora_digital_workflow_handle_islandora_hook');
      drupal_set_message('[' . date('H:i:s') . '] The settings have been reset.');
      break;
  }
}

/**
 * This routine will display a message if it is not able to write to the
 * configured "Private Path".
 */
function islandora_digital_workflow_admin_form_check_private_files() {
  $private_path = variable_get('file_private_path');
  if ($private_path) {
    $file_in_private_path = $private_path . '/testfile';
    file_put_contents($file_in_private_path, '');

    if (!file_exists($file_in_private_path)) {
      drupal_set_message(t('The "Private Path" is not writable by the web server.  Check the folder permissions on <br>@private_path', array(
          '@private_path' => $private_path)), 'warning');
    }
    else {
      @unlink($file_in_private_path);
    }
  }
  else {
    drupal_set_message(t('The "Private Path" value (') .
        l('Media | File System', 'admin/config/media/file-system') . 
        t(') is not set.'), 'error');
  }
}
