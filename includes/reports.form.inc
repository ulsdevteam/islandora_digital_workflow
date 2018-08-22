<?php

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

// the /stats report from bigfoot : display these totals / values for each batch:
/// batch	description	item_count	file_count	file_size	date


function islandora_digital_workflow_get_reports_form($form, &$form_state) {
  $report_links = islandora_digital_workflow_strap_report_reports();

  $form = array(
      'report_links' => array(
          '#type' => 'item',
          '#markup' => '<ul><li>' . implode("</li><li>", $report_links) . '</li></ul>',
      ),
      'input1' => array(
          '#type' => 'textfield',
          '#title' => 'Input 1',
          '#description' => 'foobar',
          '#default_value' => 'default value',
      ),
      'delete' => array(
          '#type' => 'submit',
          '#value' => t('Load Report'),
      ),
  );
  return $form;
}

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
    $output .= '<ul>';
    foreach ($reports as $r) {
      $output .= '<li>' . l($r['title'], $report_repos . '/' . str_replace('/', '.', $r['report_name'])) . '</li>';
    }
    $output .= '</ul>';
  }
  // return $output;

  dpm($reports);
  dpm($report_repos);

  $links = array();
  $links[] = l('Batch Stats' ,'/reports/batch_stats') . '<br>' .
          '<span class="small_lt_text">- display the "File Count" and "File Size" value for all batches.<br></span> ' .
          "<hr><hr>" . $output;

  return $links;
}