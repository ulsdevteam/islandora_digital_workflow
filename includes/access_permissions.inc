<?php

/**
 * Helper function that returns whether or not the user has permission.
 *
 * @global object $user
 *   A standard drupal user object.
 * @param string $permission_constant
 *   Constant that represents any permission
 * @return boolean
 *   Whether or not the user has access to $permission_constant.
 */
function islandora_digital_workflow_user_has_access($permission_constant) {
  global $user;
  return ($permission_constant) ? user_access($permission_constant, $user) : FALSE;
}

function islandora_digital_workflow_user_roles() {
  global $user;
  // Based on the user's permissions that are configured under the Site's
  // permissions, we can derive the set of roles that they have.
  $user_is_roles = array();
  $user_is_curator = user_access(ISLANDORA_DIGITAL_WORKFLOW_IS_CURATOR, $user);
  $user_is_mad = user_access(ISLANDORA_DIGITAL_WORKFLOW_IS_MAD, $user);
  $user_is_drl = user_access(ISLANDORA_DIGITAL_WORKFLOW_IS_DRL, $user);
  $user_is_workflow_user = user_access(ISLANDORA_DIGITAL_WORKFLOW, $user);

  if ($user_is_curator) {
    $user_is_roles[] = 'Curator';
  }
  if ($user_is_drl) {
    $user_is_roles[] = 'DRL';
  }
  if ($user_is_mad) {
    $user_is_roles[] = 'MAD';
  }
  if ($user_is_workflow_user) {
    $user_is_roles[] = 'User';
  }
  return array(
    'user_is_curator' => $user_is_curator,
    'user_is_drl' => $user_is_drl,
    'user_is_mad' => $user_is_mad,
    'user_is_workflow_user' => $user_is_workflow_user,
    'role' => implode(", ", $user_is_roles));
}

