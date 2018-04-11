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
  $user_role_permissions_table = islandora_digital_workflow_user_role_permissions_table();

  $form = array(
    'about' => array(
      '#type' => 'item',
      '#markup' => $about_islandora_digital_workflow_markup,
    ),
    // Islandora Objects
    'islandora_frame' => array(
      '#type' => 'fieldset',
      '#title' => t('Supported Models and Role Permissions'),
      'user_roles_frame' => $user_role_permissions_table,
    ),

    // Ingest Options
    'ingest_fields' => array(
      '#type' => 'fieldset',
      '#title' => t('Ingest options'),
      'ingest_prepared_path' => array(
        '#type' => 'textfield',
        '#title' => t('Ingest Working Directory'), // Incoming Ingest Path'),
        '#description' => t('The working directory for prepared ingest batch files.  This path
                             should be configured to a location that has plenty of hard space.'),
        '#default_value' => variable_get('islandora_digital_workflow_ingest_prepared_path', '/ingest/islandora_ingest'),
      ),
    ),

    // Stages
    'stages_frame' => array(
      '#type' => 'fieldset',
      '#title' => t('Stages'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      'colors' => array(
        '#type' => 'container',
        '#suffix' => '<small>For the color settings, use any legal CSS color definition.</small>',
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
      islandora_digital_workflow_save_roles_and_models($form_state['values']);
      variable_set('islandora_digital_workflow_stages', $stage_values);
      variable_set('islandora_digital_workflow_rules_one', $form_state['values']['rules_one']);
      variable_set('islandora_digital_workflow_ingest_prepared_path', $form_state['values']['ingest_prepared_path']);

      islandora_digital_workflow_update_dynamic_css();
      drupal_set_message('[' . date('H:i:s') . '] The settings have been saved.');
      // drupal_goto('admin/islandora/islandora_digital_workflow');
      break;
    case 'edit-reset':
      variable_del('islandora_digital_workflow_islandora_models');
      variable_del('islandora_digital_workflow_stages');
      variable_del('islandora_digital_workflow_user_role_permissions');
      variable_del('islandora_digital_workflow_rules_one');
      variable_del('islandora_digital_workflow_ingest_prepared_path');
      variable_del('islandora_digital_workflow_islandora_models');
      variable_del('islandora_digital_workflow_roles_and_models');

      islandora_digital_workflow_update_dynamic_css();
      drupal_set_message('[' . date('H:i:s') . '] The settings have been reset.');
      break;
  }
}

function islandora_digital_workflow_user_role_permissions_table() {
  $roles = array('DRL', 'MAD', 'Curator', 'Workflow user');
  $models = islandora_digital_workflow_get_reduced_cmodels();

  $user_role_permissions = variable_get('islandora_digital_workflow_user_role_permissions', '');
  $roles_and_models = variable_get('islandora_digital_workflow_roles_and_models', '');

  $readable_models = islandora_digital_workflow_get_reduced_readable_cmodels();
  $configured_models = $role_permissions = $return_form_elements = array();
  $configured_supported_models = variable_get('islandora_digital_workflow_islandora_models', array());

  foreach ($readable_models as $cmodel => $model) {
    $configured_models[$cmodel] = array(
      '#type' => 'checkbox',
      '#title' => $model,
      // Mark the checkboxes as checked based on islandora_digital_workflow_islandora_models
      '#default_value' => isset($configured_supported_models[$cmodel]) ? (($configured_supported_models[$cmodel] <> 0) ? 1 : 0) : 0,
    );
    $user_role_checkboxes = array();
    foreach ($roles as $role) {
      $role_model = $role . '/' . $cmodel;
      $fieldset = array(
        '#type' => 'item',
        '#prefix' => '<div class="indent-item">',
        '#suffix' => '</div>',
      );
      $user_role_checkboxes[$role_model]['#title'] = $role;
      // $role_permissions[$role_model][$role]['#title_display'] = 'invisible';
      $user_role_checkboxes[$role_model]['#type'] = 'checkbox';
      $user_role_checkboxes[$role_model]['#default_value'] = $roles_and_models[$role][$cmodel];;
      $fieldset['checkboxes'] = $user_role_checkboxes;
      $role_permissions[$cmodel] = $fieldset;
    }
    $return_form_elements[$model] = $configured_models[$cmodel];
    $return_form_elements[] = $role_permissions[$cmodel];
  }
  return $return_form_elements;
}

function islandora_digital_workflow_save_roles_and_models($values) {
  $readable_models = islandora_digital_workflow_get_reduced_readable_cmodels();
  $configured_islandora_models = array();
  foreach ($readable_models as $cmodel => $model) {
    $configured_islandora_models[$cmodel] = $values[$model];
  }
  variable_set('islandora_digital_workflow_islandora_models', $configured_islandora_models);
  // Loop through the $form_state values - to get the correct checkboxes based
  // on role and model name.
  $roles_and_models_values = array();
  foreach ($values as $key => $value) {
    if (strstr($key, "/") <> '') {
      @list($role, $model) = explode("/", $key);
      $roles_and_models_values[$role][$model] = $value;
    }
  }
  variable_set('islandora_digital_workflow_roles_and_models', $roles_and_models_values);
}
