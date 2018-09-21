<?php

/**
 * @file
 * Form and functions related to displaying the Forena reports related to the
 * Islandora Digital Workflow.
 */

/**
 * The Drupal reports form representation.
 * 
 * This is to present the user with a list of possible reports on the batches
 * and items.
 *
 * @param object $node
 *   Drupal node object.
 * @return array
 *   The Drupal form definition.
 */
function islandora_digital_workflow_reports() {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/lookup.form');

  $lookup_form_markup = theme('islandora_digital_workflow_lookup', array('searchterm' => ''));

  $breadcrumb = array(
    l(t('Home'), '<front>'),
    l(t('Islandora Digital Workflow'), 'islandora/islandora_digital_workflow'),
  );
  drupal_set_breadcrumb($breadcrumb);

  $reports_form = drupal_get_form('islandora_digital_workflow_get_reports_form');
  return $lookup_form_markup . drupal_render($reports_form);
}

/**
 * Drupal form definition to display links to all of the "Islandora Digital
 * Workflow" reports.
 *
 * This uses islandora_digital_workflow_strap_report_reports to get an array
 * of links to all of the forena reports.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @return array
 *   The Drupal form definition.
 */
function islandora_digital_workflow_get_reports_form($form, &$form_state) {
  $report_links = islandora_digital_workflow_strap_report_reports();

  $form = array(
      'report_links' => array(
          '#type' => 'item',
          '#markup' => '<ul><li>' . $report_links . '</li></ul>',
      ),
  );
  return $form;
}

/**
 * Helper function to retrieve the Forena reports in the "Islandora Digital 
 * Workflow" category
 *
 * NOTE: This will not return any reports that have their "Hidden" property set
 * to TRUE within their General settings page.
 *
 * @return string
 *   Markup for the links to each of the found forena reports.
 */
function islandora_digital_workflow_strap_report_reports() {
  module_load_include('inc', 'forena', 'forena.common');
  $output = '';
  $category = 'Islandora Digital Workflow';
  $reports = forena_get_user_reports($category);
  $report_repos = variable_get('forena_path', 'reports');
  if (!$reports) {
    $output = 'No Reports Found';
  }
  foreach ($reports as $category => $reports) {
    foreach ($reports as $r) {
      $output .= l($r['title'], $report_repos . '/' . str_replace('/', '.', $r['report_name'])) . 
              ((isset($r['description']) && $r['description']) ? '<br><span class="small_lt_text"> - ' . $r['description'] . '<br></span>': '') . "<br>";
    }
  }

  return $output;
}