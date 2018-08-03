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
      <?php if (isset($table_description)) : ?>
      <p><?php print $table_description; ?></p>
      <?php endif; ?>

      <?php $toggle = FALSE; ?>
      <?php foreach ($folders as $folder) { ?>
      <div class="lookup_result <?php print ($toggle) ? 'evenrow' : 'oddrow'; ?>">
          <?php
          $toggle = !$toggle;
          ?>
          <div class="lookup_result_indent">
            <?php if ($action_link): ?>
            <a href="<?php print $action_link . $folder['batch_name']; ?>">Sync "<?php print $folder['batch_name']; ?>" back to working directories.</a><hr>
            <?php endif; ?>
            <b>Batch:</b> "<a href="/islandora/islandora_digital_workflow/edit_batch/<?php print $folder['batch_name']; ?>"><?php print $folder['batch_name']; ?></a>"<br>
            <b>Files:</b> <?php print number_format(count($folder['files_table_rows'])); ?>
            <fieldset class="collapsible collapsed straight-edge" id="<?php print $folder['batch_name']; ?>">
                <legend><span class="fieldset-legend">Show files</span></legend>
                <table class="fieldset-wrapper" style="display: none;">
                    <tr>
                        <th>Filename</th>
                        <th>Size</th>
                        <th>Modified</th>
                    </tr>
                    <?php print $folder['files_table_rows']; ?>
                </table>
            </fieldset>
          </div>
      </div>
      <?php } ?>
      <?php if ($root): ?><span class="small_lt_text">Folders and files located under: <em><?php print $root; ?></em></span><?php endif; ?>
    </div>
</div>
<br style="clear:both" />
