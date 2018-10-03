<?php

/**
* @file
* islandora-digital-workflow-admin-workflow-sequence.tpl display template.
*
* Variables available:
* - $workflow_sequence_id => '',
* - $sequence_name => '',
* - $markup_batch_actions => array(),
* - $markup_item_actions => array(),
* - $models => '',
* - $sequence_in_use => FALSE,
* - $sequence_last_modified 
*
*/
  // This will be handy later when the l() function is used.
  $link_button_class = array('attributes' => array('class' => array('link_button')));
?><div class="dashboard-report">
    <div class="small_right_float">
      <?php print l(t('Edit'), 'admin/islandora/islandora_digital_workflow/edit_workflow_sequence/' . $workflow_sequence_id); ?>
      <?php if ($sequence_in_use) : ?>
        &nbsp;| <span class="disabled_text" title="This sequence can not be deleted since it is used by at least one active batch.">Delete</span>
      <?php else: ?>
        &nbsp;| <?php print l(t('Delete'), 'admin/islandora/islandora_digital_workflow/delete_workflow_sequence/' . $workflow_sequence_id); ?>
      <?php endif; ?>
    </div>
    <h3><a name="<?php print $sequence_name; ?>"><?php print l($sequence_name, 'admin/islandora/islandora_digital_workflow/edit_workflow_sequence/' . $workflow_sequence_id); ?></a></h3>
    <p><?php print $sequence_name; ?></p>
    <div class="lookup_result_square">
      <p><?php print l(t('Edit Models'), 'admin/islandora/islandora_digital_workflow/workflow_sequence_models/' . $workflow_sequence_id, $link_button_class); ?> <?php if ($models) { print 'Currently configured Models: ' . $models; } ?></p>
      <?php if ($is_mixed): ?>
        <ul>
        <?php foreach ($sequence_models as $model): ?>
          <p><b><?php print l('Edit ' . $model . t(' Actions'), 'admin/islandora/islandora_digital_workflow/workflow_sequences/' . $workflow_sequence_id . '|' . $model, $link_button_class); ?>:</b></p>
          <ul>
          <?php if (array_key_exists($model, $markup_batch_actions) && count($markup_batch_actions[$model]) > 0): ?>
            <li class="no_indent">Batch actions
              <ul class="batch_action_box corner_bordered"><?php print implode("\n" , $markup_batch_actions[$model]); ?></ul></li>
          <?php endif; ?>
          <?php if (array_key_exists($model, $markup_item_actions) && count($markup_item_actions[$model]) > 0): ?>
            <li class="no_indent">Item actions
              <ul class="corner_bordered item_action_box"><?php print implode("\n" , $markup_item_actions[$model]); ?></ul></li>
          <?php endif; ?>
          </ul>
        <?php endforeach; ?>
          <?php if ($sequence_last_modified): ?>
          <li><div class="admin_links disabled_text">Sequence last modified: <b><?php print (($sequence_last_modified == '0000-00-00 00:00:00') ? '(not set)' : $sequence_last_modified); ?></b></div></li>
          <?php endif; ?>
        </ul>
      <?php else: ?>
        <p><?php print l(t('Edit Actions'), 'admin/islandora/islandora_digital_workflow/workflow_sequences/' . $workflow_sequence_id, $link_button_class); ?></p>
        <ul>
        <?php if (count($markup_batch_actions) > 0): ?>
          <li class="no_indent">Batch actions
            <ul class="batch_action_box corner_bordered"><?php print implode("\n" , $markup_batch_actions); ?></ul></li>
        <?php endif; ?>
        <?php if (count($markup_item_actions) > 0): ?>
          <li class="no_indent">Item actions
            <ul class="corner_bordered item_action_box"><?php print implode("\n" , $markup_item_actions); ?></ul></li>
        <?php endif; ?>
          <?php if ($sequence_last_modified): ?>
          <li><div class="admin_links disabled_text">Sequence last modified: <b><?php print (($sequence_last_modified == '0000-00-00 00:00:00') ? '(not set)' : $sequence_last_modified); ?></b></div></li>
          <?php endif; ?>
        </ul>
      <?php endif; ?>
    </div>
  </div>