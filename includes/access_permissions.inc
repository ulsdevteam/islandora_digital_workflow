<?php

/**
 * @file
 * Unit to handle some dynamic permission handling for the various Islandora models.
 */

/**
 * Used for setting up module permissions.
 *
 * @param string $cmodel
 *   Name of the Fedora content model.
 * @return type
 *   The permission constant for the given Islandora model.
 */
function islandora_digital_workflow_permission_constant_name_of_model($cmodel) {
  return 'ISLANDORA_DIGITAL_WORKFLOW_' . str_replace(
      array(
          ":",
          " ",
      ),
      array(
          "",
          "_",
      ), strtoupper($cmodel));
}

/**
 * Given an Islandora model name, this returns the permission name.
 *
 * This is used for checking permissions throughout the module such as:
 *   [islandora digital workflow Manuscript]
 *
 * @param string $cmodel
 *   Name of the Fedora content model.
 * @return string
 *   Permission name related to a given Islandora model.
 */
function islandora_digital_workflow_permission_of_model($cmodel) {
  return 'islandora digital workflow ' . $cmodel;
}

/**
 * Saves the islandora model configuration settings.
 *
 * @param array $values
 *   Associative array of model setting values for module configuration.
 */
function islandora_digital_workflow_save_models($values) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  $readable_models = islandora_digital_workflow_get_reduced_readable_cmodels();
  $configured_islandora_models = array();
  foreach ($readable_models as $cmodel => $model) {
    $configured_islandora_models[$cmodel] = $values[str_replace(" ", "+", $model)];
  }
  variable_set('islandora_digital_workflow_islandora_models', $configured_islandora_models);
}
