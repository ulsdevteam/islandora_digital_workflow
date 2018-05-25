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