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
  // Needed for collapsible fieldsets
  drupal_add_js('misc/collapse.js');
  $about_islandora_digital_workflow_markup = '<p>These are the settings for ' .
      'the <strong>' . l(t('Islandora Digital Workflow'), '/islandora/islandora_digital_workflow') . '</strong> system.</p>';
  $models_table = islandora_digital_workflow_models_table();

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
      '#title' => t('Workflow rules'),
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
        '#title' => t('Ingest namespace'),
        '#description' => t('The namespace for objects that get ingested for ' .
            'batches by default.  Each batch can override this value.'),
        '#default_value' => variable_get('islandora_digital_workflow_ingest_namespace', 'islandora'),
      ),
      'ingest_staging_path' => array(
        '#type' => 'textfield',
        '#title' => t('Incoming Staging Directory'),
        '#description' => t('The staging directory is where the files will be after they are scanned.'),
        '#required' => true,
        '#default_value' => variable_get('islandora_digital_workflow_ingest_staging_path', ''),
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
      '#title' => t('Extra batch properties & options'),
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
      variable_set('islandora_digital_workflow_ingest_staging_path', $form_state['values']['ingest_staging_path']);
      variable_set('islandora_digital_workflow_ingest_prepared_path', $form_state['values']['ingest_prepared_path']);
      variable_set('islandora_digital_workflow_ingest_problems_path', $form_state['values']['ingest_problems_path']);
      variable_set('islandora_digital_workflow_hosts_for_ingest', $form_state['values']['batch_hosts']);
      variable_set('islandora_digital_workflow_use_isMemberOfSite', $form_state['values']['batch_use_isMemberOfSite']);
      drupal_set_message('[' . date('H:i:s') . '] The settings have been saved.');
      // drupal_goto('admin/islandora/islandora_digital_workflow');
      break;
    case 'edit-reset':
      variable_del('islandora_digital_workflow_islandora_models');
      variable_del('islandora_digital_workflow_ingest_namespace');
      variable_del('islandora_digital_workflow_ingest_staging_path');
      variable_del('islandora_digital_workflow_ingest_prepared_path');
      variable_del('islandora_digital_workflow_ingest_problems_path');
      variable_del('islandora_digital_workflow_hosts_for_ingest');
      variable_del('islandora_digital_workflow_use_isMemberOfSite');

      drupal_set_message('[' . date('H:i:s') . '] The settings have been reset.');
      break;
  }
}
