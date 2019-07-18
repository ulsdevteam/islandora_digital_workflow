<?php

/**
* @file
* islandora-digital-workflow-files.tpl display template.
*
* Variables available:
* - $batch_record => array(),
* - $item_file_records => array(),
* - $found_files => array(),
* - $table_title => '',
* - $table_description => '',
*
*/
?>
<?php $toggle = TRUE; ?>
<div id="no-sidebars">
  <div class="report_table">
    <h3><?php print $table_title; ?></h3>
    <?php if (isset($table_description)) { ?>
    <p><?php print $table_description; ?></p>
    <?php } ?>

    <div class="lookup_result <?php print ($toggle) ? 'evenrow' : 'oddrow'; ?>">
    <?php $toggle = TRUE; ?>
        <div class="lookup_result_indent">
            <span class="<?php print (count($item_file_records) == $batch_record['object_count']) ? 'good' : 'bad'; ?>">
            <b>How many physical objects:</b><?php print $batch_record['object_count']; ?>
            <?php if (count($item_file_records) <> $batch_record['object_count']) : ?>
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
            <?php if ($is_paged_content): ?><th>Master Filename</th><?php endif; ?>
            <th>Size</th>
        </tr>
      <?php foreach ($item_file_records as $item) { ?>
          <?php $toggle = !$toggle; ?>
          <tr class="<?php print (($toggle) ? 'evenrow' : 'oddrow') .
              (($item->class <> '') ?  ' ' . $item->class : '') ; ?>">
              <td><?php print $item->title; ?></td>
              <td><a href="/node/<?php print $node->nid; ?>/item/<?php print $item->batch_item_id; ?>"><?php print $item->identifier; ?></a></td>
              <?php if ($is_paged_content): ?>
                <td><?php print $item->master_filename_basename; ?></td>
              <?php endif; ?>
              <td><?php print $item->filesize; ?></td>
          </tr>
      <?php } ?>
    </table>
    <?php endif; ?>
    <?php if (count($found_files) > 0) : ?>
    <h3>Working Files</h3>
    <p>The batch folder "<?php print $batch_path; ?>" contains the following files.
    Files that are marked in green are expected, while the red files are not referenced
    by the batch items; it is possible that those files may not cause any problems.</p>
      <?php $otoggle = FALSE; ?>
      <?php $total_size = 0;  $last_path = ''; ?>
      <?php foreach ($item_file_records as $item) { ?>
        <?php $otoggle = !$otoggle; ?>
        <?php $toggle = FALSE; ?>

<fieldset  class="not_white <?php print (($otoggle) ? 'evenrow' : 'oddrow'); ?>" id="<?php print $item->identifier; ?>">
    <legend>
      <span class="fieldset-legend">
          <a href="/node/<?php print $node->nid; ?>/item/<?php print $item->batch_item_id; ?>"><?php print $item->identifier; ?></a></span>
    </legend>
    <div class="text-report scroll-v-200">
      <table>
        <tr>
          <th>Filename</th>
          <th class="numeric">Size (bytes)</th>
        </tr>

        <?php $items_found_files = (array_key_exists($item->identifier, $found_files) ? $found_files[$item->identifier] : array()); ?>
        <?php foreach ($items_found_files as $filename => $file_info) { ?>
          <?php if (strstr($file_info['scan_path'], '/' . $item->identifier . '/')) : ?>
            <?php if ($last_path <> $file_info['scan_path']) { $last_path =  $file_info['scan_path']; } ?>
            <?php $total_size += $file_info['filesize']; ?>
            <?php $toggle = !$toggle; ?>
            <tr class="<?php print (($toggle) ? 'evenrow' : 'oddrow') .
              (($file_info['class'] <> '') ? ' ' . $file_info['class'] : ''); ?>">
            <td><?php print $filename; ?></td>
            <td class="numeric"><?php print number_format($file_info['filesize']); ?></td>
          </tr>
          <?php endif; ?>
        <?php } ?>
      </table>
    </div>
    <div class="right-total"><span><b>Total: </b><?php print number_format($total_size); ?> bytes</span></div>
        <small class="small_lt_text"><b>Files location:</b> <?php print $last_path; ?></small>
    
</fieldset>

    <?php } ?>
    <?php endif; ?>

  </div><!-- /end dashboard-report -->

</div><!-- /end no-sidebars -->
