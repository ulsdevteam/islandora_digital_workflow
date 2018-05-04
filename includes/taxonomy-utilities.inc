<?php

function islandora_digital_workflow_get_tid_by_name($name, $vocab) {
  $terms = taxonomy_get_term_by_name($name, $vocab->machine_name);
  if (is_array($terms)) {
    $term_object = array_shift($terms);
    return (is_object($term_object) ? $term_object->tid : FALSE);
  }
  else {
    return FALSE;
  }
}

function islandora_digital_workflow_get_name_of_tid($tid) {
  $term = taxonomy_term_load($tid);
  return $term->name;
}
