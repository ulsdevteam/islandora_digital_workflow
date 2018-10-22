<?php

/**
 * @file
 * Set of taxonomy related functions related to the Islandora Digital Workflow.
 */

/**
 * Gets the taxonomy vocabulary tid from the name.
 *
 * This may not work if the same name is used more than once within the $vocab.
 *
 * @param string $name
 *   The name for which to look up the $tid
 * @param object $vocab
 *   Drupal vocabulary object. *
 * @return mixed
 *   Either the vocabulary $tid value OR FALSE if it cannot be found.
 */
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

/**
 * Get the term name given a vocabulary term $tid.
 *
 * @param integer $tid
 *   Value of the vocabulary term $tid.
 * @return mixed
 *   Either the $name of the taxonomy term or FALSE if it cannot be found.
 */
function islandora_digital_workflow_get_name_of_tid($tid) {
  $term = taxonomy_term_load($tid);
  return (is_object($term)) ? $term->name : FALSE;
}

/**
 * Get the set of vocabulary terms in a given $vocabulary (drupal) object.
 *
 * @param object $vocab
 *   Drupal taxonomy vocabulary object.
 * @return array
 *   An array of vocabulary terms objects.
 */
function islandora_digital_workflow_get_all_terms_of_vocab($vocab) {
  $terms = entity_load('taxonomy_term', FALSE, array('vid' => $vocab->vid));
  return $terms;
}
