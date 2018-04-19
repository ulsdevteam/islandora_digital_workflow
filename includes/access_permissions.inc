<?php

/**
 * Helper function that returns whether or not the user has permission.
 *
 * @global object $user
 *   A standard drupal user object.
 * @param string $permission_constant
 *   Constant that represents any permission
 * @return boolean
 *   Whether or not the user has access to $permission_constant.
 */
function islandora_digital_workflow_user_has_access($permission_constant) {
  global $user;
  return ($permission_constant) ? user_access($permission_constant, $user) : FALSE;
}

function islandora_digital_workflow_user_roles() {
  global $user;
  // Based on the user's permissions that are configured under the Site's
  // permissions, we can derive the set of roles that they have.
  $user_is_roles = array();
  $user_is_curator = user_access(ISLANDORA_DIGITAL_WORKFLOW_IS_CURATOR, $user);
  $user_is_mad = user_access(ISLANDORA_DIGITAL_WORKFLOW_IS_MAD, $user);
  $user_is_drl = user_access(ISLANDORA_DIGITAL_WORKFLOW_IS_DRL, $user);
  $user_is_workflow_user = user_access(ISLANDORA_DIGITAL_WORKFLOW, $user);

  if ($user_is_curator) {
    $user_is_roles[] = 'Curator';
  }
  if ($user_is_drl) {
    $user_is_roles[] = 'DRL';
  }
  if ($user_is_mad) {
    $user_is_roles[] = 'MAD';
  }
  if ($user_is_workflow_user) {
    $user_is_roles[] = 'Workflow user';
  }
  return array(
    'Curator' => $user_is_curator,
    'DRL' => $user_is_drl,
    'MAD' => $user_is_mad,
    'Workflow user' => $user_is_workflow_user,
    'role' => implode(", ", $user_is_roles));
}

function islandora_digital_workflow_save_roles_and_models($values) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  $readable_models = islandora_digital_workflow_get_reduced_readable_cmodels();
  $configured_islandora_models = array();
  foreach ($readable_models as $cmodel => $model) {
    $configured_islandora_models[$cmodel] = $values[str_replace(" ", "+", $model)];
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

function islandora_digital_workflow_user_role_configured_models() {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  $all_models = islandora_digital_workflow_get_reduced_cmodels();
  // Need to limit the models to what has been configured
  $configured_models = variable_get('islandora_digital_workflow_islandora_models', array());
  $supported_models_indices = array();
  $i = 1;
  foreach ($configured_models as $cmodel => $setting) {
    if ($setting) {
      $supported_models_indices[$i] = $cmodel;
    }
    $i++;
  }
  $configured_roles_and_models = variable_get('islandora_digital_workflow_roles_and_models', '');
  $user_roles = islandora_digital_workflow_user_roles();

  $models = array();
  // Loop through the $supported_models_indices and set $models accordingly
  $between_parenthesis_regex = '#\((.*?)\)#';
  foreach ($supported_models_indices as $index => $cmodel) {
    // Get the key name from $all_models -- $index SHOULD match an item, but
    // safer to check.
    if (array_key_exists($index, $all_models)) {
      $model_name_readable_and_model_name = $all_models[$index];
      preg_match($between_parenthesis_regex, $model_name_readable_and_model_name, $matches);

      if (count($matches) > 0) {
        $match_cmodel = $matches[1];
        $model_name_readable = trim(str_replace('(' . $match_cmodel . ')', '', $model_name_readable_and_model_name));
        // This step is required to restrict access to models based on user's roles.
        if (islandora_digital_workflow_user_access_cmodel($match_cmodel, $configured_roles_and_models, $user_roles)) {
          $models[$match_cmodel] = $model_name_readable;
        }
      }
    }
  }

  // Final step -- need to set the key to the vocabulary term tid
  $vocab = taxonomy_vocabulary_machine_name_load(IDW_CONTENT_TYPE_TAXONOMY_MACHINE_NAME);
  $tid_models = array();
  foreach ($models as $cmodel => $readable_name) {
    $readable_name = str_replace(" Content Model", "", $readable_name);
    $terms = taxonomy_get_term_by_name($readable_name, $vocab->machine_name);
    if (is_array($terms)) {
      $term_object = array_shift($terms);
      $tid_models[$term_object->tid] = $readable_name;
    }
  }
  return $tid_models;
}

function islandora_digital_workflow_user_role_permissions_table() {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
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
      '#prefix' => '<div class="major-item">',
      '#suffix' => '</div>',
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
    $return_form_elements[str_replace(" ", "+", $model)] = $configured_models[$cmodel];
    $return_form_elements[] = $role_permissions[$cmodel];
  }
  return $return_form_elements;
}

function islandora_digital_workflow_user_access_cmodel($match_cmodel, $configured_roles_and_models, $user_roles) {
  // Assume no access -- the subsequent checks may provide access.
  $access = FALSE;
  foreach ($configured_roles_and_models as $role => $cmodel_matrix) {
    $access |= (!(array_search($match_cmodel, $cmodel_matrix) === FALSE) && $user_roles[$role] == 1 && $cmodel_matrix[$match_cmodel] == 1);
  }
  return $access;
}