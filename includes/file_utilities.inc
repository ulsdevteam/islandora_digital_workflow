<?php

/**
 * Helper function to return a set of filenames from a given path -- with the option to filter ($mask)
 * and an option to recursively ($recurse) search for files under the initial directory path ($dir).
 */
function islandora_digital_workflow_file_scan_directory($dir, $mask = '', $recurse = FALSE, $returnOnlyFolders = FALSE) {
  $result = array();
  if (!is_dir($dir)) {
    return $result;
  }
  $cdir = scandir($dir);
  foreach ($cdir as $key => $value) {
    if (!in_array($value,array(".",".."))) {
      if ($recurse && (is_dir($dir . DIRECTORY_SEPARATOR . $value))) {
        $result[$value] = islandora_digital_workflow_file_scan_directory($dir . DIRECTORY_SEPARATOR . $value, $mask, $recurse, $returnOnlyFolders);
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


function islandora_digital_workflow_add_filesize_date_to_files($file_root, $files) {
  $filesize_date_files = array();
  foreach ($files as $index => $file) {
    $full_file_path = $file_root . '/' . $file;
    $size = filesize($full_file_path);
    $size_as_string = ($size === FALSE) ? 'n/a' : number_format($size);
    $date = filemtime($full_file_path);
    $date_as_string = ($date === FALSE) ? 'n/a' : date("F d Y H:i:s.", $date);
    $filesize_date_files[$index] = $file . "\t" . $size_as_string . "\t" . $date_as_string;
  }
  return $filesize_date_files;
}