<div class="lookup_results">
    <div class="dashboard-report">
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
            Edit batch: "<a href="/islandora/islandora_digital_workflow/edit_batch/<?php print $batch_record->batch_name; ?>"><?php print $batch_record->batch_name; ?></a>"<br>
            <b>Description:</b> <?php print $batch_record->batch_description; ?>
          </div>
      </div>
      <?php } ?>
    </div>
<br style="clear:both" />
