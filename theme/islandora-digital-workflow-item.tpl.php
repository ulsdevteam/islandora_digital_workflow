<?php

/**
* @file
* islandora-digital-workflow-item.tpl display template.
*
* Variables available:
* - $batch_record => array(),
* - $item => stdObject,
* - $transaction_records => array(),
* - $item_record_transactions => array(),
* - $scanned_files => array(),
*/
?>
<div id="no-sidebars">
  <h3>Return to <b><a href="../items"><?php print $batch_record->batch_name; ?></a></b></h3>

  <h3>Item Details</h3>
  <div class="lookup_result oddrow">
      <form action="" method="POST" enctype="multipart/form-data">
        <fieldset class="lookup_result_indent evenrow"<?php print ($can_update) ? '' : ' disabled' ?>>
            <label for="edit-title">Title: </label>
            <input id="edit-title" name="title" value="<?php print htmlspecialchars($item->title); ?>">
            <label for="edit-identifier">Identifier: </label>
            <input id="edit-identifier" name="identifier" value="<?php print $item->identifier; ?>">
            <label for="edit-filename">Filename: </label>
            <input id="edit-filename" name="filename" value="<?php print $item->filename_basename; ?>">
            <label for="edit-pending1" class="disabled_text">ITEM PROP 1: </label>
            <input id="edit-pending1" name="pending" disabled readonly="readonly" value="pending development">
            <label for="edit-pending2" class="disabled_text">ITEM PROP 2: </label>
            <input id="edit-pending2" name="pending" disabled readonly="readonly" value="pending development">
            <label for="edit-pending3" class="disabled_text">ITEM PROP 3: </label>
            <input id="edit-pending3" name="pending" disabled readonly="readonly" value="pending development">

            <label for="edit-mods">MODS: </label>
            <textarea rows=8 class="short-text-area" id="edit-mods" name="mods"><?php print $item->mods; ?></textarea>
            <em>NOTE:</em> The MODS is generated from the CSV upload and editing this here may not be the right thing to do.
            <?php if ($is_paged_content) : ?>
            <div>
                <label for="edit-marc-file">MARC (MAchine-Readable Cataloging) record:</label><br>
                <input type="file" id="edit-marc-file" name="marc_file" />
            </div>
            <textarea rows=8 class="short-text-area" id="edit-marc" name="marc"><?php print $item->marc; ?></textarea>

            <?php endif; ?>
        </fieldset>
      <?php if ($can_update) : ?>
      <input type="submit" value="Update Batch Item">
      <?php endif; ?>
      </form>
  </div>


  <div class="report_table">
    <h3>Item Transactions</h3>
    <?php if (count($item_record_transactions) > 0) : ?>
    <?php $toggle = FALSE; ?>
    <table>
        <tr>
            <th>Description</th>
            <th>When</th>
            <th>User</th>
            <th>Timestamp</th>
        </tr>
      <?php foreach ($item_record_transactions as $transaction_record) { ?>
          <?php
          $toggle = !$toggle;
          ?>
          <tr class="<?php print ($toggle) ? 'evenrow' : 'oddrow'; ?>">
              <td>
                <div class="<?php
                  print $transaction_record->glyph_class;
                  if ($transaction_record->already_exists) {
                    print ' optional_action';
                  }
                ?>">&nbsp;</div>
                <?php if ($transaction_record->already_exists): ?>
                <span class="optional_action">
                <?php endif; ?>
                <?php print $transaction_record->description; ?>
                <?php if ($transaction_record->already_exists): ?>
                </span>
                <?php endif; ?>

                <?php if ($transaction_record->admin_links <> ''): ?>
                  <div class="admin_links">
                      <?php if ($transaction_record->transaction_id <> -1): ?>
                      <a href="/node/<?php print $transaction_record->nid; ?>/delete_transaction/<?php print $transaction_record->transaction_id; ?>" title="Delete">
                        <div class="delete_20">&nbsp;</div></a>
                      <a href="/node/<?php print $transaction_record->nid; ?>/edit_transaction/<?php print $transaction_record->transaction_id; ?>" title="Edit">
                        <div class="edit_20">&nbsp;</div></a>
                      <?php else: ?>
                      <a href="/node/<?php print $transaction_record->nid; ?>/add_transaction/<?php print $item->batch_item_id; ?>/<?php print $transaction_record->action_id; ?>" title="Add">
                        <div class="add_20">&nbsp;</div></a>
                      <?php endif; ?>
                  </div>
                <?php endif; ?>
              </td>
              <td><?php print $transaction_record->how_long_ago; ?></td>
              <td><?php print $transaction_record->user_name; ?></td>
              <td><?php print $transaction_record->timestamp; ?></td>
          </tr>
      <?php } ?>
    </table>
    <?php else: ?>
    <em>There are no item transactions yet for this batch item.</em>
    <?php endif; ?>
    <hr>
    <?php print $workflow_sequence_text; ?>

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
