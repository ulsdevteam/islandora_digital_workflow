<?php

/**
 * @file
 * islandora_digital_workflow default batch view
 */


/**
 * Implements hook_views_default_views().
 */
function islandora_digital_workflow_views_default_views() {
  $view = new view();
  $view->name = 'workflow_nodes';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'node';
  $view->human_name = 'Workflow Nodes';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Workflow Nodes';
  $handler->display->display_options['use_ajax'] = TRUE;
  $handler->display->display_options['use_more'] = TRUE;
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['group_by'] = TRUE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['query']['options']['distinct'] = TRUE;
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '50';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['pager']['options']['id'] = '0';
  $handler->display->display_options['pager']['options']['quantity'] = '9';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'nid' => 'nid',
    'php_1' => 'php_1',
    'title' => 'title',
    'field_batch_name' => 'field_batch_name',
    'field_barcodes' => 'field_barcodes',
    'field_pid_count' => 'field_pid_count',
    'php_2' => 'php_2',
    'field_content_type' => 'field_content_type',
    'field_stage' => 'field_stage',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'nid' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'php_1' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'title' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'field_batch_name' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'field_barcodes' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'field_pid_count' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'php_2' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'field_content_type' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'field_stage' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* Field: Content: Nid */
  $handler->display->display_options['fields']['nid']['id'] = 'nid';
  $handler->display->display_options['fields']['nid']['table'] = 'node';
  $handler->display->display_options['fields']['nid']['field'] = 'nid';
  $handler->display->display_options['fields']['nid']['label'] = '';
  $handler->display->display_options['fields']['nid']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
  /* Field: Global: PHP */
  $handler->display->display_options['fields']['php_1']['id'] = 'php_1';
  $handler->display->display_options['fields']['php_1']['table'] = 'views';
  $handler->display->display_options['fields']['php_1']['field'] = 'php';
  $handler->display->display_options['fields']['php_1']['label'] = 'Select';
  $handler->display->display_options['fields']['php_1']['use_php_setup'] = 0;
  $handler->display->display_options['fields']['php_1']['php_output'] = '<input type="checkbox" id="nid_<?php print $row->nid; ?>">';
  $handler->display->display_options['fields']['php_1']['use_php_click_sortable'] = '0';
  $handler->display->display_options['fields']['php_1']['php_click_sortable'] = '';
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  /* Field: Content: Batch name */
  $handler->display->display_options['fields']['field_batch_name']['id'] = 'field_batch_name';
  $handler->display->display_options['fields']['field_batch_name']['table'] = 'field_data_field_batch_name';
  $handler->display->display_options['fields']['field_batch_name']['field'] = 'field_batch_name';
  $handler->display->display_options['fields']['field_batch_name']['label'] = '';
  $handler->display->display_options['fields']['field_batch_name']['exclude'] = TRUE;
  $handler->display->display_options['fields']['field_batch_name']['element_label_colon'] = FALSE;
  /* Field: Content: Identifiers */
  $handler->display->display_options['fields']['field_identifiers']['id'] = 'field_identifiers';
  $handler->display->display_options['fields']['field_identifiers']['table'] = 'field_data_field_identifiers';
  $handler->display->display_options['fields']['field_identifiers']['field'] = 'field_identifiers';
  $handler->display->display_options['fields']['field_identifiers']['label'] = '';
  $handler->display->display_options['fields']['field_identifiers']['exclude'] = TRUE;
  $handler->display->display_options['fields']['field_identifiers']['element_label_colon'] = FALSE;
  /* Field: Content: Object Count */
  $handler->display->display_options['fields']['field_pid_count']['id'] = 'field_pid_count';
  $handler->display->display_options['fields']['field_pid_count']['table'] = 'field_data_field_pid_count';
  $handler->display->display_options['fields']['field_pid_count']['field'] = 'field_pid_count';
  $handler->display->display_options['fields']['field_pid_count']['label'] = '# of digital objects';
  $handler->display->display_options['fields']['field_pid_count']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_pid_count']['settings'] = array(
    'thousand_separator' => '',
    'prefix_suffix' => 1,
  );
  /* Field: Global: PHP */
  $handler->display->display_options['fields']['php_2']['id'] = 'php_2';
  $handler->display->display_options['fields']['php_2']['table'] = 'views';
  $handler->display->display_options['fields']['php_2']['field'] = 'php';
  $handler->display->display_options['fields']['php_2']['label'] = '# of Identifiers';
  $handler->display->display_options['fields']['php_2']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['php_2']['use_php_setup'] = 0;
  $handler->display->display_options['fields']['php_2']['php_value'] = '';
  $handler->display->display_options['fields']['php_2']['php_output'] = '<?php
    $digital_objects_count = isset($data->field_data_field_pid_count_field_pid_count_value) ? $data->field_data_field_pid_count_field_pid_count_value : 0;
    $identifiers_count = ($data->field_data_field_identifiers_field_identifiers_value <> \'\') ? count(explode("\\n", $data->field_data_field_identifiers_field_identifiers_value)) : 0;
  ?>
  <div class="views-align-right <?php print ($digital_objects_count == $identifiers_count) ? \'good\' : \'bad\'; ?>">
  <?php print $identifiers_count; ?>
  </div>';
  $handler->display->display_options['fields']['php_2']['use_php_click_sortable'] = '0';
  $handler->display->display_options['fields']['php_2']['php_click_sortable'] = '';
  /* Field: Content: Stage */
  $handler->display->display_options['fields']['field_stage']['id'] = 'field_stage';
  $handler->display->display_options['fields']['field_stage']['table'] = 'field_data_field_stage';
  $handler->display->display_options['fields']['field_stage']['field'] = 'field_stage';
  $handler->display->display_options['fields']['field_stage']['alter']['alter_text'] = TRUE;
  $handler->display->display_options['fields']['field_stage']['alter']['text'] = '<div class=\'stage_[field_stage]\'>[field_stage]</div>';
  $handler->display->display_options['fields']['field_stage']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_stage']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['field_stage']['type'] = 'taxonomy_term_reference_plain';
  /* Sort criterion: Content: Post date */
  $handler->display->display_options['sorts']['created']['id'] = 'created';
  $handler->display->display_options['sorts']['created']['table'] = 'node';
  $handler->display->display_options['sorts']['created']['field'] = 'created';
  $handler->display->display_options['sorts']['created']['order'] = 'DESC';
  /* Contextual filter: Content: Stage (field_stage) */
  $handler->display->display_options['arguments']['field_stage_tid']['id'] = 'field_stage_tid';
  $handler->display->display_options['arguments']['field_stage_tid']['table'] = 'field_data_field_stage';
  $handler->display->display_options['arguments']['field_stage_tid']['field'] = 'field_stage_tid';
  $handler->display->display_options['arguments']['field_stage_tid']['exception']['title_enable'] = TRUE;
  $handler->display->display_options['arguments']['field_stage_tid']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['field_stage_tid']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['field_stage_tid']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['field_stage_tid']['summary_options']['items_per_page'] = '25';
  /* Filter criterion: Content: Published */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'node';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['value'] = 1;
  $handler->display->display_options['filters']['status']['group'] = 1;
  $handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
  /* Filter criterion: Content: Type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'node';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['value'] = array(
    'workflow_batch' => 'workflow_batch',
  );
  $handler->display->display_options['filters']['type']['group'] = 1;
  /* Filter criterion: Content: Batch name (field_batch_name) */
  $handler->display->display_options['filters']['field_batch_name_value']['id'] = 'field_batch_name_value';
  $handler->display->display_options['filters']['field_batch_name_value']['table'] = 'field_data_field_batch_name';
  $handler->display->display_options['filters']['field_batch_name_value']['field'] = 'field_batch_name_value';
  $handler->display->display_options['filters']['field_batch_name_value']['operator'] = 'contains';
  $handler->display->display_options['filters']['field_batch_name_value']['group'] = 1;
  $handler->display->display_options['filters']['field_batch_name_value']['exposed'] = TRUE;
  $handler->display->display_options['filters']['field_batch_name_value']['expose']['operator_id'] = 'field_batch_name_value_op';
  $handler->display->display_options['filters']['field_batch_name_value']['expose']['label'] = 'Batch name';
  $handler->display->display_options['filters']['field_batch_name_value']['expose']['operator'] = 'field_batch_name_value_op';
  $handler->display->display_options['filters']['field_batch_name_value']['expose']['identifier'] = 'field_batch_name_value';
  $handler->display->display_options['filters']['field_batch_name_value']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    3 => 0,
    4 => 0,
    5 => 0,
    6 => 0,
  );

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['path'] = 'islandora/islandora_digital_workflow/batches/%';

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->display->display_options['path'] = 'islandora/islandora_digital_workflow/batches/%';

  $views[$view->name] = $view;
  return $views;
}
