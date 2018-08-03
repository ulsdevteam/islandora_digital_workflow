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
*
*/
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
      <p><b><?php print l(t('Models'), 'admin/islandora/islandora_digital_workflow/workflow_sequence_models/' . $workflow_sequence_id); ?>:</b> <?php print $models; ?></p>
      <p><b><?php print l(t('Actions'), 'admin/islandora/islandora_digital_workflow/workflow_sequences/' . $workflow_sequence_id); ?>:</b>
      <ul>
      <?php if (count($markup_batch_actions) > 0): ?>
        <li class="no_indent">Batch actions</li>
        <ul class="batch_action_box corner_bordered"><?php print implode("\n" , $markup_batch_actions); ?></ul>
      <?php endif; ?>
      <?php if (count($markup_item_actions) > 0): ?>
        <li class="no_indent">Item actions</li>
        <ul class="corner_bordered item_action_box"><?php print implode("\n" , $markup_item_actions); ?></ul>
      <?php endif; ?>
      </ul>
    </div>
  </div>