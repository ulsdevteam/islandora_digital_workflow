<?php

/**
 * Display page for admin/islandora/islandora_digital_workflow/workflow_sequences
 * menu route.
 *
 * @return string
 *   Markup for the page to provide links to edit the sequences, or model
 * associations.
 */
function islandora_digital_workflow_sequences() {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');

  drupal_set_title(t('Islandora Digital Workflow - Workflow Sequences'), CHECK_PLAIN);
  $breadcrumb = array(
    l('Home', '<front>'),
    l('Administration', 'admin'),
    l('Islandora', 'admin/islandora'),
    l('Islandora Digital Workflow', 'admin/islandora/islandora_digital_workflow')
  );
  drupal_set_breadcrumb($breadcrumb);

  $workflow_sequences = islandora_digital_workflow_get_workflow_sequences();

  $workflow_sequence_actions = islandora_digital_workflow_get_workflow_sequence_actions();
  $workflow_model_sequences = islandora_digital_workflow_get_workflow_model_sequences();

  $form['link'] = array(
      '#type' => 'item',
      '#markup' => l('Manage Reserved Actions', 'admin/islandora/islandora_digital_workflow/workflow_sequence_reserved_actions'),
  );
  $form['wrapper'] = array(
      '#type' => 'fieldset',
      '#title' => t('Workflow Sequences'),
  );
  $form['wrapper']['add_new'] = array(
      '#type' => 'item',
      '#markup' => '<ul class="action-links"><li>' . 
        l('Create new sequence', 'admin/islandora/islandora_digital_workflow/create_workflow_sequences') .
      '</li></ul>',
  );
  $models_arr = array();
  foreach ($workflow_model_sequences as $workflow_sequence_id => $models) {
    foreach ($models as $model) {
      if ($model) {
        $models_arr[$workflow_sequence_id][] = $model;
      }
    }
  }

  foreach ($workflow_sequences as $workflow_sequence_id => $name_description) {
    $models = array_key_exists($workflow_sequence_id, $models_arr) ? implode(", ", $models_arr[$workflow_sequence_id]) : '';
    $this_workflow_sequence_actions = array_key_exists($workflow_sequence_id, $workflow_sequence_actions) ? $workflow_sequence_actions[$workflow_sequence_id] : array();
    $markup_item_actions = $markup_batch_actions = array();
    foreach ($this_workflow_sequence_actions as $action_record_array) {
      $class = ($action_record_array['is_required']) ? 'required_action' : 'optional_action';
      $glyph_class = 'transaction_action_' . (($action_record_array['action_name']) ? strtolower(str_replace(array("-", " "), "_", $action_record_array['action_name'])) : 'spacer');
      $li_markup = '<li class="' . $glyph_class . ' ' . $class . '">' . $action_record_array['action_name'] . '</li>';
      if ($action_record_array['action_id'] > 99) {
        $markup_batch_actions[] = $li_markup;
      } else {
        $markup_item_actions[] = $li_markup;
      }
    }

    $form['wrapper']['workflow_sequence_id_' . $workflow_sequence_id] = array(
      '#type' => 'item',
      '#markup' =>
        '<a name="' . $name_description['name'] . '">' .
        '<div class="dashboard-report">' .
        '  <div class="small_right_float">' .
          l('Edit', 'admin/islandora/islandora_digital_workflow/edit_workflow_sequence/' . $workflow_sequence_id) . ' | ' .
          l('Delete', 'admin/islandora/islandora_digital_workflow/delete_workflow_sequence/' . $workflow_sequence_id) .
        '  </div>' .
        '<h3>' . l($name_description['name'], 'admin/islandora/islandora_digital_workflow/edit_workflow_sequence/' . $workflow_sequence_id) . '</h3>' .
        '<p>' . $name_description['description'] . '</p>' . 
        '  <div class="lookup_result_square"><p><b>' .
          l('Models', 'admin/islandora/islandora_digital_workflow/workflow_sequence_models/' . $workflow_sequence_id) . ':</b> ' . $models .
          '</p><p><b>' .
          l('Actions', 'admin/islandora/islandora_digital_workflow/workflow_sequences/' . $workflow_sequence_id) .
        ':</b><ul>' .
        ((count($markup_batch_actions) > 0) ?
          '<li class="no_indent">Batch actions</li>' .
          '<ul class="batch_action_box corner_bordered">' . implode("\n" , $markup_batch_actions) . '</ul>' : '') .
        ((count($markup_item_actions) > 0) ?
          '<li class="no_indent">Item actions</li>' .
          '<ul class="corner_bordered item_action_box">' . implode("\n" , $markup_item_actions) . '</ul>' : '') .
        '</ul></p></div></div></a>',
    );
  }

  if (count($workflow_sequences) < 1) {
    $form['wrapper']['none'] = array(
      '#type' => 'item',
      '#markup' => '<em class="bad">There are no workflow sequences yet.</em>  ' .
        'This area would allow each sequence to be managed.',
    );
  }

  return $form;
}

/**
 * Form to handle delete - will prompt user.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @param type $workflow_sequence_id
 *   The identifier for the workflow sequence.
 *
 * @return array
 *   Drupal form definition.
 */
function islandora_digital_workflow_order_delete(array $form, array &$form_state, $workflow_sequence_id) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');

  $workflow_sequence_actions = islandora_digital_workflow_get_workflow_sequence_actions();
  $workflow_model_sequences = islandora_digital_workflow_get_workflow_model_sequences();
  $workflow_sequences = islandora_digital_workflow_get_workflow_sequences();
  $sequence_title = $workflow_sequences[$workflow_sequence_id]['name'];
  $affected_batch_nodes = islandora_digital_workflow_get_batches_by_workflow_sequence_id($workflow_sequence_id);
  $form = array();

  $form['workflow_sequence_id'] = array(
          '#type' => 'hidden',
          '#default_value' => $workflow_sequence_id
      );
  $form['warning'] = array(
          '#type' => 'item',
          '#markup' => '<span class="bad"><b>This can not be undone!</b>  Are ' . 
            'you certain that you want to delete this workflow sequence and any ' . 
            'action / Islandora model relationships?</span>',
      );
  $form['confirm'] = array(
          '#type' => 'submit',
          '#value' => t('Confirm DELETE'),
      );
  $form['cancel'] = array(
          '#type' => 'submit',
          '#value' => t('Cancel'),
      );
  if (count($affected_batch_nodes) > 0) {
    $form['warning']['#prefix'] = '<div class="dashboard-report"><p>If this ' .
        'workflow wequence is deleted, the following batches would need to be ' .
        'associated with another Workflow Sequence:<ul><li>' .
        implode('</li><li>', $affected_batch_nodes) . '</ul></p></div>';
  }

  drupal_set_title(t('Islandora Digital Workflow - Delete Workflow Sequence'), CHECK_PLAIN);
  $breadcrumb = array(
    l('Home', '<front>'),
    l('Administration', 'admin'),
    l('Islandora', 'admin/islandora'),
    l('Islandora Digital Workflow', 'admin/islandora/islandora_digital_workflow'),
    l('Islandora Digital Workflow - Workflow Sequences', 'admin/islandora/islandora_digital_workflow/workflow_sequences'),
    $sequence_title,
  );

  return $form;
}

/**
 * This will delete the workflow sequence and the related model and batch
 * record associations.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_digital_workflow_order_delete_submit(array $form, array &$form_state) {
  // If cancel is clicked, redirect to the workflow_sequences page
  if ($form_state['triggering_element']['#value'] == 'Cancel') {
    drupal_set_message('Delete was cancelled for workflow sequence.');
  }
  else {
    $workflow_sequence_id = $form_state['values']['workflow_sequence_id'];
    // Delete any islandora_digital_workflow_model_sequence relationship records.
    $deleted_models_count = db_query('DELETE FROM {islandora_digital_workflow_model_sequence} WHERE ' .
        'workflow_sequence_id = :workflow_sequence_id',
            array(':workflow_sequence_id' => $workflow_sequence_id));

    // Delete any islandora_digital_workflow_sequence_actions records.
    $deleted_actions_count = db_query('DELETE FROM {islandora_digital_workflow_sequence_actions} WHERE ' .
        'workflow_sequence_id = :workflow_sequence_id',
            array(':workflow_sequence_id' => $workflow_sequence_id));

    // Update any batch records that were using this sequence to display links
    // to select a new workflow sequence for these.
    //
    // First, we must select the batch identifiers that are impacted for the
    // message that will be displayed to update these batches.
    $affected_batch_nodes = islandora_digital_workflow_get_batches_by_workflow_sequence_id($workflow_sequence_id);
    $query = db_select('islandora_digital_workflow_batch', 'b')
        ->fields('b', array('nid', 'batch_name'))
        ->condition('b.workflow_sequence_id', $workflow_sequence_id);
    $results = $query->execute();
    $rows = $results->fetchAll();
    $affected_batch_nodes = array();
    $link_attributes = array('attributes' => array('target' => '_blank'));
    foreach ($rows as $i => $row) {
      // Make a list of links to edit these batchs' batch records.
      $affected_batch_nodes[] = l($row->batch_name, 'node/' . $row->nid . '/batch', $link_attributes);
    }
    $batches_updated_count = db_update('islandora_digital_workflow_batch')
        ->fields(array('workflow_sequence_id' => NULL))
        ->condition('workflow_sequence_id', $workflow_sequence_id)
        ->execute();

    // Finally, delete the actual record for this workflow sequence.
    db_query('DELETE FROM {islandora_digital_workflow_sequence} WHERE ' .
        'workflow_sequence_id = :workflow_sequence_id',
            array(':workflow_sequence_id' => $workflow_sequence_id));

    if ($batches_updated_count) {
      drupal_set_message($batches_updated_count . ' batch records were updated ' .
          'in order to remove this workflow sequence.', 'warning');
      drupal_set_message(explode('
', $affected_batch_nodes), 'warning');
    }

    if ($deleted_models_count) {
      drupal_set_message('Related model associations for the deleted sequence were also removed,');
    }
    if ($deleted_actions_count) {
      drupal_set_message('Related actions for the deleted sequence were also removed,');
    }

    drupal_set_message('Deleted workflow sequence.');
  }
  drupal_goto('admin/islandora/islandora_digital_workflow/workflow_sequences');
}

/**
 * Returns the batches that are using a given workflow_sequence_id.
 *
 * @param integer $workflow_sequence_id
 *   A workflow sequence identifier.
 * @return array
 *   Links to the batches that are related to the given workflow_sequence_id.
 */
function islandora_digital_workflow_get_batches_by_workflow_sequence_id($workflow_sequence_id) {
  $query = db_select('islandora_digital_workflow_batch', 'b')
      ->fields('b', array('nid', 'batch_name', 'batch_description'))
      ->condition('b.workflow_sequence_id', $workflow_sequence_id);
  $results = $query->execute();
  $rows = $results->fetchAll();
  $affected_batch_nodes = array();
  $link_attributes = array('attributes' => array('target' => '_blank'));
  foreach ($rows as $i => $row) {
    // Make a list of links to edit these batchs' batch records.
    $affected_batch_nodes[] = '<b>' . l($row->batch_name, 'node/' . $row->nid . '/batch', $link_attributes) . '</b> ' .
        ($row->batch_description ? $row->batch_description : '');
  }
  return $affected_batch_nodes;
}

/**
 * Form handler for creating a new workflow sequence.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @return array
 *   Drupal form definition.
 */
function islandora_digital_workflow_workflow_sequence_create(array $form, array &$form_state) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
  $form['add_new'] = array(
      '#type' => 'fieldset',
      '#title' => t('Create new sequence'),
      'sequence_name' => array(
          '#title' => t('New sequence name'),
          '#description' => t('Enter a name for the sequence.'),
          '#type' => 'textfield',
          '#default_value' => '',
      ),
      'sequence_description' => array(
          '#title' => t('Sequence description'),
          '#description' => t('Enter a description for the sequence.'),
          '#type' => 'textarea',
          '#default_value' => '',
      ),
      'submit' => array(
          '#type' => 'submit',
          '#value' => t('Save changes')
      ),
  );
  $title = 'Islandora Digital Workflow - Create Workflow Sequence';
  drupal_set_title($title, CHECK_PLAIN);
  $breadcrumb = array(
    l('Home', '<front>'),
    l('Administration', 'admin'),
    l('Islandora', 'admin/islandora'),
    l('Islandora Digital Workflow', 'admin/islandora/islandora_digital_workflow'),
    l('Islandora Digital Workflow - Workflow Sequences', 'admin/islandora/islandora_digital_workflow/workflow_sequences'),
    $title,
  );
  return $form;
}

/**
 * Submit form handler for creating new workflow sequences.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_digital_workflow_workflow_sequence_create_submit($form, &$form_state) {
  $inserted = db_query('INSERT INTO {islandora_digital_workflow_sequence} ' .
      '(`name`, `description`) VALUES (:name, ' .
      ':description)', array(
          ':name' => $form_state['values']['sequence_name'],
          ':description' => $form_state['values']['sequence_description'],
  ));
  $results = db_query('SELECT workflow_sequence_id FROM ' .
      '{islandora_digital_workflow_sequence} WHERE `name` = :name AND ' .
      '`description` = :description', array(
          ':name' => $form_state['values']['sequence_name'],
          ':description' => $form_state['values']['sequence_description'],
  ))->fetchAll();

  $result_obj = array_pop($results);
  $edit_models_link = l('Models', 'admin/islandora/islandora_digital_workflow/workflow_sequence_models/' . $result_obj->workflow_sequence_id);
  $edit_actions_link = l('Actions', 'admin/islandora/islandora_digital_workflow/workflow_sequences/' . $result_obj->workflow_sequence_id);
  drupal_set_message('The new workflow sequence has been saved.  Manage ' . $edit_models_link . ' | ' . $edit_actions_link);
  drupal_goto('admin/islandora/islandora_digital_workflow/workflow_sequences');
}

/**
 * Form handler for editing an existing workflow sequence.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @param type $workflow_sequence_id
 *   The identifier for the workflow sequence.
 *
 * @return array
 *   Drupal form definition.
 */
function islandora_digital_workflow_workflow_sequence_edit(array $form, array &$form_state, $workflow_sequence_id) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
  $workflow_sequences = islandora_digital_workflow_get_workflow_sequences();
  $sequence_title = $workflow_sequences[$workflow_sequence_id]['name'];
  $form['add_new'] = array(
      '#type' => 'fieldset',
      '#title' => t('Edit Sequence'),
      'workflow_sequence_id' => array(
          '#type' => 'hidden',
          '#default_value' => $workflow_sequence_id,
      ),
      'sequence_name' => array(
          '#type' => 'textfield',
          '#title' => t('Sequence name'),
          '#description' => t('Enter a name for the sequence.'),
          '#required' => TRUE,
          '#default_value' => $workflow_sequences[$workflow_sequence_id]['name'],
      ),
      'sequence_description' => array(
          '#type' => 'textarea',
          '#title' => t('Sequence description'),
          '#description' => t('Enter a description for the sequence.'),
          '#default_value' => $workflow_sequences[$workflow_sequence_id]['description'],
      ),
      'submit' => array(
          '#type' => 'submit',
          '#value' => t('Save changes')
      ),
  );

  $related_batch_nodes = islandora_digital_workflow_get_batches_by_workflow_sequence_id($workflow_sequence_id);

  if (count($related_batch_nodes) > 0) {
    $form['wrapper']['related_batches'] = array(
      '#type' => 'item',
      '#markup' => '<div class="corner_bordered"><h3>Batches using "' . $sequence_title .
          '":</h3><ul><li>' . implode("</li><li>", $related_batch_nodes) . '</li></ul></div>',
    );
  }

  drupal_set_title(t('Islandora Digital Workflow - Edit Workflow Sequence'), CHECK_PLAIN);
  $breadcrumb = array(
    l('Home', '<front>'),
    l('Administration', 'admin'),
    l('Islandora', 'admin/islandora'),
    l('Islandora Digital Workflow', 'admin/islandora/islandora_digital_workflow'),
    l('Islandora Digital Workflow - Workflow Sequences', 'admin/islandora/islandora_digital_workflow/workflow_sequences'),
    $sequence_title,
  );

  return $form;
}

/**
 * Submit form handler for editing existing workflow sequence details.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_digital_workflow_workflow_sequence_edit_submit($form, &$form_state) {
  $workflow_sequence_id = (array_key_exists('workflow_sequence_id', $form_state['values']) ?
    $form_state['values']['workflow_sequence_id'] : NULL);
  if ($workflow_sequence_id) {
    $updated = db_query('REPLACE INTO {islandora_digital_workflow_sequence} ' .
        '(`name`, `description`, `workflow_sequence_id`) VALUES (:name, ' .
        ':description, :workflow_sequence_id)', array(
            ':name' => $form_state['values']['sequence_name'],
            ':description' => $form_state['values']['sequence_description'],
            ':workflow_sequence_id' => $workflow_sequence_id,
    ));
    $edit_models_link = l('Models', 'admin/islandora/islandora_digital_workflow/workflow_sequence_models/' . $workflow_sequence_id);
    $edit_actions_link = l('Actions', 'admin/islandora/islandora_digital_workflow/workflow_sequences/' . $workflow_sequence_id);
    drupal_set_message('The workflow sequence has been saved.  Manage ' . $edit_models_link . ' | ' . $edit_actions_link);
    drupal_goto('admin/islandora/islandora_digital_workflow/workflow_sequences');
  }
}

/**
 * Displays configuration options related to a workflow sequence management.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * $workflow_sequence_id integer
 *   Identifier for an islandora_digital_workflow_sequence record.
 *
 * @return string|array
 */
function islandora_digital_workflow_workflow_sequence_actions_form(array $form, array &$form_state, $workflow_sequence_id) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');

  $workflow_sequence_actions = islandora_digital_workflow_get_workflow_sequence_actions();
  $workflow_model_sequences = islandora_digital_workflow_get_workflow_model_sequences();
  $actions = islandora_digital_workflow_get_item_actions(TRUE);
  $batch_actions = islandora_digital_workflow_get_batch_action_options(TRUE);
  $workflow_sequences = islandora_digital_workflow_get_workflow_sequences();
  $sequence_title = $workflow_sequences[$workflow_sequence_id]['name'];
  $insert_batch_action_options = $insert_action_options = array('' => 'Select Action');

  foreach ($actions as $action_id => $arr) {
    $insert_action_options[$action_id] = $arr['description'];
  }
  foreach ($batch_actions as $action_id => $arr) {
    $insert_batch_action_options[$action_id] = $arr['description'];
  }
  if (array_key_exists($workflow_sequence_id, $workflow_sequence_actions)) {
    foreach ($workflow_sequence_actions[$workflow_sequence_id] as $index => $this_workflow_action_array) {
      if (!(array_key_exists($this_workflow_action_array['action_id'], $actions) === FALSE)) {
        unset($insert_action_options[$this_workflow_action_array['action_id']]);
      }
      if (!(array_key_exists($this_workflow_action_array['action_id'], $batch_actions) === FALSE)) {
        unset($insert_batch_action_options[$this_workflow_action_array['action_id']]);
      }
    }
  }
  $form = array(
    '#tree' => TRUE,
    'wrapper' => array(
      '#type' => 'fieldset',
      '#title' => t('Available Item Actions'),
      '#attributes' => array('class' => array('draggableviews-parent')),
      'add_new_item_action' => array(
          '#title' => t('Add another action'),
          '#type' => 'select',
          '#options' => $insert_action_options,
          '#default_value' => '',
          '#attributes' => array('class' => array('item_action')),
      ),
    ),
    'batch_wrapper' => array(
      '#type' => 'fieldset',
      '#title' => t('Available Batch Actions'),
      'add_new_batch_action' => array(
          '#title' => t('Add another action'),
          '#type' => 'select',
          '#options' => $insert_batch_action_options,
          '#default_value' => '',
          '#attributes' => array('class' => array('batch_action')),
      ),
      'models' => array(
          '#type' => 'item',
          '#title' => t('Models'),
          '#description' => l('Edit models association', 'admin/islandora/islandora_digital_workflow/workflow_sequence_models/' . $workflow_sequence_id) .
            str_replace(": ,", ": (default), ", '.  Current Models using this sequence: ' . implode(", ", $workflow_model_sequences[$workflow_sequence_id])) . '. ',
      ),
  ));

  if (array_key_exists($workflow_sequence_id, $workflow_sequence_actions)) {
    foreach ($workflow_sequence_actions[$workflow_sequence_id] as $i => $sequence_array) {
      $form['wrapper']['rows'][$sequence_array['action_id']] = array(
        '#attributes' => array(
          'class' => array(($sequence_array['action_id'] > 99 ? 'batch_action_box' : 'item_action_box')),
        ),
        'action_description' => array(
          '#type' => 'textfield',
          '#default_value' => $sequence_array['action_description'],
          '#attributes' => array('readonly' => 'readonly', 'disabled' => 'disabled'),
        ),
        'action_name' => array(
          '#type' => 'hidden',
          '#default_value' => $sequence_array['action_name'],
        ),
        'is_required' => array(
          '#type' => 'checkbox',
          '#default_value' => $sequence_array['is_required'],
        ),
        'weight' => array(
          '#type' => 'weight',
          '#title_display' => 'invisible',
          '#default_value' => $sequence_array['order'],
        ),
        'workflow_sequence_id' => array(
          '#type' => 'hidden',
          '#default_value' => $workflow_sequence_id,
        ),
        'action_id' => array(
          '#type' => 'hidden',
          '#default_value' => $sequence_array['action_id'],
        ),
        'op' => array(
          '#type' => 'submit',
          '#value' => 'Remove',
          '#name' => 'action_id-' . $sequence_array['action_id'],
        ),
      );
    }
  }
  else {
    $form['wrapper']['rows'] = array();
  }

  $title = ($sequence_title ? ' "' . $sequence_title . '"' : 'Islandora Digital Workflow');
  drupal_set_title($title, CHECK_PLAIN);
  $breadcrumb = array(
    l('Home', '<front>'),
    l('Administration', 'admin'),
    l('Islandora', 'admin/islandora'),
    l('Islandora Digital Workflow', 'admin/islandora/islandora_digital_workflow'),
    l('Islandora Digital Workflow - Workflow Sequences', 'admin/islandora/islandora_digital_workflow/workflow_sequences'),
    $title,
  );
  drupal_set_breadcrumb($breadcrumb);

  $form['submit'] = array('#type' => 'submit', '#value' => t('Save changes'));

  $related_batch_nodes = islandora_digital_workflow_get_batches_by_workflow_sequence_id($workflow_sequence_id);

  if (count($related_batch_nodes) > 0) {
    $form['related_batches'] = array(
      '#type' => 'item',
      '#markup' => '<div class="corner_bordered"><h3>Batches using "' . $sequence_title .
          '":</h3><ul><li>' . implode("</li><li>", $related_batch_nodes) . '</li></ul></div>',
    );
  }
  return $form;
}

/**
 * Submit handler for order_manage form.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_digital_workflow_workflow_sequence_actions_form_submit($form, &$form_state) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
  $item_actions = islandora_digital_workflow_get_item_actions(FALSE);
  $batch_actions = islandora_digital_workflow_get_batch_action_options(FALSE);
  $all_actions = array();
  foreach ($item_actions as $action_id => $item_action) {
    $all_actions[$action_id] = $item_action;
  }
  foreach ($batch_actions as $action_id => $batch_action) {
    $all_actions[$action_id] = $batch_action;
  }
  $current_action_ids = $slides = array();
  $workflow_sequence_id = 0;
  // Removed action_id rows are submitted individually via their [Remove] button.
  if ($form_state['triggering_element']['#value'] == 'Remove') {
    // The name will be something like "action_id-4"
    $button_name = $form_state['triggering_element']['#name'];
    $id_parts = explode("-", $button_name);
    $remove_action_id = isset($id_parts[1]) ? $id_parts[1] : 0;
  }
  else {
    $remove_action_id = 0;
  }
  if (array_key_exists('rows', $form_state['values']['wrapper'])) {
    foreach ($form_state['values']['wrapper']['rows'] as $id => $item) {
      if ($id <> $remove_action_id) {
        $current_action_ids[$item['action_id']] = $item['action_id'];
        $workflow_sequence_id |= $item['workflow_sequence_id'];
      }
    }
    foreach ($form_state['values']['wrapper']['rows'] as $key => $sequence_arr) {
      $workflow_sequence_id = $sequence_arr['workflow_sequence_id'];
    }
  }
  if ($form_state['values']['wrapper']['add_new_item_action']) {
    if ((array_key_exists('rows', $form_state['values']['wrapper']))) {
      $index = count($form_state['values']['wrapper']['rows']) + 1;
    }
    else {
      $index = 1;
      $workflow_sequence_id = arg(4);
      $form_state['values']['wrapper']['rows'] = array();
    }
    $insert_row = array(
        'action_id' => $form_state['values']['wrapper']['add_new_item_action'],
        'action_name' => $all_actions[$form_state['values']['wrapper']['add_new_item_action']]['name'],
        'workflow_sequence_id' => $workflow_sequence_id,
        'is_required' => 0,
        'weight' => 10,
    );
    $form_state['values']['wrapper']['rows'][$index] = $insert_row;
  }
  if ($form_state['values']['batch_wrapper']['add_new_batch_action']) {
    $index = count($form_state['values']['wrapper']['rows']) + 1;
    $insert_row = array(
        'action_id' => $form_state['values']['batch_wrapper']['add_new_batch_action'],
        'action_name' => $all_actions[$form_state['values']['batch_wrapper']['add_new_batch_action']]['name'],
        'workflow_sequence_id' => $workflow_sequence_id,
        'is_required' => 0,
        'weight' => 10,
    );
    $form_state['values']['wrapper']['rows'][$index] = $insert_row;
  }

  if ($workflow_sequence_id && $remove_action_id) {
    $sql = 'DELETE FROM {islandora_digital_workflow_sequence_actions} WHERE workflow_sequence_id = :workflow_sequence_id' .
        ((count($current_action_ids) > 0) ? ' AND action_id NOT IN (' . implode(', ', $current_action_ids) . ')' : '');
    $deleted = db_query($sql,
      array(
          ':workflow_sequence_id' => $item['workflow_sequence_id'],
      ));
  }
  // Because the form elements were keyed with the item ids from the database,
  // we can simply iterate through the submitted values.
  $remove_button_parts = explode("-", $remove_action_id);
  $remove_item_action_id = 0;
  foreach ($remove_button_parts as $id_part) {
    if (!$remove_item_action_id && is_numeric($id_part)) {
      $remove_item_action_id = $id_part;
    }
  }
  $replaced = FALSE;
  if (array_key_exists('rows', $form_state['values']['wrapper'])) {
    foreach ($form_state['values']['wrapper']['rows'] as $id => $item) {
      if ($item['action_id'] <> $remove_item_action_id) {
        $replaced = db_query('REPLACE INTO {islandora_digital_workflow_sequence_actions} ' .
                '(workflow_sequence_id, action_id, is_required, `order`) VALUES (' .
                ':workflow_sequence_id, :action_id, :is_required, :weight)',
          array(
              ':weight' => $item['weight'],
              ':is_required' => $item['is_required'],
              ':workflow_sequence_id' => $workflow_sequence_id,
              ':action_id' => $item['action_id'],
          ));
      }
    }
  }

  if ($replaced) {
    drupal_set_message(t('Actions have been saved.'));
  }
}

/**
 * Custom array sort function by weight.
 *
 * @param integer $a
 *   The first value to compare.
 * @param integer $b
 *   The second value to compare.
 * @return int
 *   TRUE if the two elements should be switched.
 */
function _islandora_digital_workflow_arraysort($a, $b) {
  if (isset($a['order']) && isset($b['order'])) {
    return $a['order'] < $b['order'] ? -1 : 1;
  }
  return 0;
}

/**
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @return array
 *   Drupal form definition.
 *
 * @return string|array
 */
function islandora_digital_workflow_workflow_sequence_models_form(array $form, array &$form_state, $workflow_sequence_id) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');

  $accessible_models = islandora_digital_workflow_get_accessible_cmodels();
  $workflow_model_sequences = islandora_digital_workflow_get_workflow_model_sequences();
  $workflow_sequences = islandora_digital_workflow_get_workflow_sequences();
  $sequence_title = $workflow_sequences[$workflow_sequence_id]['name'];
  $add_model_options = array();
  foreach ($accessible_models as $tid => $accessible_model) {
    if ($accessible_model) {
      $this_model_is_used = FALSE;
      if (array_key_exists($workflow_sequence_id, $workflow_model_sequences)) {
        foreach ($workflow_model_sequences[$workflow_sequence_id] as $sequence) {
          if ($sequence && $accessible_model == $sequence) {
            $this_model_is_used = TRUE;
          }
        }
      }
      if (!$this_model_is_used) {
        $add_model_options[(($tid) ? $accessible_model : '')] = $accessible_model;
      }
    }
  }
  $title = ($sequence_title ? ' "' . $sequence_title . '"' : 'Islandora Digital Workflow');
  drupal_set_title($title, CHECK_PLAIN);
  $breadcrumb = array(
    l('Home', '<front>'),
    l('Islandora', 'admin/islandora'),
    l('Islandora Digital Workflow', 'admin/islandora/islandora_digital_workflow'),
    l('Islandora Digital Workflow - Workflow Sequences', 'admin/islandora/islandora_digital_workflow/workflow_sequences'),
    $title,
  );
  drupal_set_breadcrumb($breadcrumb);

  // Have two controls -- one to add another model to use this sequence and the 
  // other to remove a currently configured model from this sequence.
  $form['configured_models'] = array(
      '#type' => 'item',
      '#title' => t('Configured Models'),
  );
  $use = array_key_exists($workflow_sequence_id, $workflow_model_sequences) ? $workflow_model_sequences[$workflow_sequence_id] : array();
  foreach ($use as $configured_model) {
    if ($configured_model) {
      $form['configured_models']['models'][] = array(
          '#type' => 'item',
          '#markup' => $configured_model,
          '#prefix' => '<blockquote>',
          '#suffix' => '</blockquote>',
      );
    }
  }
  $form['add'] = array(
        '#type' => 'select',
        '#title' => t('Add a Model'),
        '#options' => $add_model_options,
      );
  $tmp = array('' => 'Select Object Model');
  $removable_models = $tmp + (array_key_exists($workflow_sequence_id, $workflow_model_sequences) ? $workflow_model_sequences[$workflow_sequence_id] : array());
  //  array_unshift($workflow_model_sequences[$workflow_sequence_id], 'Select Object Model');
  $form['remove'] = array(
        '#type' => 'select',
        '#title' => t('Remove a Model'),
        '#options' => array_key_exists($workflow_sequence_id, $workflow_model_sequences) ? $workflow_model_sequences[$workflow_sequence_id] : array(),
      );
  $form['workflow_sequence_id'] = array(
      '#type' => 'hidden',
      '#default_value' => $workflow_sequence_id,
  );
  
  $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Save changes')
      );

  $related_batch_nodes = islandora_digital_workflow_get_batches_by_workflow_sequence_id($workflow_sequence_id);

  if (count($related_batch_nodes) > 0) {
    $form['related_batches'] = array(
      '#type' => 'item',
      '#markup' => '<div class="corner_bordered"><h3>Batches using "' . $sequence_title .
          '":</h3><ul><li>' . implode("</li><li>", $related_batch_nodes) . '</li></ul></div>',
    );
  }
  return $form;
}

/**
 * Submit handler for sequence models associations form.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_digital_workflow_workflow_sequence_models_form_submit($form, &$form_state) {
  $add_model = $form_state['values']['add'];
  $remove_model = $form_state['values']['remove'];
  $workflow_sequence_id = $form_state['values']['workflow_sequence_id'];
  if ($workflow_sequence_id && ($add_model || $remove_model)) {
    if ($add_model) {
      if (db_query('REPLACE INTO {islandora_digital_workflow_model_sequence} ' .
              '(workflow_sequence_id, islandora_model) VALUES (' .
              ':workflow_sequence_id, :islandora_model)',
        array(
            ':workflow_sequence_id' => $workflow_sequence_id,
            ':islandora_model' => $add_model,
        ))) {
        drupal_set_message('Model "' . $add_model . '" added to this sequence.');
      }
      else {
        drupal_set_message('There was a problem adding model "' . $add_model . '" to this sequence.');
      }
    }
    else {
      if (db_query('DELETE FROM {islandora_digital_workflow_model_sequence} ' .
              'WHERE workflow_sequence_id = :workflow_sequence_id AND islandora_model = :islandora_model',
        array(
            ':workflow_sequence_id' => $workflow_sequence_id,
            ':islandora_model' => $remove_model,
        ))) {
        drupal_set_message('Model "' . $remove_model . '" removed to this sequence.');
      }
      else {
        drupal_set_message('There was a problem removing model "' . $remove_model . '" to this sequence.');
      }
    }
  }
  else {
    drupal_set_message('No changes were saved because no model was selected.', 'warning');
  }
}

/**
 * Displays configuration options related to reserved actions that can be used
 * for workflow sequence management.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 *
 * @return string|array
 */
function islandora_digital_workflow_workflow_reserved_actions_form(array $form, array &$form_state) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');

  // Get all of the actions (batch_actions and item_actions together).
  $item_actions = islandora_digital_workflow_get_item_actions(TRUE);
  $batch_actions = islandora_digital_workflow_get_batch_action_options(TRUE);
  $reserved_actions = array();
  foreach ($item_actions as $action_id => $item_action) {
    if (strstr($item_action['name'], 'reserved-action-')) {
      $reserved_actions[$action_id] = $item_action;
    }
  }
  foreach ($batch_actions as $action_id => $batch_action) {
    if (strstr($batch_action['name'], 'reserved-action-')) {
      $reserved_actions[$action_id] = $batch_action;
    }
  }

  $form = array(
      'reserved_actions' => array(
          '#type' => 'fieldset',
          '#title' => 'Reserved Actions',
          '#attributes' => array('class' => array('foobar')),
          '#description' => '<p>In order for any of these reserved actions to ' .
              'appear for use in the ' . 
              l('Islandora Digital Workflow - Workflow Sequences', 'admin/islandora/islandora_digital_workflow/workflow_sequences') .
              ', their description must be ' .
              'changed so that it does not begin with "Reserved".</p>',
      ),
  );
  $markup_item_actions = $markup_batch_actions = $action_constants = array();
  foreach ($reserved_actions as $action_id => $action_record_array) {
    $glyph_class = 'transaction_action_' . (($action_record_array['name']) ? strtolower(str_replace(array("-", " "), "_", $action_record_array['name'])) : 'spacer');
    $markup = '<span class="' . $glyph_class . '"><b>Action name:</b><br>' . $action_record_array['name'] .
            '</span>';
    if ($action_record_array['action_id'] > 99) {
      $action_constants[$action_record_array['action_id']] = 
              '<span class="transaction_action_reserved_action_spacer"><b>Action constant:</b><pre>IDW_ACTION_RESERVED_ALL_' . ($action_record_array['action_id'] - 119) . '</pre></span>';
      $markup_batch_actions[$action_record_array['action_id']] = $markup;
    } else {
      $action_constants[$action_record_array['action_id']] =
              '<span class="transaction_action_reserved_action_spacer"><b>Action constant:</b><pre>IDW_ACTION_RESERVED_' . ($action_record_array['action_id'] - 19) . '</pre></span>';
      $markup_item_actions[$action_record_array['action_id']] = $markup;
    }
  }
  $last_action_id = '';
  foreach ($markup_item_actions as $action_id => $markup) {
    $form['reserved_actions']['item_actions']['item_action_' . $action_id] = array(
        '#type' => 'textfield',
        '#title' => 'Action description:',
        '#default_value' => $reserved_actions[$action_id]['description'],
        '#prefix' => ($last_action_id ? '' : '<h3>Item actions</h3>') . $markup,
        '#suffix' => $action_constants[$action_id] . '<br style="clear:both">',
    );
    $last_action_id = $action_id;
  }
  $form['reserved_actions']['item_actions']['item_action_' . $action_id]['#suffix'] = 
      $form['reserved_actions']['item_actions']['item_action_' . $action_id]['#suffix'] . '<hr><h3>Batch actions</h3>';

  foreach ($markup_batch_actions as $action_id => $markup) {
    $form['reserved_actions']['batch_actions']['batch_action_' . $action_id] = array(
        '#type' => 'textfield',
        '#title' => 'Action description:',
        '#default_value' => $reserved_actions[$action_id]['description'],
        '#prefix' => $markup,
        '#suffix' => $action_constants[$action_id] . '<br style="clear:both">',
    );
  }
  $form['submit'] = array(
          '#type' => 'submit',
          '#value' => t('Save changes')
      );

  return $form;
}

/**
 * Submit form handler for managing reserved actions.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_digital_workflow_workflow_reserved_actions_form_submit(array $form, array &$form_state) {
  $form_value_keys = array('item_action_20', 'item_action_21', 'item_action_22',
      'item_action_23', 'item_action_24', 'item_action_25',
      'batch_action_120', 'batch_action_121', 'batch_action_122');
  $updated = FALSE;
  foreach ($form_value_keys as $form_key) {
    $action_id = str_replace(array('item_action_', 'batch_action_'), '', $form_key);
    $updated |= db_update('islandora_digital_workflow_actions')
        ->fields(array('description' => $form_state['values'][$form_key]))
        ->condition('action_id', $action_id)
        ->execute();
  }
  if ($updated) {
    drupal_set_message('The settings have been saved.');
  }
  else {
    drupal_set_message('No changes have been made.');
  }
}
