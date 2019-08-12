<?php

/**
 * @file
 * Lookup form and submit handling for Islandora Digital Workflow module.
 */

/**
 * Handler for menu route "islandora/islandora_digital_workflow/lookup".  This
 * simply redirects to the corrislandora_digital_workflow_lookup_form_submitect
 * handler without the "q=" part.
 */
function islandora_digital_workflow_lookup() {
  $q = isset($_POST['q']) ? trim($_POST['q']) : NULL;
  $request_uri = isset($_POST['request_uri']) ? trim($_POST['request_uri']) : 'islandora/islandora_digital_workflow';
  if (strlen($q) < 3) {
    drupal_set_message(t('The search term must be more than 2 characters. You ' .
        'have been redirected to the page from which you came.'), 'warning');
  }
  elseif ($q) {
    $_SESSION['request_uri'] = $request_uri;
    drupal_goto('/islandora/islandora_digital_workflow/lookup/' . $q);
  }
  else {
    drupal_set_message(t('The lookup search term can not be blank. You have been ' .
        'redirected to the page from which you came.'), 'warning');
  }
  drupal_goto($request_uri);
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
  // To preserve any instances of a possible search match, but that wasn't in
  // one of the searched fields.  This pertains to digitization request for now, 
  // but the code for other searches could also use the $highlight_hash to
  // temporarily replace those instances with a unique string that will be replaced
  // when the markup is returned.
  $highlight_hash = md5(date('h:m:s'));
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
    $records = islandora_digital_workflow_search_digitization_requests($searchterm, $highlight_hash);
    if (count($records) > 0) {
      $found_records['digitization requests'] = $records;
    }
    $found_total += ((is_array($records)) ? count($records) : 0);
  }
  $found_batch_ids = array();
  foreach ($found_records as $reason => $records) {
    foreach ($records as $found_object) {
      $found_batch_ids[$found_object->batch_id] = $found_object->batch_id;
    }
  }
  $found_keys = array_keys($found_records);
  // Also use a redirect when $found_total was more than one yet ONLY a single
  // batch is returned, for more than one reason.
  if ((count($found_batch_ids) == 1 && $found_total > 1) ||
      $found_total == 1) {
    // If there is only one result, jump to that page --- else, present the
    // results as a list that can be used to jump to each.
    if (count($records) < 2) {
      // Correct the reason in the event that there was only one batch
      // returned for more than one reason such as "batch name" AND "csv file contents".
      if (count($found_batch_ids) == 1 && $found_total > 1) {
        $reason = '"' . implode('", "', $found_keys) . '"';
      }
      // If this is a batch record, display the message that the redirect happened
      // due to the match $reason.
      if ($reason <> 'digitization requests') {
        drupal_set_message(t('Located batch "@batch_name" by an item that matched ' .
            '"@searchterm" <em>in <b>@reason</b> values</em>',
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

    // Must get the count of the secondary results after they have been grouped.
    $secondary_results_count = islandora_digital_workflow_get_digitization_requests_count($found_records);
    $found_markup = islandora_digital_workflow_make_content_clean_output_to_highlight(
        theme('islandora_digital_workflow_lookup_results', array(
          'records' => $found_records,
          'results_count' => count($found_records),
          'secondary_results_count' => $secondary_results_count,
          'searchterm' => $searchterm,
          'matched_csv_only' => $matched_csv_only,
        )
      )
    );

    // This part is needed in order to keep fields that are not marked as searchable
    // from displaying highlight matches since they did not match on that field
    // according to the configuration.
    $highlighted_markup = islandora_digital_workflow_highlight($found_markup, $searchterm);
    $searched_fields_only_highlighed_markup = str_replace($highlight_hash, $searchterm, $highlighted_markup);

    return $markup . $searched_fields_only_highlighed_markup;

  }
  else {
    drupal_set_message(t('"@searchterm" could not be located.', array('@searchterm' => $searchterm)), 'error');
    $request_uri = isset($_SESSION['request_uri']) ? trim($_SESSION['request_uri']) : 'islandora/islandora_digital_workflow';
    drupal_goto($request_uri);
  }
}

/**
 * This calculates the total number of digitization requests that are in the
 * given $found_records.
 *
 * @param array $found_records
 *   Array of found record objects -- either batch records or can be digitization
 * request submissions.
 * @return int
 *   The count of digitization requests from the given $found_records.
 */
function islandora_digital_workflow_get_digitization_requests_count($found_records) {
  $count = 0;
  foreach ($found_records as $found_record_obj) {
    if (substr($found_record_obj->batch_id, 0, 3) == 'ws_') {
      $count++;
    }
  }
  return $count;
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
      $first_three = substr($found_record_obj->batch_id, 0, 3);
      if (!isset($raw_records_found[$found_record_obj->batch_id]->reasons) || ($first_three == 'ws_'))  {
        if ($first_three == 'ws_') {
          $raw_records_found[$found_record_obj->batch_id]->search_matches =
              (isset($raw_records_found[$found_record_obj->batch_id]->search_matches) ? ($raw_records_found[$found_record_obj->batch_id]->search_matches + 1) : 1);
        }
        $raw_records_found[$found_record_obj->batch_id]->reasons = array($reason);
      }
      else {
        $raw_records_found[$found_record_obj->batch_id]->reasons[] = $reason;
      }
    }
  }
  // Sort the Digitization Requests by the number of search matches
  uasort($raw_records_found, "islandora_digital_workflow_DR_compare");

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

/**
 * Helper function so that the associative array can be searched based on the
 * elements' search_matches value.
 *
 * Since the array that this operates on contains both standard batch record
 * objects as well as Digitization Requests, the code should be careful about
 * trying to access a property that may not exist.
 *
 * @param array $a
 * @param array $b
 * @return int
 *   Determines the sorted item direction.
 */
function islandora_digital_workflow_DR_compare($a, $b) {
  $a_matches = isset($a->search_matches) ? $a->search_matches : 999;
  $b_matches = isset($b->search_matches) ? $b->search_matches : 999;
  if ($a_matches == $b_matches) {
      return 1;
  }
  return ($a_matches > $b_matches) ? -1 : 1;
}