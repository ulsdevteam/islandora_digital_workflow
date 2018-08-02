<?php

/**
* @file
* islandora-digital-workflow-file-browser.tpl display template.
*
* Variables available:
* - $folders => array(),
* - $transaction_records => array(),
* - $table_title => '',
* - $table_description => '',
*
*/
?>
<div class="lookup_results">
    <div class="dashboard-report">
      <h3><?php print $table_title; ?></h3>
      <?php if (isset($table_description)) { ?>
      <p><?php print $table_description; ?></p>
      <?php } ?>

      <?php $toggle = FALSE; ?>
      <?php foreach ($folders as $folder) { ?>
      <div class="lookup_result <?php print ($toggle) ? 'evenrow' : 'oddrow'; ?>">
          <?php
          $toggle = !$toggle;
          ?>
          <div class="lookup_result_indent">
            <b>Batch:</b> "<a href="/islandora/islandora_digital_workflow/edit_batch/<?php print $folder['batch_name']; ?>"><?php print $folder['batch_name']; ?></a>"<br>
            <b>Files:</b> <?php print number_format(count($folder['files'])); ?>
            <fieldset class="collapsible collapsed" id="<?php print $folder['batch_name']; ?>">
                <legend><span class="fieldset-legend">Show files</span></legend>
                <div class="fieldset-wrapper" style="display: none;">
                <?php foreach ($folder['files'] as $file): ?>
                    <?php print $file; ?><br>
                <?php endforeach; ?>
                </div>
            </fieldset>
          </div>
      </div>
      <?php } ?>
    </div>
</div>
<br style="clear:both" />
