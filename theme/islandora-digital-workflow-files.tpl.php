<?php

/**
* @file
* islandora-digital-workflow-transactions.tpl display template.
*
* Variables available:
* - $batch_record => array(),
* - $transaction_records => array(),
* - $table_title => '',
* - $table_description => '',
*
*/
?>
<div id="no-sidebars">
  <div class="report_table">
    <h3><?php print $table_title; ?></h3>
    <?php if (isset($table_description)) { ?>
    <p><?php print $table_description; ?></p>
    <?php } ?>

    <?php $toggle = FALSE; ?>
    <div class="lookup_result <?php print ($toggle) ? 'evenrow' : 'oddrow'; ?>">
        <?php
        $toggle = !$toggle;
        ?>
        <div class="lookup_result_indent">
            <b>Description:</b> <?php print $batch_record->batch_description; ?><br>
            <span class="<?php print (count($item_file_records) == $batch_record->object_count) ? 'good' : 'bad'; ?>">
            <b>How many physical objects:</b><?php print $batch_record->object_count; ?>
            <?php if (count($item_file_records) <> $batch_record->object_count) : ?>
            <em>(<?php print count($item_file_records); ?> items records)</em>
            <?php endif; ?>
            </span>
        </div>
    </div>

    <?php if (count($item_file_records) > 0) : ?>
    <table>
        <tr>
            <th>Title</th>
            <th>Identifier</th>
            <th>Filename</th>
            <th>Size</th>
        </tr>
      <?php foreach ($item_file_records as $item) { ?>
          <?php $toggle = !$toggle; ?>
          <tr class="<?php print (($toggle) ? 'evenrow' : 'oddrow') .
              (($item->class <> '') ?  ' ' . $item->class : '') ; ?>">
              <td><?php print $item->title; ?></td>
              <td><?php print $item->identifier; ?></td>
              <td><?php print $item->filename_basename; ?></td>
              <td><?php print $item->filesize; ?></td>
          </tr>
      <?php } ?>
    </table>
    <?php endif; ?>

    <?php if (count($scanned_files) > 0) : ?>
    <h3>Actual files found in batch folder</h3>
    <p>The batch folder "<?php print $batch_path; ?>" contains the following files.
    Files that are marked in green are expected, while the red files are not referenced
    by the batch items; it is possible that those files may not cause any problems.</p>
    <table style="width: 50%">
        <tr>
            <th>Filename</th>
            <th class="numeric">Size</th>
        </tr>
      <?php foreach ($scanned_files as $filename => $file_info) { ?>
          <?php $toggle = !$toggle; ?>
          <tr class="<?php print (($toggle) ? 'evenrow' : 'oddrow') .
              (($file_info['class'] <> '') ? ' ' . $file_info['class'] : ''); ?>">
              <td><?php print $filename; ?></td>
              <td class="numeric"><?php print number_format($file_info['filesize']); ?></td>
          </tr>
      <?php } ?>
    </table>
    <?php endif; ?>

  </div><!-- /end dashboard-report -->

</div><!-- /end no-sidebars -->