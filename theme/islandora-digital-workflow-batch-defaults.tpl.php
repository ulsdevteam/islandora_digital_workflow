<?php

/**
 * @file
 * islandora-digital-workflow-batch-defaults.tpl display template.
 *
 * This template will render all of the fields that are in a given batch record
 * in a way that can be human readable.  This is only used to set the nodes'
 * body value for workflow_batch type nodes.
 *
 * Variables available:
 * - $islandora_digital_workflow_batch array
 */

?>

<div id="islandora-digital-workflow-batch-defaults">
  <h3>Batch Defaults</h3>
  <?php if (is_array($islandora_digital_workflow_batch)) { ?>
  Edit Batch: "<?php print l($islandora_digital_workflow_batch['batch_name'],
        'islandora/islandora_digital_workflow/edit_batch/' . $islandora_digital_workflow_batch['batch_name']); ?>"<hr>
  <?php
    foreach ($islandora_digital_workflow_batch as $field => $value) {
      $span_id = "islandora-digital-workflow-" . $field; ?>
  <div><label for='<?php print $span_id; ?>'><?php print $field; ?></label> <span id='<?php print $span_id; ?>'>"<?php print $value; ?>"</span></div>
    <?php }
  } ?>
</div><!-- /end islandora-digital-workflow-batch-defaults -->
