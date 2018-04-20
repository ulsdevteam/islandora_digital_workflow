<?php

function islandora_digital_workflow_permission_constant_name_of_model($model_name) {
  return 'ISLANDORA_DIGITAL_WORKFLOW_' . str_replace(" ", "_", strtoupper($model_name));
}

function islandora_digital_workflow_permission_of_model($readable_model) {
  return 'islandora digital workflow ' . $readable_model;
}

function islandora_digital_workflow_save_models($values) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  $readable_models = islandora_digital_workflow_get_reduced_readable_cmodels();
  $configured_islandora_models = array();
  foreach ($readable_models as $cmodel => $model) {
    $configured_islandora_models[$cmodel] = $values[str_replace(" ", "+", $model)];
  }
  variable_set('islandora_digital_workflow_islandora_models', $configured_islandora_models);
}

function islandora_digital_workflow_user_access_cmodel($match_cmodel, $configured_roles_and_models, $user_roles) {
  // Assume no access -- the subsequent checks may provide access.
  $access = FALSE;
  foreach ($configured_roles_and_models as $role => $cmodel_matrix) {
    $access |= (!(array_search($match_cmodel, $cmodel_matrix) === FALSE) && $user_roles[$role] == 1 && $cmodel_matrix[$match_cmodel] == 1);
  }
  return $access;
}