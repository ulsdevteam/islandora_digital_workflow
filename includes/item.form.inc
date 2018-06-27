<?php

/**
 * View a single batch_item
 *
 * @param object $node
 *   Drupal node.
 * @param integer $batch_item_id
 *   The id value for the batch_item record to display.
 * @return string
 *   The rendered HTML.
 */
function islandora_digital_workflow_items_view_item($node, $batch_item_id) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/lookup.form');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
  $lookup_form_markup = theme('islandora_digital_workflow_lookup', array('searchterm' => ''));

  $batch_name = $node->title;
  $results = islandora_digital_workflow_find_batch_records_by_name($batch_name);
  $batch_record = array_pop($results);
  $batch_path = islandora_digital_workflow_batch_path($batch_record);

  $required_batch_actions = islandora_digital_workflow_get_required_batch_actions($batch_record->batch_id);
  if (is_array($required_batch_actions)) {
    $first_required_batch_actions = array_shift($required_batch_actions);
    $sequence_name = $first_required_batch_actions['sequence_name'];
    $workflow_sequence_text = '<div><h3>"' . $sequence_name .
        '" sequence actions:</h3>' .
        theme('islandora_digital_workflow_transactions_glyphs', array('transaction_actions' => $required_batch_actions)) .
        '</div><br>';
  }
  else {
    $workflow_sequence_text = '<em>Either there are no actions configured for this ' .
        'batch\'s "Workflow Sequence", or this batch does not have a workflow ' .
        'sequence associated with it.</em>  In order to enforce that certain transactions ' .
        'occur for this batch\'s objects, ' .
        l('select a "Workflow Sequence" for this batch', '/node/' . $node->nid . '/batch') . '.';
  }
  $paged_content_models = islandora_digital_workflow_retrieve_applicable_paged_content_cmodels();
  $is_paged_content = (!(array_search($batch_record->default_type_of_resource, $paged_content_models) === FALSE));
  // Did the user post an update to this batch item data?  If so, handle that update here
  if (isset($_POST['title'])) {
    $item_record_results = islandora_digital_workflow_get_batch_item_record($batch_item_id);
    $original_values = array_pop($item_record_results);
    if ($paged_content_models) {
      if (isset($_FILES['marc_file']) && isset($_FILES['marc_file']['tmp_name'])) {
        $marc = file_get_contents($_FILES['marc_file']['tmp_name']);
        $_POST['marc'] = $marc;
        @list($namespace, $item_identifier) = explode(":", $original_values->identifier);
        // Move the uploaded file to the $batch_path + specific item folder
        drupal_move_uploaded_file($_FILES['marc_file']['tmp_name'], $batch_path . '/' . $item_identifier . '/' . $_FILES['marc_file']['name']);
      }
    }
    $filename_pathinfo = pathinfo($original_values->filename);
    $filename_folder = str_replace($filename_pathinfo['basename'], '', $original_values->filename);
    $_POST['filename'] = $filename_folder . $_POST['filename'];
    $records_updated = islandora_digital_workflow_update_batch_item_record($batch_item_id, $_POST, $original_values);
    if ($records_updated) {
      drupal_set_message(t('This batch object record has been updated <br>' .
          '<b>Previous Title:</b> "' . $original_values->title . '"<br>' .
          '<b>Updated Title:</b> "<span class="' . 
              (($_POST['title'] <> $original_values->title) ? 'good' : '') .
              '">' . $_POST['title'] . '</span>"<br>' .
          '<b>Previous Identifier:</b> "' . $original_values->identifier . '"<br>' .
          '<b>Updated Identifier:</b> "<span class="' . 
              (($_POST['identifier'] <> $original_values->identifier) ? 'good' : '') .
              '">' . $_POST['identifier'] . '</span>"<br>' .
          '<b>Previous Filename:</b> "' . $original_values->filename . '"<br>' .
          '<b>Updated Filename:</b> "<span class="' . 
              (($_POST['filename'] <> $original_values->filename) ? 'good' : '') .
              '">' . $_POST['filename'] . '</span>"'));
    }
  }

  $item_record_results = islandora_digital_workflow_get_batch_item_record($batch_item_id);

  $item = array_pop($item_record_results);
  @list($namespace, $pid) = explode(":", $item->identifier);

  $paged_content_models = islandora_digital_workflow_retrieve_applicable_paged_content_cmodels();
  if (!(array_search($batch_record->default_type_of_resource, $paged_content_models) === FALSE)) {
    $scanned_files = islandora_digital_workflow_scan_paged_content_files($batch_path . '/' . $pid);
  }
  else {
    $file_records = islandora_digital_workflow_get_batch_files_by_nid($batch_record->nid);
    $scanned_files = islandora_digital_workflow_scan_files($batch_path . '/' . $pid, $file_records);
  }

  $item_record_transactions = islandora_digital_workflow_get_batch_item_transactions($batch_item_id, $node->nid);

  return $lookup_form_markup . 
      theme('islandora_digital_workflow_item', array(
        'batch_record' => $batch_record,
        'item' => $item,
        'is_paged_content' => $is_paged_content,
        'workflow_sequence_text' => $workflow_sequence_text,
        'can_update' => user_access(ISLANDORA_DIGITAL_WORKFLOW_UPDATE_ITEMS),
        'item_record_transactions' => $item_record_transactions,
        'pid' => $pid,
        'scanned_files' => $scanned_files));
}
