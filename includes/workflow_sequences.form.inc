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
    $markup_actions = array();
    foreach ($this_workflow_sequence_actions as $action_record_array) {
      $class = ($action_record_array['is_required']) ? 'required_action' : 'optional_action';
      $glyph_class = 'transaction_action_' . (($action_record_array['action_description']) ? strtolower(str_replace(" ", "_", $action_record_array['action_description'])) : 'spacer');
      $markup_actions[] = '<span class="' . $glyph_class . ' ' . $class . '">' . $action_record_array['action_description'] . '</span>';
    }

    $form['wrapper']['workflow_sequence_id_' . $workflow_sequence_id] = array(
      '#type' => 'item',
      '#markup' =>
        '<div class="dashboard-report"><div class="small_right_float">' .
        l('Edit', 'admin/islandora/islandora_digital_workflow/edit_workflow_sequence/' . $workflow_sequence_id) . ' | ' . 
        l('Delete', 'admin/islandora/islandora_digital_workflow/delete_workflow_sequence/' . $workflow_sequence_id) .
        '</div><h3>' . l($name_description['name'], 'admin/islandora/islandora_digital_workflow/edit_workflow_sequence/' . $workflow_sequence_id) . '</h3>' .
        '<p>' . $name_description['description'] . '</p>' . 
        '<blockquote><p><b>' .
        l('Models', 'admin/islandora/islandora_digital_workflow/workflow_sequence_models/' . $workflow_sequence_id) . ':</b> ' . $models .
        '</p><p><b>' .
        l('Actions', 'admin/islandora/islandora_digital_workflow/workflow_sequences/' . $workflow_sequence_id) .
        ':</b><ul><li>' . implode("</li><li>" , $markup_actions) . '</li></ul></p></blockquote></div>',
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

function islandora_digital_workflow_order_delete(array $form, array &$form_state, $workflow_sequence_id) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');

  $workflow_sequence_actions = islandora_digital_workflow_get_workflow_sequence_actions();
  $workflow_model_sequences = islandora_digital_workflow_get_workflow_model_sequences();
  $actions = islandora_digital_workflow_get_item_actions();
  $workflow_sequences = islandora_digital_workflow_get_workflow_sequences();
  $form = array(
      'workflow_sequence_id' => array(
          '#type' => 'hidden',
          '#default_value' => $workflow_sequence_id
      ),
      'warning' => array(
          '#type' => 'item',
          '#markup' => '<span class="bad"><b>This can not be undone!</b>  Are ' . 
            'you certain that you want to delete this workflow sequence and any ' . 
            'action / Islandora model relationships?',
      ),
      'confirm' => array(
          '#type' => 'submit',
          '#value' => t('Confirm DELETE'),
      ),
      'cancel' => array(
          '#type' => 'submit',
          '#value' => t('Cancel'),
      ));
  return $form;
}

function islandora_digital_workflow_order_delete_submit(array $form, array &$form_state) {
  // If cancel is clicked, redirect to the workflow_sequences page
  if ($form_state['triggering_element']['#value'] == 'Cancel') {
    drupal_set_message('Delete was cancelled for workflow sequence.');
  }
  else {
    $workflow_sequence_id = $form_state['values']['workflow_sequence_id'];
    db_query('DELETE FROM {islandora_digital_workflow_sequence} WHERE ' .
        'workflow_sequence_id = :workflow_sequence_id',
            array(':workflow_sequence_id' => $workflow_sequence_id));
    drupal_set_message('Deleted workflow sequence.');
  }
  drupal_goto('admin/islandora/islandora_digital_workflow/workflow_sequences');
}

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
  return $form;
}

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

function islandora_digital_workflow_workflow_sequence_edit(array $form, array &$form_state, $workflow_sequence_id) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
  $workflow_sequences = islandora_digital_workflow_get_workflow_sequences();
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
  return $form;
}

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
 * @return array
 *   Drupal form definition.
 *
 * @return string|array
 */
function islandora_digital_workflow_order_manage(array $form, array &$form_state, $workflow_sequence_id) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');

  $workflow_sequence_actions = islandora_digital_workflow_get_workflow_sequence_actions();
  $workflow_model_sequences = islandora_digital_workflow_get_workflow_model_sequences();
  $actions = islandora_digital_workflow_get_item_actions();
  $workflow_sequences = islandora_digital_workflow_get_workflow_sequences();
  $sequence_title = $workflow_sequences[$workflow_sequence_id]['description'];
  $insert_action_options = array('' => 'Select Action');

  foreach ($actions as $action_id => $arr) {
    $insert_action_options[$action_id] = $arr['description'];
  }
  foreach ($workflow_sequence_actions[$workflow_sequence_id] as $index => $this_workflow_action_array) {
    if (!(array_key_exists($this_workflow_action_array['action_id'], $actions) === FALSE)) {
      unset($insert_action_options[$this_workflow_action_array['action_id']]);
    }
  }
  $form = array(
    '#tree' => TRUE,
    'wrapper' => array(
      '#type' => 'fieldset',
      '#title' => t('Workflow Sequences'),
      '#attributes' => array('class' => array('draggableviews-parent')),
      'add_new' => array(
          '#title' => t('Add another action'),
          '#type' => 'select',
          '#options' => $insert_action_options,
          '#default_value' => '',
      ),
      'models' => array(
          '#type' => 'item',
          '#title' => t('Models'),
          '#description' => l('Edit models association', 'admin/islandora/islandora_digital_workflow/workflow_sequence_models/' . $workflow_sequence_id) .
            str_replace(": ,", ": (default), ", '.  Current Models using this sequence: ' . implode(", ", $workflow_model_sequences[$workflow_sequence_id])) . '. ',
      )
  ));

  
  foreach ($workflow_sequence_actions[$workflow_sequence_id] as $i => $sequence_array) {
    $form['wrapper']['rows'][$sequence_array['action_id']] = array(
      'action_name' => array(
        '#type' => 'textfield',
        '#default_value' => $sequence_array['action_description'],
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
      'action_description' => array(
        '#type' => 'hidden',
        '#default_value' => $sequence_array['action_description'],
      ),
      'op' => array(
        '#type' => 'submit',
        '#value' => 'Remove',
      ),
    );
  }

  $title = t('Islandora Digital Workflow' . ($sequence_title ? ' "' . $sequence_title . '"' : ''));
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
function islandora_digital_workflow_order_manage_submit($form, &$form_state) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
  $actions = islandora_digital_workflow_get_item_actions();
  $current_action_ids = $slides = array();
  $workflow_sequence_id = 0;
  // Removed action_id rows are submitted individually via their [Remove] button.
  $remove_action_id = ($form_state['triggering_element']['#value'] == 'Remove' && array_key_exists(2, $form_state['triggering_element']['#parents'])) ?
          $form_state['triggering_element']['#parents'][2] : 0;
  foreach ($form_state['values']['wrapper']['rows'] as $id => $item) {
    if ($id <> $remove_action_id) {
      $current_action_ids[$item['action_id']] = $item['action_id'];
      $workflow_sequence_id |= $item['workflow_sequence_id'];
    }
  }
  foreach ($form_state['values']['wrapper']['rows'] as $key => $sequence_arr) {
    $workflow_sequence_id = $sequence_arr['workflow_sequence_id'];
  }
  if ($form_state['values']['wrapper']['add_new']) {
    $index = count($form_state['values']['wrapper']['rows']) + 1;
    $insert_row = array(
        'action_id' => $form_state['values']['wrapper']['add_new'],
        'action_name' => $actions[$form_state['values']['wrapper']['add_new']]['name'],
        'workflow_sequence_id' => $workflow_sequence_id,
        'is_required' => 0,
        'weight' => 10,
    );
    $form_state['values']['wrapper']['rows'][$index] = $insert_row;
  }
  if (count($current_action_ids) > 0 && $workflow_sequence_id && $remove_action_id) {
    $deleted = db_query('DELETE FROM {islandora_digital_workflow_sequence_actions} WHERE workflow_sequence_id = :workflow_sequence_id AND action_id NOT IN (' . implode(', ', $current_action_ids) . ')',
      array(
          ':workflow_sequence_id' => $item['workflow_sequence_id'],
      ));
  }
  // Because the form elements were keyed with the item ids from the database,
  // we can simply iterate through the submitted values.
  $remove_button_id = array_key_exists('#id', $form_state['triggering_element']) ? 
      $form_state['triggering_element']['#id'] : '';
  $remove_button_parts = explode("-", $remove_action_id);
  $remove_item_action_id = 0;
  foreach ($remove_button_parts as $id_part) {
    if (!$remove_item_action_id && is_numeric($id_part)) {
      $remove_item_action_id = $id_part;
    }
  }
  foreach ($form_state['values']['wrapper']['rows'] as $id => $item) {
    if ($item['action_id'] <> $remove_item_action_id) {
      $replaced = db_query('REPLACE INTO {islandora_digital_workflow_sequence_actions} ' .
              '(workflow_sequence_id, action_id, is_required, `order`) VALUES (' .
              ':workflow_sequence_id, :action_id, :is_required, :weight)',
        array(
            ':weight' => $item['weight'],
            ':is_required' => $item['is_required'],
            ':workflow_sequence_id' => $item['workflow_sequence_id'],
            ':action_id' => $item['action_id'],
        ));
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
  $sequence_title = $workflow_sequences[$workflow_sequence_id]['description'];
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
  $title = t('Islandora Digital Workflow' . ($sequence_title ? ' "' . $sequence_title . '"' : ''));
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
