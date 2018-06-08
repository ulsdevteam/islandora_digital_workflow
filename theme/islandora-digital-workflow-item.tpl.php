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
  <h3>Return to <b><a href="../items"><?php print $batch_record->batch_name; ?></a></b></h3>
  <div class="report_table">
    <?php if (count($item_record_transactions) > 0) : ?>
    <?php $toggle = FALSE; ?>
    <h3>Item Transactions</h3>
    <table>
        <tr>
            <th>Description</th>
            <th>When</th>
            <th>Timestamp</th>
        </tr>
      <?php foreach ($item_record_transactions as $transaction_record) { ?>
          <?php
          $toggle = !$toggle;
          ?>
          <tr class="<?php print ($toggle) ? 'evenrow' : 'oddrow'; ?>">
              <td>
                <div class="<?php print $transaction_record->glyph_class; ?>">&nbsp;</div>
                <?php print $transaction_record->description; ?>
                <?php if ($transaction_record->admin_links <> ''): ?>
                  <div class="admin_links">
                      <a href="/node/<?php print $transaction_record->nid; ?>/edit_transaction/<?php print $transaction_record->transaction_id; ?>" title="Edit">
                        <div class="edit_20">&nbsp;</div></a>
                      <a href="/node/<?php print $transaction_record->nid; ?>/delete_transaction/<?php print $transaction_record->transaction_id; ?>" title="Delete">
                        <div class="delete_20">&nbsp;</div></a>
                  </div>
                <?php endif; ?>
              </td>
              <td><?php print $transaction_record->how_long_ago; ?></td>
              <td><?php print $transaction_record->timestamp; ?></td>
          </tr>
      <?php } ?>
    </table>
    <?php endif; ?>

    <h3>Item Details</h3>
    <div class="lookup_result oddrow">
        <form action="" method="POST">
          <fieldset class="lookup_result_indent evenrow"<?php print ($can_update) ? '' : ' disabled' ?>>
              <label for="edit-title">Title: </label>
              <input id="edit-title" name="title" value="<?php print htmlspecialchars($item->title); ?>">
              <label for="edit-identifier">Identifier: </label>
              <input id="edit-identifier" name="identifier" value="<?php print $item->identifier; ?>">
              <label for="edit-filename">Filename: </label>
              <input id="edit-filename" name="filename" value="<?php print $item->filename_basename; ?>">
              <label for="edit-mods">MODS: </label>
              <textarea rows=8 class="short-text-area" id="edit-mods" name="mods"><?php print $item->mods; ?></textarea>
              <em>NOTE:</em> The MODS is generated from the CSV upload and editing this here may not be the right thing to do.
          </fieldset>
        <?php if ($can_update) : ?>
        <input type="submit" value="Update Batch Item">
        <?php endif; ?>
        </form>
    </div>

    <?php if (count($scanned_files) > 0) : ?>
    <?php $toggle = FALSE; ?>
    <h3>Files</h3>
      <table>
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
