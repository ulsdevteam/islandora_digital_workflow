<?php

/**
 * @file
 * Set of functions related to file and folder operations.
 */

/**
 * Helper function to return a set of filenames from a given path -- with the option to filter ($mask)
 * and an option to recursively ($recurse) search for files under the initial directory path ($dir).
 */
function islandora_digital_workflow_file_scandir($dir, $mask = '', $recurse = FALSE, $returnOnlyFolders = FALSE) {
  $result = array();
  if (!is_dir($dir)) {
    return $result;
  }
  $cdir = scandir($dir);
  foreach ($cdir as $key => $value) {
    if (!in_array($value,array(".",".."))) {
      if ($recurse && (is_dir($dir . DIRECTORY_SEPARATOR . $value))) {
        $result[$value] = islandora_digital_workflow_file_scandir($dir . DIRECTORY_SEPARATOR . $value, $mask, $recurse, $returnOnlyFolders);
      }
      elseif (!$recurse || !$returnOnlyFolders) {
        if (!$mask || (strstr($value, $mask))) {
          $result[] = $value;
        }
      }
    }
  }
  return $result;
}

/**
 * This returns a string for the files that provides the file size and date.
 *
 * @param string $file_root
 *   The location for all of the files.
 * @param array $files
 *   Array of files referenced under the $file_root location
 * @return string
 *   The filename, size, and date as strings separated by TAB characters.
 */
function islandora_digital_workflow_add_filesize_date_to_files($file_root, $files) {
  $filesize_date_files = array();
  foreach ($files as $index => $file) {
    $full_file_path = $file_root . '/' . $file;
    $size = file_exists($full_file_path) ? filesize($full_file_path) : FALSE;
    $size_as_string = ($size === FALSE) ? 'n/a' : number_format($size);
    $date = file_exists($full_file_path) ? filemtime($full_file_path) : FALSE;
    $date_as_string = ($date === FALSE) ? 'n/a' : date("F d Y H:i:s", $date);
    // $filesize_date_files[$index] = $file . "\t" . $size_as_string . "\t" . $date_as_string;
    $filesize_date_files[$index] = array(
        'file' => $file,
        'size' => $size,
        'size_as_string' => $size_as_string,
        'date_as_string' => $date_as_string
    );
  }
  return $filesize_date_files;
}

/**
 * Helper function to clean up directories and indiscriminately purge any files
 * that were in the folder (function recursively calls itself).
 *
 * @param string $dir
 *   Directory (or file) to delete.
 * @return boolean
 *   whether or not the referenced $dir was deleted.
 */
function islandora_digital_workflow_deleteDirectory($dir) {
  if (!file_exists($dir)) {
    return true;
  }
  if (!is_dir($dir)) {
    return @unlink($dir);
  }
  foreach (scandir($dir) as $item) {
    if ($item == '.' || $item == '..') {
      continue;
    }
    if (!islandora_digital_workflow_deleteDirectory($dir . DIRECTORY_SEPARATOR . $item)) {
      return false;
    }
  }
  return rmdir($dir);
}

/**
 * This copies the delivery files for paged content and resequences the files
 * according to their alphabetical sorting of source filenames.
 *
 * Depending on the configuration, this may delete the file after it is copied.
 *
 * @param array $item_files
 *   Array of filenames that will be copied / moved.
 * @param string $item_folder
 *   The configured "Incoming Delivery Directory" for the item.
 * @param string $item_working_folder
 *   The configured "Ingest Working Directory" for the item.
 * @return boolean
 */
function islandora_digital_workflow_copy_resequence_files($item_files, $item_folder, $item_working_folder, $batch_name = '') {
  $sorted_index = array();
  $copied = FALSE;
  $i = 1;
  foreach ($item_files as $filename => $item_file_arr) {
    $item_full_filename = $item_folder . '/' . $filename;
    $file_pathinfo = pathinfo($item_full_filename);
    $sequence_value = str_pad($i, 4, "0", STR_PAD_LEFT);
    $sorted_index[$sequence_value] = array(
        'from' => $item_full_filename,
        'to' => $filename
    );
    $i++;
  }
  if (!file_exists($item_working_folder)) {
    mkdir($item_working_folder);
  }
  ksort($sorted_index);
  foreach ($sorted_index as $sequence_value => $item_file_arr) {
    $file_filename = $sorted_index[$sequence_value]['to'];
    $file_pathinfo = pathinfo($file_filename);
    $extension = (array_key_exists('extension', $file_pathinfo)) ? $file_pathinfo['extension'] : '';
    $sorted_index[$sequence_value] = array(
        'from' => $sorted_index[$sequence_value]['from'],
        'to' => $item_working_folder . '/' . $sequence_value . '/OBJ.' . $extension,
    );
    $item_sequence_folder = $item_working_folder . '/' . $sequence_value;
    if (!file_exists($item_sequence_folder)) {
      mkdir($item_sequence_folder);
    }
    $copied = copy($sorted_index[$sequence_value]['from'],  $item_sequence_folder . '/OBJ.' . $extension);
    if ($copied && (variable_get('islandora_digital_workflow_delete_delivery_files', 0))) {
      @unlink($sorted_index[$sequence_value]['from']);
    }
  }
  // If all of the files are now moved, remove the folder too.
  if ($copied && (variable_get('islandora_digital_workflow_delete_delivery_files', 0))) {
    $delivery_files_remaining = islandora_digital_workflow_file_scandir($item_folder);
    if (count($delivery_files_remaining) < 1) {
      islandora_digital_workflow_deleteDirectory($item_folder);
    }
    $item_folder_pathinfo = pathinfo($item_folder);
    if (array_key_exists('dirname', $item_folder_pathinfo) && strstr($item_folder_pathinfo['dirname'], $batch_name) ) {
      dpm('XXXXX resume work here');
    }
  }
  return TRUE;
}

/**
 * Simple handler that will spit out a download of either the minimal CSV template
 * or a more extended version that could be used to create item MODS.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @param string $verb
 *   Either "extended" or "minimal"
 */
function islandora_digital_workflow_csv_template_download(array $form, array &$form_state, $verb) {
  if(isset($verb)){
    header("Content-type: text/csv");
    header("Content-Disposition: attachment; filename=CSV_template_" . 
        (($verb == 'minimal') ? 'minimal' : 'extended')  . ".csv");
    if ($verb == 'minimal') {
      print 'title,identifier
';
    }
    else {
      print 'title,creator,subject,description,normalized_date,normalized_date_qualifier,identifier,copyright_status,publication_status,rights_holder,type_of_resource,genre,format,filename,source_collection_id,source_collection,address,batch,subject_location,source_id,depositor
';
    }
    die;
  }
} 
