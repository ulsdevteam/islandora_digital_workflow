<?php
/**
 * @file
 * Autocomplete functionality for content models within the Islandora Digital Workflow.
 */

/**
 * Autocomplete the content model name.
 *
 * @param string $string
 *   A search string.
 *
 * @return string
 *   The rendered JSON results.
 */
function islandora_digital_workflow_content_model_autocomplete($string) {
  if (($string == 'field_islandora_model') && (arg(4) <> '')) {
    $string = arg(4);
  }
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  $accessible_models = islandora_digital_workflow_get_cmodels_having_sequences();

  $content_models = array();
  foreach ($accessible_models as $tid => $result) {
    $content_models[$result['pid']] = "{$result['label']} ({$result['pid']})";
  }

  $output = array();
  foreach ($content_models as $model => $label) {
    if (preg_match("/{$string}/i", $label) !== 0) {
      $output[$model] = $label;
    }
  }
  return drupal_json_output($output);
}