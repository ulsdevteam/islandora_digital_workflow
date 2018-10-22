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
    $filesize_date_files[$index] = array('file' => $file, 'size' => $size, 'size_as_string' => $size_as_string, 'date_as_string' => $date_as_string);
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