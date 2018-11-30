<?php

/**
 * @file
 * Lookup form and submit handling for Islandora Digital Workflow module.
 */

/**
 * Handler for menu route "islandora/islandora_digital_workflow/lookup".  This
 * simply redirects to the corrislandora_digital_workflow_lookup_form_submitect handler without the "q=" part.
 */
function islandora_digital_workflow_lookup() {
  $q = isset($_POST['q']) ? trim($_POST['q']) : NULL;
  if ($q) {
    drupal_goto('/islandora/islandora_digital_workflow/lookup/' . $q);
  }
  else {
    $refer_url = $_SERVER['HTTP_REFERER'];
    drupal_set_message(t('The lookup search term can not be blank. You have been redirected to the page from which you came.'), 'warning');
    drupal_goto($refer_url);
  }
}

/**
 * Function that sets the Drupal variables with user's input.
 *
 * @param string $searchterm
 *   The search term.
 */
function islandora_digital_workflow_lookup_form_submitted($searchterm = '') {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
  // Try to load a islandora_digital_workflow_batch record by name ... if it 
  // does not come back, the searchterm may relate to an identifier that is
  // associated with a batch.
  $breadcrumb = array(
    l(t('Home'), '<front>'),
    l(t('Islandora Digital Workflow'), 'islandora/islandora_digital_workflow'),
  );
  if ($searchterm) {
    $breadcrumb[] = 'Searched for "' . $searchterm . '"';
  }
  drupal_set_breadcrumb($breadcrumb);

  $matched_csv_only = FALSE;
  $reason = '';
  $found_records = array();
  $found_total = 0;
  $records = islandora_digital_workflow_find_batch_records_by_name($searchterm);
  if (count($records) > 0) {
    $found_records['batch name'] = $records;
  }
  $found_total += ((is_array($records)) ? count($records) : 0);
  $records = islandora_digital_workflow_find_batch_records_by_description($searchterm);
  if (count($records) > 0) {
    $found_records['batch description'] = $records;
  }
  $found_total += ((is_array($records)) ? count($records) : 0);
  $records = islandora_digital_workflow_get_batch_record_by_items_identifier($searchterm);
  if (count($records) > 0) {
    $found_records['batch item identifier'] = $records;
  }
  $found_total += ((is_array($records)) ? count($records) : 0);
  $records = islandora_digital_workflow_search_batch_csv_by_identifier($searchterm);
  if (count($records) > 0) {
    $found_records['uploaded CSV file'] = $records;
  }
  $found_total += ((is_array($records)) ? count($records) : 0);
  if (module_exists('islandora_digitization_requests')) {
    $records = islandora_digital_workflow_search_digitization_requests($searchterm);
    if (count($records) > 0) {
      $found_records['digitization requests'] = $records;
    }
    $found_total += ((is_array($records)) ? count($records) : 0);
  }

  if ($found_total == 1) {
    foreach ($found_records as $reason => $records) {
    }
    // If there is only one result, jump to that page --- else, present the
    // results as a list that can be used to jump to each.
    if (count($records) < 2) {
      // If this is a batch record, display the message that the redirect happened
      // due to the match $reason.
      if ($reason <> 'digitization requests') {
        drupal_set_message(t('Located batch "@batch_name" by an item that matched ' .
            '"@searchterm" <em>in <b>@reason</b> values</em>', // 'identifier value "@searchterm".',
            array('@batch_name' => $records[0]->batch_name,
                '@searchterm' => $searchterm,
                '@reason' => $reason)));
        if ($records[0]->nid) {
          drupal_goto('node/' . $records[0]->nid . '/batch');
        }
        else {
          drupal_goto('islandora/islandora_digital_workflow/edit_batch/' . $records[0]->batch_name);
        }
      }
      else {
        // This record must be a digitization request... display a slightly
        // different message and redirect to the node/171/submission/1 handler
        // to view it.
        drupal_set_message(t('Located a digitization request that contained the value ' .
            '"@searchterm"',
            array('@searchterm' => $searchterm)));
        drupal_goto('node/' . $records[0]->nid . '/submission/' . $records[0]->sid);
      }
    }
  }
  if ($found_total > 1) {
    $markup = theme('islandora_digital_workflow_lookup', array('searchterm' => ''));
    // reorder the found_batch_records so that it removes duplicates and sorts
    // by the number of match reasons.
    $found_records = islandora_digital_workflow_ungroup_search_records($found_records);
    return $markup . islandora_digital_workflow_highlight(
      islandora_digital_workflow_make_content_clean_output_to_highlight(
        theme('islandora_digital_workflow_lookup_results', array(
          'records' => $found_records,
          'results_count' => count($found_records),
          'searchterm' => $searchterm,
          'matched_csv_only' => $matched_csv_only,
        )
      )
    ),
    $searchterm);
  }
  else {
    drupal_set_message(t('"@searchterm" could not be located.', array('@searchterm' => $searchterm)), 'message_info');
    drupal_goto('islandora/islandora_digital_workflow/');
  }
}

/**
 * This returns some "cleaned" output that is ready to be highlighted.
 *
 * @param string $content
 *   HTML content that needs to be cleaned for search.
 * @return string
 *   "cleaned" HTML content ready for highlighting.
 */
function islandora_digital_workflow_make_content_clean_output_to_highlight($content) {
  $content = htmlspecialchars_decode(str_replace(array(
      '&nbsp;',
    ), array(
      ' ',
    ), $content));
  return $content;
}

/**
 * This will ungroup the search results arrays that contain arrays of results
 * that are keyed by the reason they match.  The final array is sorted by the
 * # of matched reasons (reverse high to low).
 *
 * @param array $found_batch_records
 *   An array of batch records that matched a search, but the results are each
 * grouped into a sub-array with the key of that array being their "match reason".
 * @return array
 *   An array of search results (batch records objects) with an additional
 * property that specifies their reasons of matching the search.
 */
function islandora_digital_workflow_ungroup_search_records($found_batch_records) {
  $raw_records_found = $records_found = $keysort_results = array();
  foreach ($found_batch_records as $reason => $found_records) {
    foreach ($found_records as $found_record_obj) {
      if (!(array_key_exists($found_record_obj->batch_id, $raw_records_found))) {
        $raw_records_found[$found_record_obj->batch_id] = $found_record_obj;
      }
      if (!isset($raw_records_found[$found_record_obj->batch_id]->reasons)) {
        $raw_records_found[$found_record_obj->batch_id]->reasons = array($reason);
      }
      else {
        $raw_records_found[$found_record_obj->batch_id]->reasons[] = $reason;
      }
    }
  }
  foreach ($raw_records_found as $batch_id => $found_object) {
    $reasons_count = count($found_object->reasons);
    $keysort_results[$reasons_count][] = $found_object;
  }
  ksort($keysort_results);
  $keysort_results = array_reverse($keysort_results);
  foreach ($keysort_results as $result) {
    if (is_array($result)) {
      foreach ($result as $result_item) {
        $records_found[] = $result_item;
      }
    }
    else {
      $records_found[] = $result;
    }
  }
  return $records_found;
}