<?php

/**
* @file
* islandora-digital-workflow-batch-records-table.tpl display template.
*
* Variables available:
* - $batch_records => array(),
* - $transaction_records => array(),
* - $table_title => '',
* - $table_description => '',
*
*/
?>
<div class="lookup_results">
    <div class="dashboard-report messages error">
      <h3><?php print $table_title; ?></h3>
      <?php if (isset($table_description)) { ?>
      <p><?php print $table_description; ?></p>
      <?php } ?>

      <?php $toggle = FALSE; ?>
      <?php foreach ($batch_records as $batch_record) { ?>
      <div class="lookup_result <?php print ($toggle) ? 'evenrow' : 'oddrow'; ?>">
          <?php
          $toggle = !$toggle;
          ?>
          <div class="lookup_result_indent">
              <h3><a href="/islandora/islandora_digital_workflow/edit_batch/<?php print $batch_record->batch_name; ?>"><?php print $batch_record->batch_name; ?></a></h3>
              <ul>
              <li><b>Priority:</b> <?php print $batch_record->priority; ?></li>
              <li><b>Description:</b> <?php print $batch_record->batch_description; ?></li>
              <?php if (isset($batch_record->is_batch_request) && $batch_record->is_batch_request): ?>
              <li><b class="bad">Request due date:</b> <?php print format_date($batch_record->batch_request_due_date, 'custom', 'Y/m/d'); ?></li>
              <li><b>Requestor:</b> <?php print $batch_record->batch_requestor; ?></li>
              <?php endif; ?>
              </ul>
          </div>
      </div>
      <?php } ?>
    </div>
</div>
<br style="clear:both" />
