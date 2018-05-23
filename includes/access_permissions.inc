<?php

/**
 * Used for setting up module permissions.
 *
 * @param string $model_name
 *   Name of the Islandora model.
 * @return type
 *   The permission constant for the given Islandora model.
 */
function islandora_digital_workflow_permission_constant_name_of_model($model_name) {
  return 'ISLANDORA_DIGITAL_WORKFLOW_' . str_replace(" ", "_", strtoupper($model_name));
}

/**
 * Given a human-readable Islandora model name, this returns the permission name.
 *
 * This is used for checking permissions throughout the module such as:
 *   [islandora digital workflow Manuscript]
 *
 * @param string $readable_model
 *   Human-readable name of the Islandora model.
 * @return string
 *   Permission name related to a given Islandora model.
 */
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
