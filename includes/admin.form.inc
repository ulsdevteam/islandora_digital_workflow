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
  // module_load_include('inc', 'islandora_digital_workflow', 'includes/access_permissions');
  // Needed for collapsible fieldsets
  drupal_add_js('misc/collapse.js');
  $about_islandora_digital_workflow_markup = '<p>These are the settings for ' .
      'the <strong>Islandora Digital Workflow</strong> system.</p>';
  $default_colors_json = '{"New":{"background-color":"#997","color":"#220"},"Scanned"' .
      ':{"background-color":"#472","color":"#FFF"},"Problem":{"' .
      'background-color":"#F00","color":"#FF3"},"Reviewed":{"' .
      'background-color":"#237","color":"#CCC"},"Done":{"' .
      'background-color":"#2C2","color":"#349"}}';
  $default_colors = json_decode($default_colors_json, TRUE);
  $stage_values = array_merge($default_colors, json_decode(variable_get('islandora_digital_workflow_stages', $default_colors_json), TRUE));

  $rendered_stages = array();
  foreach ($stage_values as $stage => $colors) {
    $rendered_stages[$stage] = '<span id="color_' . $stage . '" style="width:110px; color:' .
        $colors['color'] . ';background-color:' . $colors['background-color'] .
        '" class="stage_block stage_' . $stage . '">' . $stage . '</span>';
  }
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
      '#collapsed' => TRUE,
      'user_roles_frame' => $models_table,
    ),

    // Ingest Options
    'ingest_fields' => array(
      '#type' => 'fieldset',
      '#title' => t('Ingest options'),
      'ingest_namespace' => array(
        '#type' => 'textfield',
        '#title' => t('Ingest namespace'), // Incoming Ingest Path'),
        '#description' => t('The namespace for objects that get ingested for ' .
            'batches by default.  Each batch can override this value.'),
        '#default_value' => variable_get('islandora_digital_workflow_ingest_namespace', 'islandora'),
      ),
      'ingest_prepared_path' => array(
        '#type' => 'textfield',
        '#title' => t('Ingest Working Directory'), // Incoming Ingest Path'),
        '#description' => t('The working directory for prepared ingest batch files.  This path
                             should be configured to a location that has plenty of hard space.'),
        '#default_value' => variable_get('islandora_digital_workflow_ingest_prepared_path', '/ingest/islandora_ingest'),
      ),
    ),

    'extra_options' => array(
      '#type' => 'fieldset',
      '#title' => t('Extra batch properties & options'),
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

    // Stages
    'stages_frame' => array(
      '#type' => 'fieldset',
      '#title' => t('Stages'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      'colors' => array(
        '#type' => 'container',
        '#prefix' => 'For the color settings, use any legal CSS color definition.',
        'stage_problem_frame' => array(
          '#type' => 'fieldset',
          '#title' => $rendered_stages['Problem'],
          '#collapsible' => TRUE,
          '#collapsed' => TRUE,
          'stage_Problem_background_color' => array(
            '#type' => 'textfield',
            '#title' => t('Background'),
            '#description' => 'Default value: ' . $stage_values['Problem']['background-color'],
            '#default_value' => $stage_values['Problem']['background-color'],
          ),
          'stage_Problem_color' => array(
            '#type' => 'textfield',
            '#title' => t('Text Color'),
            '#description' => 'Default value: ' . $stage_values['Problem']['color'],
            '#default_value' => $stage_values['Problem']['color'],
          ),
        ),
        'stage_new_frame' => array(
          '#type' => 'fieldset',
          '#title' => $rendered_stages['New'],
          '#collapsible' => TRUE,
          '#collapsed' => TRUE,
          'stage_New_background_color' => array(
            '#type' => 'textfield',
            '#title' => t('Background'),
            '#description' => 'Default value: ' . $stage_values['New']['background-color'],
            '#default_value' => $stage_values['New']['background-color'],
          ),
          'stage_New_color' => array(
            '#type' => 'textfield',
            '#title' => t('Text Color'),
            '#description' => 'Default value: ' . $stage_values['New']['color'],
            '#default_value' => $stage_values['New']['color'],
          ),
        ),
        'stage_scanned_frame' => array(
          '#type' => 'fieldset',
          '#title' => $rendered_stages['Scanned'],
          '#collapsible' => TRUE,
          '#collapsed' => TRUE,
          'stage_Scanned_background_color' => array(
            '#type' => 'textfield',
            '#title' => t('Background'),
            '#description' => 'Default value: ' . $stage_values['Scanned']['background-color'],
            '#default_value' => $stage_values['Scanned']['background-color'],
          ),
          'stage_Scanned_color' => array(
            '#type' => 'textfield',
            '#title' => t('Text Color'),
            '#description' => 'Default value: ' . $stage_values['Scanned']['color'],
            '#default_value' => $stage_values['Scanned']['color'],
          ),
        ),
        'stage_reviewed_frame' => array(
          '#type' => 'fieldset',
          '#title' => $rendered_stages['Reviewed'],
          '#collapsible' => TRUE,
          '#collapsed' => TRUE,
          'stage_Reviewed_background_color' => array(
            '#type' => 'textfield',
            '#title' => t('Background'),
            '#description' => 'Default value: ' . $stage_values['Reviewed']['background-color'],
            '#default_value' => $stage_values['Reviewed']['background-color'],
          ),
          'stage_Reviewed_color' => array(
            '#type' => 'textfield',
            '#title' => t('Text Color'),
            '#description' => 'Default value: ' . $stage_values['Reviewed']['color'],
            '#default_value' => $stage_values['Reviewed']['color'],
          ),
        ),
        'stage_done_frame' => array(
          '#type' => 'fieldset',
          '#title' => $rendered_stages['Done'],
          '#collapsible' => TRUE,
          '#collapsed' => TRUE,
          'stage_Done_background_color' => array(
            '#type' => 'textfield',
            '#title' => t('Background'),
            '#description' => 'Default value: ' . $stage_values['Done']['background-color'],
            '#default_value' => $stage_values['Done']['background-color'],
          ),
          'stage_Done_color' => array(
            '#type' => 'textfield',
            '#title' => t('Text Color'),
            '#description' => 'Default value: ' . $stage_values['Done']['color'],
            '#default_value' => $stage_values['Done']['color'],
          ),
        ),
      ),
    ),

    // Rules
    'rules_frame' => array(
      '#type' => 'fieldset',
      '#title' => t('Rules'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      'rules_one' => array(
        '#type' => 'textfield',
        '#title' => t('Rules "One"'),
        '#description' => t('Placeholder for actual settings.'),
        '#default_value' => variable_get('islandora_digital_workflow_rules_one', ''),
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
 * Function that sets the Drupal variables with user's input.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_digital_workflow_admin_form_submit(array $form, array &$form_state) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/access_permissions');
  $id = $form_state['triggering_element']['#id'];
  module_load_include('inc', 'islandora_digital_workflow', 'includes/islandora_digital_workflow_css');
  switch ($id) {
    case 'edit-submit':
      $stage_values = json_encode(array(
        'New' => array(
            'color' => $form_state['values']['stage_New_color'],
            'background-color' => $form_state['values']['stage_New_background_color'],
        ),
        'Scanned' => array(
            'color' => $form_state['values']['stage_Scanned_color'],
            'background-color' => $form_state['values']['stage_Scanned_background_color'],
        ),
        'Problem' => array(
            'color' => $form_state['values']['stage_Problem_color'],
            'background-color' => $form_state['values']['stage_Problem_background_color'],
        ),
        'Reviewed' => array(
            'color' => $form_state['values']['stage_Reviewed_color'],
            'background-color' => $form_state['values']['stage_Reviewed_background_color'],
        ),
        'Done' => array(
            'color' => $form_state['values']['stage_Done_color'],
            'background-color' => $form_state['values']['stage_Done_background_color'],
        ),
      ));
      islandora_digital_workflow_save_models($form_state['values']);
      variable_set('islandora_digital_workflow_stages', $stage_values);
      variable_set('islandora_digital_workflow_rules_one', $form_state['values']['rules_one']);
      variable_set('islandora_digital_workflow_ingest_namespace', $form_state['values']['ingest_namespace']);
      variable_set('islandora_digital_workflow_ingest_prepared_path', $form_state['values']['ingest_prepared_path']);
      variable_set('islandora_digital_workflow_use_isMemberOfSite', $form_state['values']['batch_use_isMemberOfSite']);
      islandora_digital_workflow_update_dynamic_css();
      drupal_set_message('[' . date('H:i:s') . '] The settings have been saved.');
      // drupal_goto('admin/islandora/islandora_digital_workflow');
      break;
    case 'edit-reset':
      variable_del('islandora_digital_workflow_islandora_models');
      variable_del('islandora_digital_workflow_stages');
      variable_del('islandora_digital_workflow_rules_one');
      variable_del('islandora_digital_workflow_ingest_namespace');
      variable_del('islandora_digital_workflow_ingest_prepared_path');
      variable_del('islandora_digital_workflow_use_isMemberOfSite');

      islandora_digital_workflow_update_dynamic_css();
      drupal_set_message('[' . date('H:i:s') . '] The settings have been reset.');
      break;
  }
}
