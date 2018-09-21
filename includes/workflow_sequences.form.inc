<?php

/**
 * @file
 * Set of forms related to configuring the Workflow Sequences, the Actions,
 * and Islandora model associations per sequence.
 */

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
    l(t('Home'), '<front>'),
    l(t('Administration'), 'admin'),
    l(t('Islandora'), 'admin/islandora'),
    l(t('Islandora Digital Workflow'), 'admin/islandora/islandora_digital_workflow')
  );
  drupal_set_breadcrumb($breadcrumb);

  $workflow_sequences = islandora_digital_workflow_get_workflow_sequences();

  $workflow_sequence_actions = islandora_digital_workflow_get_workflow_sequence_actions();
  $workflow_model_sequences = islandora_digital_workflow_get_workflow_model_sequences();

  $form['link'] = array(
      '#type' => 'item',
      '#markup' => l(t('Manage Actions'), 'admin/islandora/islandora_digital_workflow/workflow_sequence_actions'),
  );
  $form['wrapper'] = array(
      '#type' => 'fieldset',
      '#title' => t('Workflow Sequences'),
  );
  $form['wrapper']['add_new'] = array(
      '#type' => 'item',
      '#markup' => '<ul class="action-links"><li>' . 
        l(t('Create new sequence'), 'admin/islandora/islandora_digital_workflow/create_workflow_sequence') .
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
  foreach ($workflow_sequences as $workflow_sequence_id => $sequence_info) {
    $models = array_key_exists($workflow_sequence_id, $models_arr) ? implode(", ", $models_arr[$workflow_sequence_id]) : '';
    $this_workflow_sequence_actions = array_key_exists($workflow_sequence_id, $workflow_sequence_actions) ? $workflow_sequence_actions[$workflow_sequence_id] : array();
    $markup_item_actions = $markup_batch_actions = array();
    if ($sequence_info['is_mixed']) {
      foreach ($models_arr[$workflow_sequence_id] as $sequence_model) {
        $markup_item_actions[] = $sequence_model;
        $markup_batch_actions[] = $sequence_model;
      }
    }
    foreach ($this_workflow_sequence_actions as $action_record_array) {
      $model_of_this_action = (isset($action_record_array['islandora_model']) ? $action_record_array['islandora_model'] : '');
      $class = (isset($action_record_array['is_required']) ? 'required_action' : 'optional_action');
      $glyph_class = islandora_digital_workflow_glyph_class($action_record_array['action_name']);
      $li_markup = '<li class="' . $glyph_class . ' ' . $class . '">' . (isset($action_record_array['action_description']) ?
              $action_record_array['action_description'] : '[action description missing]') . '</li>';
      if ($model_of_this_action) {
        if (islandora_digital_workflow_is_batch_action($action_record_array)) {
          $markup_batch_actions[$model_of_this_action][] = $li_markup;
        } else {
          $markup_item_actions[$model_of_this_action][] = $li_markup;
        }
      }
      else {
        if (islandora_digital_workflow_is_batch_action($action_record_array)) {
          $markup_batch_actions[] = $li_markup;
        } else {
          $markup_item_actions[] = $li_markup;
        }
      }
    }
    $affected_batch_nodes = islandora_digital_workflow_get_batches_by_workflow_sequence_id($workflow_sequence_id);
    $sequence_in_use = (count($affected_batch_nodes) > 0);
    $sequence_models = array_key_exists($workflow_sequence_id, $models_arr) ? $models_arr[$workflow_sequence_id] : array();
    $form['wrapper']['workflow_sequence_id_' . $workflow_sequence_id] = array(
      '#type' => 'item',
      /**
       * @todo replace all of this markup with a themed template call that passes
       * in the $workflow_sequence_id, $sequence_info['name'], $markup_batch_actions,
       * $markup_item_actions, $models, and value for $sequence_in_use.
       */
      '#markup' => theme('islandora_digital_workflow_admin_workflow_sequence', array(
          'workflow_sequence_id' => $workflow_sequence_id,
          'sequence_name' => $sequence_info['name'],
          'is_mixed' => $sequence_info['is_mixed'],
          'sequence_models' => $sequence_models,
          'markup_batch_actions' => $markup_batch_actions,
          'markup_item_actions' => $markup_item_actions,
          'models' => $models,
          'sequence_in_use' => $sequence_in_use,
          'sequence_last_modified' => $workflow_sequences[$workflow_sequence_id]['how_long_ago'] . 
            ' on ' . $workflow_sequences[$workflow_sequence_id]['max_timestamp'],
      )),
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
 * @return array
 *   Drupal form definition.
 */
function islandora_digital_workflow_workflow_sequence_delete(array $form, array &$form_state, $workflow_sequence_id) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');

  $workflow_sequence_actions = islandora_digital_workflow_get_workflow_sequence_actions();
  $workflow_model_sequences = islandora_digital_workflow_get_workflow_model_sequences();
  $workflow_sequences = islandora_digital_workflow_get_workflow_sequences();
  $sequence_title = $workflow_sequences[$workflow_sequence_id]['name'];
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

  drupal_set_title(t('Islandora Digital Workflow - Delete Workflow Sequence'), CHECK_PLAIN);
  $breadcrumb = array(
    l(t('Home'), '<front>'),
    l(t('Administration'), 'admin'),
    l(t('Islandora'), 'admin/islandora'),
    l(t('Islandora Digital Workflow'), 'admin/islandora/islandora_digital_workflow'),
    l(t('Islandora Digital Workflow - Workflow Sequences'), 'admin/islandora/islandora_digital_workflow/workflow_sequences'),
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
function islandora_digital_workflow_workflow_sequence_delete_submit(array $form, array &$form_state) {
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
      'sequence_is_mixed' => array(
        '#type' => 'checkbox',
        '#title' => t('Mixed objects batch?'),
        '#description' => 'Will this sequence represent more than one object model? ' . 
            '"Mixed" workflow sequences must provide the set of actions for each ' .
            'selected model.',
        '#default_value' => 0,
      ),
      'submit' => array(
          '#type' => 'submit',
          '#value' => t('Save changes')
      ),
  );
  $title = 'Islandora Digital Workflow - Create Workflow Sequence';
  drupal_set_title($title, CHECK_PLAIN);
  $breadcrumb = array(
    l(t('Home'), '<front>'),
    l(t('Administration'), 'admin'),
    l(t('Islandora'), 'admin/islandora'),
    l(t('Islandora Digital Workflow'), 'admin/islandora/islandora_digital_workflow'),
    l(t('Islandora Digital Workflow - Workflow Sequences'), 'admin/islandora/islandora_digital_workflow/workflow_sequences'),
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
      '(`name`, `description`, `is_mixed`) VALUES (:name, ' .
      ':description, :is_mixed)', array(
          ':name' => $form_state['values']['sequence_name'],
          ':description' => $form_state['values']['sequence_description'],
          ':is_mixed' => $form_state['values']['sequence_is_mixed'],
  ));
  $results = db_query('SELECT workflow_sequence_id FROM ' .
      '{islandora_digital_workflow_sequence} WHERE `name` = :name AND ' .
      '`description` = :description', array(
          ':name' => $form_state['values']['sequence_name'],
          ':description' => $form_state['values']['sequence_description'],
  ))->fetchAll();

  $result_obj = array_pop($results);
  $edit_models_link = l(t('Models'), 'admin/islandora/islandora_digital_workflow/workflow_sequence_models/' . $result_obj->workflow_sequence_id);
  $edit_actions_link = l(t('Actions'), 'admin/islandora/islandora_digital_workflow/workflow_sequences/' . $result_obj->workflow_sequence_id);
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
      'sequence_is_mixed' => array(
        '#type' => 'checkbox',
        '#title' => t('Mixed objects batch?'),
        '#description' => 'Will this sequence represent more than one object model? ' .
            '"Mixed" workflow sequences must provide the set of actions for each ' .
            'selected model.',
        '#default_value' => $workflow_sequences[$workflow_sequence_id]['is_mixed'],
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
      '#markup' => theme('islandora_digital_workflow_rounded_box', array(
          'title' => 'Batches using "' . $sequence_title .'":',
          'contents' => '<ul><li>' . implode("</li><li>", $related_batch_nodes) . '</li></ul>')),
    );
  }

  drupal_set_title(t('Islandora Digital Workflow - Edit Workflow Sequence'), CHECK_PLAIN);
  $breadcrumb = array(
    l(t('Home'), '<front>'),
    l(t('Administration'), 'admin'),
    l(t('Islandora'), 'admin/islandora'),
    l(t('Islandora Digital Workflow'), 'admin/islandora/islandora_digital_workflow'),
    l(t('Islandora Digital Workflow - Workflow Sequences'), 'admin/islandora/islandora_digital_workflow/workflow_sequences'),
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
        '(`name`, `description`, `is_mixed`, `workflow_sequence_id`) VALUES (:name, ' .
        ':description, :is_mixed, :workflow_sequence_id)', array(
            ':name' => $form_state['values']['sequence_name'],
            ':description' => $form_state['values']['sequence_description'],
            ':is_mixed' => $form_state['values']['sequence_is_mixed'],
            ':workflow_sequence_id' => $workflow_sequence_id,
    ));
    $edit_models_link = l(t('Models'), 'admin/islandora/islandora_digital_workflow/workflow_sequence_models/' . $workflow_sequence_id);
    $edit_actions_link = l(t('Actions'), 'admin/islandora/islandora_digital_workflow/workflow_sequences/' . $workflow_sequence_id);
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
 * @return array
 *   Drupal form definition.
 */
function islandora_digital_workflow_workflow_sequence_actions_form(array $form, array &$form_state, $workflow_sequence_id) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');

  @list($workflow_sequence_id, $model) = explode('|', $workflow_sequence_id);
  $workflow_sequence_actions = islandora_digital_workflow_get_workflow_sequence_actions();
  $workflow_model_sequences = islandora_digital_workflow_get_workflow_model_sequences();
  $actions = islandora_digital_workflow_get_item_action_options(TRUE);
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
    // If model has been provided, remove all actions from this array that do
    // not match the $model value.
    if ($model) {
      foreach ($workflow_sequence_actions[$workflow_sequence_id] as $key => $actions_array) {
        if ($actions_array['islandora_model'] <> $model) {
          unset($workflow_sequence_actions[$workflow_sequence_id][$key]);
        }
      }
    }
    foreach ($workflow_sequence_actions[$workflow_sequence_id] as $index => $this_workflow_action_array) {
      if (!(array_key_exists($this_workflow_action_array['action_id'], $actions) === FALSE)) {
        unset($insert_action_options[$this_workflow_action_array['action_id']]);
      }
      if (!(array_key_exists($this_workflow_action_array['action_id'], $batch_actions) === FALSE)) {
        unset($insert_batch_action_options[$this_workflow_action_array['action_id']]);
      }
    }
  }
  $models_link = l(t('Edit models association'), 'admin/islandora/islandora_digital_workflow/workflow_sequence_models/' . $workflow_sequence_id);
  $models_desc = str_replace(": ,", ": (default), ", '.  Current Models using this sequence: ' . implode(", ", $workflow_model_sequences[$workflow_sequence_id])) . '. ';

  $form = array(
    '#tree' => TRUE,
    'wrapper' => array(
      '#type' => 'fieldset',
      '#prefix' => (($model) ? '<h3>Model "' . $model . '"</h3>' .
          '<p>Since this workflow sequence is set to handle mixed batches, this page will only ' .
          'manage the actions that are required for "' . $model . '" items in the batch.</p>' :
          ''),
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
          '#description' => $models_link . $models_desc,
      ),
    ),
    'model' => array(
      '#type' => 'hidden',
      '#default_value' => $model,
    ),
  );
  if (array_key_exists($workflow_sequence_id, $workflow_sequence_actions)) {
    foreach ($workflow_sequence_actions[$workflow_sequence_id] as $i => $sequence_array) {
      $form['wrapper']['rows'][$sequence_array['action_id']] = array(
        '#attributes' => array(
          'class' => array((islandora_digital_workflow_is_batch_action($sequence_array) ? 'batch_action_box' : 'item_action_box')),
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
        'is_system_action' => array(
          '#type' => 'checkbox',
          '#default_value' => $sequence_array['is_system_action'],
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
    l(t('Home'), '<front>'),
    l(t('Administration'), 'admin'),
    l(t('Islandora'), 'admin/islandora'),
    l(t('Islandora Digital Workflow'), 'admin/islandora/islandora_digital_workflow'),
    l(t('Islandora Digital Workflow - Workflow Sequences'), 'admin/islandora/islandora_digital_workflow/workflow_sequences'),
    $title,
  );
  drupal_set_breadcrumb($breadcrumb);

  $form['submit'] = array('#type' => 'submit', '#value' => t('Save changes'));

  $related_batch_nodes = islandora_digital_workflow_get_batches_by_workflow_sequence_id($workflow_sequence_id);

  if (count($related_batch_nodes) > 0) {
    $form['related_batches'] = array(
      '#type' => 'item',
      '#markup' => theme('islandora_digital_workflow_rounded_box', array(
          'title' => 'Batches using "' . $sequence_title .'":',
          'contents' => '<ul><li>' . implode("</li><li>", $related_batch_nodes) . '</li></ul>')),
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
  $item_actions = islandora_digital_workflow_get_item_action_options(FALSE);
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
  $model = $form_state['values']['model'];
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
        'is_system_action' => 0,
        'weight' => 10,
    );
    $form_state['values']['wrapper']['rows'][$index] = $insert_row;
  }
  if (strstr($workflow_sequence_id, "|")) {
    @list($workflow_sequence_id, $model) = explode("|", $workflow_sequence_id);
  }
  if ($form_state['values']['batch_wrapper']['add_new_batch_action']) {
    $index = count($form_state['values']['wrapper']['rows']) + 1;
    $insert_row = array(
        'action_id' => $form_state['values']['batch_wrapper']['add_new_batch_action'],
        'action_name' => $all_actions[$form_state['values']['batch_wrapper']['add_new_batch_action']]['name'],
        'workflow_sequence_id' => $workflow_sequence_id,
        'is_required' => 0,
        'is_system_action' => 0,
        'weight' => 10,
    );
    $form_state['values']['wrapper']['rows'][$index] = $insert_row;
  }

  if ($workflow_sequence_id && $remove_action_id) {
    $sql = 'DELETE FROM {islandora_digital_workflow_sequence_actions} WHERE workflow_sequence_id = :workflow_sequence_id' .
        ' AND islandora_model = :model' .
        ((count($current_action_ids) > 0) ? ' AND action_id NOT IN (' . implode(', ', $current_action_ids) . ')' : '');
    $deleted = db_query($sql,
      array(
          ':workflow_sequence_id' => $item['workflow_sequence_id'],
          ':model' => $model,
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
                '(workflow_sequence_id, action_id, islandora_model, is_required, ' .
                'is_system_action, `order`) ' .
                'VALUES (:workflow_sequence_id, :action_id, :islandora_model, :is_required, ' .
                ':is_system_action, :weight)',
          array(
              ':weight' => $item['weight'],
              ':is_required' => $item['is_required'],
              ':is_system_action' => $item['is_system_action'],
              ':workflow_sequence_id' => $workflow_sequence_id,
              ':action_id' => $item['action_id'],
              ':islandora_model' => $model,
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
 * Displays configuration optoins for the Workflow Sequence - Models selection form.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @return array
 *   Drupal form definition.
 * @return array
 *   Drupal form definition.
 */
function islandora_digital_workflow_workflow_sequence_models_form(array $form, array &$form_state, $workflow_sequence_id) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  $accessible_models = islandora_digital_workflow_get_accessible_cmodels();
  $workflow_model_sequences = islandora_digital_workflow_get_workflow_model_sequences();
  $workflow_sequences = islandora_digital_workflow_get_workflow_sequences();
  $this_workflow_sequence = (!array_key_exists($workflow_sequence_id, $workflow_sequences) === FALSE) ?
      $workflow_sequences[$workflow_sequence_id] : array();
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
    l(t('Home'), '<front>'),
    l(t('Islandora'), 'admin/islandora'),
    l(t('Islandora Digital Workflow'), 'admin/islandora/islandora_digital_workflow'),
    l(t('Islandora Digital Workflow - Workflow Sequences'), 'admin/islandora/islandora_digital_workflow/workflow_sequences'),
    $title,
  );
  drupal_set_breadcrumb($breadcrumb);

  // Have two controls -- one to add another model to use this sequence and the 
  // other to remove a currently configured model from this sequence.
  $form = array('configured_wrapper' => array(
      '#type' => 'fieldset',
      '#title' => t('Configured Models'),
      'configured_models' => array(
        '#type' => 'item',
        '#title' => t('Configured Models'),
      ),
    ),
  );

  $use = array_key_exists($workflow_sequence_id, $workflow_model_sequences) ? $workflow_model_sequences[$workflow_sequence_id] : array();
  foreach ($use as $configured_model) {
    if ($configured_model) {
      $form['configured_wrapper']['models'][] = array(
          '#type' => 'item',
          '#markup' => $configured_model,
          '#prefix' => '<blockquote>',
          '#suffix' => '</blockquote>',
      );
    }
  }
  $form['configured_wrapper']['add'] = array(
        '#type' => 'select',
        '#title' => t('Add a Model'),
        '#options' => $add_model_options,
      );
  $tmp = array('' => 'Select Object Model');
  $removable_models = $tmp + (array_key_exists($workflow_sequence_id, $workflow_model_sequences) ? $workflow_model_sequences[$workflow_sequence_id] : array());

  $form['configured_wrapper']['remove'] = array(
        '#type' => 'select',
        '#title' => t('Remove a Model'),
        '#options' => $removable_models, //  array_key_exists($workflow_sequence_id, $workflow_model_sequences) ? $workflow_model_sequences[$workflow_sequence_id] : array(),
      );
  $form['workflow_sequence_id'] = array(
      '#type' => 'hidden',
      '#default_value' => $workflow_sequence_id,
  );
  $form['is_mixed'] = array(
      '#type' => 'hidden',
      '#default_value' => (array_key_exists('is_mixed', $this_workflow_sequence) ? $this_workflow_sequence['is_mixed'] : 0),
  );
  
  $form['configured_wrapper']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Save changes')
      );

  $related_batch_nodes = islandora_digital_workflow_get_batches_by_workflow_sequence_id($workflow_sequence_id);

  if (count($related_batch_nodes) > 0) {
    $form['related_batches'] = array(
      '#type' => 'item',
      '#markup' => theme('islandora_digital_workflow_rounded_box', array(
          'title' => 'Batches using "' . $sequence_title .'":',
          'contents' => '<ul><li>' . implode("</li><li>", $related_batch_nodes) . '</li></ul>')),
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
  $is_mixed = $form_state['values']['is_mixed'];
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
      // For a sequence that is mixed, need to also delete any associations in the
      // `islandora_digital_workflow_sequence_actions` table.
      if ($is_mixed && db_query('DELETE FROM {islandora_digital_workflow_sequence_actions} ' .
              'WHERE workflow_sequence_id = :workflow_sequence_id AND islandora_model = :islandora_model',
        array(
            ':workflow_sequence_id' => $workflow_sequence_id,
            ':islandora_model' => $remove_model,
        ))) {
        drupal_set_message('The references for model "' . $remove_model . '" have been removed for this mixed models workflow sequence.');
      }
    }
  }
  else {
    drupal_set_message('No changes were saved because no model was selected.', 'warning');
  }
}

/**
 * Displays configuration options related to actions that can be used
 * for workflow sequence management.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @return array
 *   Drupal form definition.
 */
function islandora_digital_workflow_actions_form(array $form, array &$form_state) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');

  // Get all of the actions (batch_actions and item_actions together).
  $item_actions = islandora_digital_workflow_get_item_action_options(TRUE);
  $batch_actions = islandora_digital_workflow_get_batch_action_options(TRUE);
  $workflow_sequence_actions = islandora_digital_workflow_get_workflow_sequence_actions();
  $rows = array();
  foreach ($item_actions as $action_id => $item_action) {
    $rows[$action_id] = $item_action;
  }
  foreach ($batch_actions as $action_id => $batch_action) {
    $rows[$action_id] = $batch_action;
  }

  $form = array(
      'reserved_actions' => array(
          '#type' => 'fieldset',
          '#title' => 'Actions',
          '#attributes' => array('class' => array('foobar')),
          '#description' => 'The description and glyph for each action can be ' . 
            'edited.  <i>Actions that have already been associated with any ' .
            'workflow sequence cannot be assigned a different description.</i>',
      ),
  );
  $markup_item_actions = $markup_batch_actions = array();
  foreach ($rows as $action_id => $action_record_array) {
    $trigger_stage = (isset($action_record_array['term_name']) ? $action_record_array['term_name']: '');
    $sequence_ids_using_action = islandora_digital_workflow_using_action($workflow_sequence_actions, $action_id);
    $glyph_class = islandora_digital_workflow_glyph_class($action_record_array['action_name']);
    $markup = '<tr class="' . $glyph_class . '"><td><b>Action name:</b><br>' . $action_record_array['action_name'] .
            '</td></tr>';
    $admin_links = '<div class="admin_links">' .
            ((count($sequence_ids_using_action) > 0) ? '' : '<a href="/admin/islandora/islandora_digital_workflow/workflow_sequence_actions/delete/' . $action_record_array['action_id'] . '" title="Delete"><div class="delete_20">&nbsp;</div></a>') .
            '<a href="/admin/islandora/islandora_digital_workflow/workflow_sequence_actions/edit/' . $action_record_array['action_id'] . '" title="Edit"><div class="edit_20">&nbsp;</div></a>' .
            '</div>';
    
    $arr = array(
        'data' => array(
            array(
                'data' => '<div class="' . $glyph_class . '">&nbsp;</div> ' . $action_record_array['action_name'] . $admin_links, 'class' => array()
            ),
            $action_record_array['action_description'],
            '<small class="disabled_text">' . $glyph_class . '</small>',
            $trigger_stage,
        ),
        'class' => array(),
    );
    if (islandora_digital_workflow_is_batch_action($action_record_array)) {
      $markup_batch_actions[$action_record_array['action_id']] = $arr;
    } else {
      $markup_item_actions[$action_record_array['action_id']] = $arr;
    }
  }
  $markup_batch_actions[] = array(
      'data' => array(
          array('data' => '<div class="transaction_action_unconfigured_reserved_action">&nbsp;</div>' . 
            '<a href="/admin/islandora/islandora_digital_workflow/workflow_sequence_actions/add">Add new batch action</a><div class="admin_links">' . 
            '<a href="/admin/islandora/islandora_digital_workflow/workflow_sequence_actions/add" title="Add"><div class="add_20">&nbsp;</div></a></div>'),
          '&nbsp;',
          '<small class="disabled_text">transaction_action_unconfigured_reserved_action</small>',
          ''
      ),
  );
  $markup_item_actions[] = array(
      'data' => array(
          array('data' => '<div class="transaction_action_unconfigured_reserved_action">&nbsp;</div>' . 
            '<a href="/admin/islandora/islandora_digital_workflow/workflow_sequence_actions/add">Add new item action</a><div class="admin_links">' . 
            '<a href="/admin/islandora/islandora_digital_workflow/workflow_sequence_actions/add" title="Add"><div class="add_20">&nbsp;</div></a></div>'),
          '&nbsp;',
          '<small class="disabled_text">transaction_action_unconfigured_reserved_action</small>',
          ''
      ),
  );

  $header = array('Name', 'Description', 'CSS class name', 'Stage');
  foreach ($markup_item_actions as $action_id => $markup) {
    $form['reserved_actions']['item_actions'] = array(
        '#type' => 'item',
        '#markup' => '<h3>Item Actions</h3>' . theme('table',
            array(
                'header' => $header,
                'rows' => $markup_item_actions,
                'attributes' => array('class' => array('full_width')))
        ),
    );
  }

  foreach ($markup_batch_actions as $action_id => $markup) {
    $form['reserved_actions']['batch_actions'] = array(
        '#type' => 'item',
        '#markup' => '<h3>Batch Actions</h3>' . theme('table',
            array(
                'header' => $header,
                'rows' => $markup_batch_actions,
                'attributes' => array('class' => array('full_width')))
        ),
    );
  }

  return $form;
}

/**
 * Displays current action values and renders a form to edit the action glyph
 * or description.
 *
 * This form is also used by the "Add Action" method.  In that case, the
 * action_id value = 0 is passed in - which should not yield any existing
 * $action_record value.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @param integer $action_id
 *   The `islandora_digital_workflow_actions` table's action_id value.
 * @return array
 *   Drupal form definition.
 */
function islandora_digital_workflow_actions_edit_form(array $form, array &$form_state, $action_id) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/taxonomy-utilities');
  $action_record = islandora_digital_workflow_get_action($action_id);
  $sequences_using_action = islandora_digital_workflow_sequences_using_action($action_id);
  if ($action_id) {
    drupal_set_title(t('Islandora Digital Workflow - Edit Action') . ': "' . $action_record['action_description'] . '"', CHECK_PLAIN);
  }
  else {
    drupal_set_title(t('Islandora Digital Workflow - Add Action'), CHECK_PLAIN);
  }
  $breadcrumb = array(
    l(t('Home'), '<front>'),
    l(t('Administration'), 'admin'),
    l(t('Islandora'), 'admin/islandora'),
    l(t('Islandora Digital Workflow'), 'admin/islandora/islandora_digital_workflow'),
    l(t('Islandora Digital Workflow - Actions Configuration'), 'admin/islandora/islandora_digital_workflow/workflow_sequence_actions'),
  );
  drupal_set_breadcrumb($breadcrumb);

  $module_path = drupal_get_path('module', 'islandora_digital_workflow');
  $static_glyph_filenames = islandora_digital_workflow_static_glyph_filenames();

  $read_only_att = array('readonly' => 'readonly', 'disabled' => 'disabled');
  $selected_glyph_filename = ((count($action_record) > 0) ? $action_record['glyph'] : '');
  $selected_glyph_filename = (($selected_glyph_filename <> '') ? $selected_glyph_filename : 'images/unknown-action_20.png');
  $form['action_id'] = array(
      '#type' => 'hidden',
      '#default_value' => $action_id,
  );

  $form['name_desc_wrapper'] = array(
      '#type' => 'fieldset',
      'name' => array(
          '#type' => 'textfield',
          '#title' => 'Action name',
          '#description' => 'Machine name is used internally do generate HTML ' .
              'and CSS classes for these action transactions.',
          '#default_value' => ((array_key_exists('action_name', $action_record)) ? $action_record['action_name'] : ''),
      ),
      'description' => array(
          '#type' => 'textfield',
          '#title' => 'Action description',
          '#description' => 'This value is used in select boxes and reports.',
          '#default_value' => ((array_key_exists('action_description', $action_record)) ? $action_record['action_description'] : ''),
      ),
  );

  $action_is_batch_action = islandora_digital_workflow_is_batch_action($action_record);
  $form['batch_action_wrapper'] = array(
      '#type' => 'fieldset',
      'is_batch_action' => array(
          '#type' => 'select',
          '#title' => 'Batch action?',
          '#description' => 'If the action is a "batch action" then it can be used ' .
              'at a batch level, else an "item action" is available for items in ' .
              'batches that use a sequence have this action configured.',
          '#options' => array(0 => 'No', 1 => 'Yes'),
          '#default_value' => (isset($action_record['is_batch_action']) ? $action_record['is_batch_action']+0 : 0),
      ),
  );

  $vocab = taxonomy_vocabulary_machine_name_load(IDW_STAGE_TAXONOMY_MACHINE_NAME);
  $terms = islandora_digital_workflow_get_all_terms_of_vocab($vocab);
  $terms_options = array('0' => 'Select a Stage');
  foreach ($terms as $term_obj) {
    $terms_options[$term_obj->tid] = $term_obj->name;
  }
  $form['batch_action_trigger'] = array(
      '#type' => 'fieldset',
      '#title' => t('Trigger (optional)'),
      'stage_taxonomy_tid' => array(
          '#type' => 'select',
          '#default_value' => (isset($action_record['stage_taxonomy_tid']) ? $action_record['stage_taxonomy_tid'] : 0),
          '#title' => 'Triggers workflow "Stage" for related batch?',
          '#options' => $terms_options,
          '#attributes' => array(
            'onchange' => 'stage_selection(this)',
          ),
      ),
      'is_triggered_by_single_item' => array(
          '#type' => 'checkbox',
          '#title' => 'Triggered by a single item?',
          '#description' => ($action_is_batch_action) ?
            'Not applicable for batch-level transactions' :
            'If this is for a "Batch Item" action, can this be triggered by a ' .
            'single batch_item (or must all be required) at a batch level, else ' .
            'an "item action" is available for items in batches that use a ' .
            'sequence have this action configured.',
          '#default_value' => (isset($action_record['is_triggered_by_single_item']) ? $action_record['is_triggered_by_single_item']+0 : 0),
      ),
  );

  // Disable this selection if the action_id is already assigned.
  if ($action_id) {
    $form['batch_action_wrapper']['#attributes'] = array('disabled' => 'disabled');
  }
  else {
    $form['batch_action_trigger']['is_triggered_by_single_item']['#attributes'] = array(
        'disabled' => 'disabled',
        'readonly' => 'readonly');
  }
  $form['glyph_wrapper'] = array(
      '#type' => 'fieldset',
      'glyph' => array(
          '#type' => 'item',
          '#title' => 'Current Glyph',
          '#markup' => theme('islandora_digital_workflow_action_glyph_preview',
              array(
                  'glyph_filenames' => array($selected_glyph_filename => $selected_glyph_filename),
                  'module_path' => $module_path,
                  'selected' => NULL,
                  'normal_size' => FALSE,)),
      ),
      'glyph_selector' => array(
          '#type' => 'item',
          '#title' => 'Select new graphic ['.$selected_glyph_filename.']',
          '#markup' => theme('islandora_digital_workflow_action_glyph_preview',
              array(
                  'glyph_filenames' => $static_glyph_filenames,
                  'module_path' => $module_path,
                  'selected' => $selected_glyph_filename,
                  'normal_size' => TRUE,)),
      ),
  );

  // Only display the message and make the edit fields read-only if the form is
  // not being submitted.
  if (is_array($sequences_using_action) && (array_key_exists('input', $form_state) && !array_key_exists('glyph', $form_state['input']))) {
    $form['name_desc_wrapper']['description']['#attributes'] = $read_only_att;
    drupal_set_message('<div class="bad">This action is being used by the ' .
        'workflow sequences.  Edits to the description for this action are ' .
        'disabled.  It can not be deleted or repurposed (by renaming the ' .
        'action).  Regardless, a different glyph could always be selected.' .
        '<ul>' . implode("</li><li>", $sequences_using_action) . '</li></ul></div>', 'warning');
  }
  $form['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Submit form handler for the actions configuration page.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @return array
 *   Drupal form definition.
 */
function islandora_digital_workflow_actions_edit_form_submit(array $form, array &$form_state) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/islandora_digital_workflow_css');
  $fields = array(
      'name' => is_null($form_state['input']['name']) ? $form_state['values']['name'] : $form_state['input']['name'],
      'description' => is_null($form_state['input']['description']) ? $form_state['values']['description'] : $form_state['input']['description'],
      'glyph' => is_null($form_state['input']['glyph_selector']) ? $form_state['values']['glyph_selector'] : $form_state['input']['glyph_selector'],
      'is_batch_action' => is_null($form_state['input']['is_batch_action']) ? $form_state['values']['is_batch_action'] : $form_state['input']['is_batch_action'],
      'stage_taxonomy_tid' => is_null($form_state['input']['stage_taxonomy_tid']) ? $form_state['values']['stage_taxonomy_tid'] : $form_state['input']['stage_taxonomy_tid'],
      'is_triggered_by_single_item' => is_null($form_state['input']['is_triggered_by_single_item']) ? $form_state['values']['is_triggered_by_single_item'] : $form_state['input']['is_triggered_by_single_item'],
  );
  $action_id = $form_state['input']['action_id'];
  if ($action_id) {
    $rows_updated = db_update('islandora_digital_workflow_actions')
      ->fields($fields)
      ->condition('action_id', $action_id)
      ->execute();
    drupal_set_message($rows_updated . ' rows updated.' .
        (($rows_updated < 1) ? '  <i>(When no changes are made, the action record is not updated)</i>' : ''));
  }
  else {
    $fields['action_id'] = islandora_digital_workflow_get_free_action_id();
    $id_inserted = db_insert('islandora_digital_workflow_actions')
      ->fields($fields)
      ->execute();
    drupal_set_message($id_inserted . ' row inserted');
  }

  // Finally, call this last to generate the updated CSS -- since it operates
  // off of what is in the database, the updates had to come first (above).
  islandora_digital_workflow_update_dynamic_css();

  drupal_set_message('The dynamic CSS has been generated.  You may need to do a "hard-reload" in your browser to see the changes.');

  drupal_goto('admin/islandora/islandora_digital_workflow/workflow_sequence_actions');
}

/**
 * Displays current action values and renders a form to confirm the deletion of
 * a given action.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @param integer $action_id
 *   The `islandora_digital_workflow_actions` table's action_id value.
 * @return array
 *   Drupal form definition.
 */
function islandora_digital_workflow_actions_delete_form(array $form, array &$form_state, $action_id) {
  module_load_include('inc', 'islandora_digital_workflow', 'includes/db');
  module_load_include('inc', 'islandora_digital_workflow', 'includes/utilities');
  $action_record = islandora_digital_workflow_get_action($action_id);
  $sequences_using_action = islandora_digital_workflow_sequences_using_action($action_id);

  if ($action_id && (!is_array($action_record) || count($action_record) < 1)) {
    drupal_set_message('The action could not be located (action_id = "' . 
        $action_id . '").  It is possible that it was just deleted.', 'warning');
    drupal_goto('admin/islandora/islandora_digital_workflow/workflow_sequence_actions');
  }

  if ($sequences_using_action && count($sequences_using_action) > 0) {
    drupal_set_message('<div class="bad">You have been redirected to the Workflow ' .
        'Sequence Actions page.  The action (action_id = ' . $action_id . ') is ' .
        'used by at least one workflow sequence and it can not be deleted.  ' .
        'Sequences that use the action include: <ul>' .
        implode("</li><li>", $sequences_using_action) . '</li></ul></div></div>', 'warning');
    drupal_goto('admin/islandora/islandora_digital_workflow/workflow_sequence_actions');
  }
  drupal_set_title(t('Islandora Digital Workflow - Delete Action') . ': "' . $action_record['action_description'] . '"', CHECK_PLAIN);
  $breadcrumb = array(
    l(t('Home'), '<front>'),
    l(t('Administration'), 'admin'),
    l(t('Islandora'), 'admin/islandora'),
    l(t('Islandora Digital Workflow'), 'admin/islandora/islandora_digital_workflow'),
    l(t('Islandora Digital Workflow - Actions Configuration'), 'admin/islandora/islandora_digital_workflow/workflow_sequence_actions'),
  );
  drupal_set_breadcrumb($breadcrumb);
  $form = array(
      'confirm_question' => array(
          '#type' => 'item',
          '#markup' => '<p style="font-size:bigger">Are you sure that you want ' .
              'to delete the action "' . $action_record['action_description'] . '"?',
      ),
      'action_id' => array(
          '#type' => 'hidden',
          '#default_value' => $action_record['action_id'],
      ),
      'confirm' => array(
          '#type' => 'submit',
          '#value' => t('Confirm DELETE'),
      ),
      'cancel' => array(
          '#type' => 'submit',
          '#value' => t('Cancel'),
      ),
  );
  return $form;
}

/**
 * This will prompt the user to confirm the deletion of an action.
 *
 * If confirmed, the action will be deleted.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_digital_workflow_actions_delete_form_submit(array $form, array &$form_state) {
  // If cancel is clicked, redirect to the workflow_sequences page
  $action_record = islandora_digital_workflow_get_action($form_state['values']['action_id']);
  if (!is_array($action_record) || count($action_record) < 1) {
    drupal_set_message('The action could not be located (action_id = "' . $form_state['values']['action_id'] . '").', 'warning');
    drupal_goto('admin/islandora/islandora_digital_workflow/workflow_sequence_actions');
  }
  else {
    if ($form_state['triggering_element']['#value'] == 'Cancel') {
      drupal_set_message('Delete was cancelled for action "' . $action_record['action_description'] . '".');
      drupal_goto('admin/islandora/islandora_digital_workflow/workflow_sequence_actions');
    }
    else {
      module_load_include('inc', 'islandora_digital_workflow', 'includes/islandora_digital_workflow_css');
      drupal_set_message('Action "' . $action_record['action_description'] . '" deleted.');
      $rows_deleted = db_delete('islandora_digital_workflow_actions')
          ->condition('action_id', $form_state['values']['action_id'])
          ->execute();
      islandora_digital_workflow_update_dynamic_css();
    }
  }
}

/**
 * Simple hander to route the "Add" request through the "Edit" form handler.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @return type
 */
function islandora_digital_workflow_actions_add_form(array $form, array &$form_state) {
  $form = drupal_get_form('islandora_digital_workflow_actions_edit_form', 0);
  return $form;
}

