<?php

function islandora_digital_workflow_create_node_from_form_data($form_state) {
  islandora_digital_workflow_sync_node_batch_record_info(NULL, $form_state, TRUE);
}

/**
 * This will update the batch_field_defaults field for the workflow node object
 * based on the underlying batch record for display purposes only.
 *
 * If this is called with an existing $node, it will be updated -- else a new
 * node object is created with type="workflow_batch" and field_batch_name to
 * match the form value for "batch name".
 *
 * @param object $node
 *   Standard drupal node object.
 */
function islandora_digital_workflow_sync_node_batch_record_info($node, $form_state, $create_new = FALSE) {
  dpm('in islandora_digital_workflow_sync_node_batch_record_info');
  dpm($form_state);
  if ($create_new) {
    $node = islandora_digital_workflow_make_new_node($form_state);
  }
  $batch_record = islandora_digital_workflow_load_batch_record_for_node($node, $form_state);
  if (is_array($batch_record)) {
    $new_value = theme('islandora_digital_workflow_batch', array('islandora_digital_workflow_batch' => $batch_record));
    $node->field_batch_defaults[LANGUAGE_NONE][0]['value'] = $new_value;
    // Make sure that the format is Full HTML here so it doesn't strip any HTML tags.
    $node->field_batch_defaults[LANGUAGE_NONE][0]['format'] = 'full_html';
    field_attach_update('node', $node);
  }
}

/**
 * Loads the batch record that corresponds to a given drupal $node.
 *
 * @param object $node
 *   Standard drupal node object.
 * @return mixed
 *   either an associative array of batch record values or NULL
 */
function islandora_digital_workflow_load_batch_record_for_node($node, $form_state = array()) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
//  $bt = debug_backtrace();die('<pre>'.print_r($bt, true));
  $batch_name = @$node->field_batch_name['und'][0]['value'];
  return islandora_digital_workflow_get_batch_record_by_name($batch_name);
}

function islandora_digital_workflow_make_new_node($form_state) {
  global $user;
  $node = new stdClass();
  $node->title = $form_state['values']['batch_name'];
  $node->type = "workflow_batch";
  // Sets some defaults. Invokes hook_prepare() and hook_node_prepare().
  node_object_prepare($node);
  // Or e.g. 'en' if locale is enabled.
  $node->language = LANGUAGE_NONE;
  $node->uid = $user->uid;
  // Status is 1 or 0; published or not.
  $node->status = 1;
  // Promote is 1 or 0; promoted to front page or not.
  $node->promote = 0;
  // Comment is 0, 1, 2; 0 = disabled, 1 = read only, or 2 = read/write.
  $node->comment = 0;

/*
  //
  // Text field
  $node->field_some_text[$node->language][]['value'] = "YOUR TEXT";

  // Term reference (taxonomy) field
  $node->field_product_tid[$node->language][]['tid'] = $form_state['values']['a taxonomy term id'];

  // Entity reference field.
  $node->field_customer_nid[$node->language][] = array(
    'target_id' => $form_state['values']['entity id'],
    // Default target_type is 'node', Other possible values are "user",
    // "taxonomy_term" or other valid entity machine name.
    'target_type' => 'node',
  );
*/
  // Set any specific CCK field values for the node object
  $node->field_batch_name['und'][0]['value'] = $form_state['values']['batch_name'];

  $node = node_submit($node); // Prepare node for saving
  node_save($node);
  return $node;
}