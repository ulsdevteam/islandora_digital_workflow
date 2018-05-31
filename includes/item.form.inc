<?php

function islandora_digital_workflow_items_view_item($node, $batch_item_id) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');

  $batch_name = $node->title;
  $results = islandora_digital_workflow_find_batch_records_by_name($batch_name);
  $batch_record = array_pop($results);
  
  // Did the user post an update to this batch item data?  If so, handle that update here
  if (isset($_POST['title'])) {
    $item_record_results = islandora_digital_workflow_get_batch_item_record($batch_item_id);
    $original_values = array_pop($item_record_results);
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
  $item_record_transactions = islandora_digital_workflow_get_batch_item_transactions($batch_item_id);

  return theme('islandora_digital_workflow_item', array(
        'batch_record' => $batch_record,
        'item' => $item,
        'can_update' => user_access(ISLANDORA_DIGITAL_WORKFLOW_UPDATE_ITEMS),
        'item_record_transactions' => $item_record_transactions));
}
