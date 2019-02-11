<?php

/**
* @file
* islandora-digital-workflow-batch-records-table.tpl display template.
*
* Variables available:
* - $batch_records => array() or stdClass, depending on the acutal routine that
*                  found these batch records, there may be additional fields
 *                 added to the stdClass object such as:
 *                   $batch_record->node_field_object_count or $batch_record->is_batch_request
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

      <?php $toggle = TRUE; ?>
      <?php foreach ($batch_records as $batch_record) { ?>
      <div class="lookup_result <?php print ($toggle) ? 'evenrow' : 'oddrow'; ?>">
          <?php
          $toggle = !$toggle;
          ?>
          <fieldset class="collapsible collapsed straight-edge" id="box_<?php print str_replace(" ", "-", $batch_record->batch_id); ?>_<?php print $table_title; ?>" >
          <!-- <div class="lookup_result_indent"> -->
              <legend><span class="fieldset-legend"><?php print $batch_record->batch_name; ?></span></legend>
              <div class="fieldset-wrapper" style="display: none;">
                <h3><a href="<?php print ($batch_record->nid) ? '/node/' . $batch_record->nid . '/batch' : '/islandora/islandora_digital_workflow/edit_batch/' . $batch_record->batch_name; ?>"><?php print $batch_record->batch_name; ?></a></h3>
                <ul class="indented_ul">
                <li><b>Description:</b> <?php print nl2br($batch_record->batch_description); ?></li>
                <?php if (isset($batch_record->priority) && $batch_record->priority): ?>
                <li><b>Priority:</b> <?php print $batch_record->priority; ?></li>
                <?php endif; ?>
                <?php if (isset($batch_record->object_count) && isset($batch_record->items_count)): ?>
                <li><b>Actual items count:</b> <span class="bad"><?php print $batch_record->object_count; ?></span></li>
                <li><b>Intended count:</b> <span class="bad"><?php print $batch_record->items_count; ?></span></li>
                <?php endif; ?>
                <?php if (isset($batch_record->is_batch_request) && isset($batch_record->how_long_from_now) && $batch_record->is_batch_request): ?>
                <li><b>Requestor:</b> <?php print $batch_record->batch_requestor; ?></li>
                <li><b>Request due date:</b> <span class="bad"><?php print format_date($batch_record->batch_request_due_date, 'custom', 'Y/m/d'); ?></span>
                    <i>- <?php print $batch_record->how_long_from_now; ?></i></li>
                <?php endif; ?>
                </ul>
              </div>
          </fieldset>
      </div>
      <?php } ?>
    </div>
</div>
<br style="clear:both" />
