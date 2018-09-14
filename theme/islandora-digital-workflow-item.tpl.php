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
* - $found_files => array(),
*/
?>
<div id="no-sidebars">

  <?php if (is_array($unresolved_problems) && count($unresolved_problems) > 0): ?>
  <div class="dashboard-report messages error">
    <h3>Unresolved Problem/s</h3>
    <?php foreach ($unresolved_problems as $unresolved_problem): ?>
    <div>
      <label>Initially logged:</label> <?php print $unresolved_problem->problem_how_long_ago . ' on ' . $unresolved_problem->problem_timestamp; ?> by <?php print $unresolved_problem->user_name; ?><br>
      <label>Problem notes:</label><pre><?php print $unresolved_problem->problem_notes ?></pre>
    </div>
    <?php endforeach; ?>
  </div>
  <?php endif; ?>

  <?php if (is_array($previous_problems) && count($previous_problems) > 0): ?>
  <div class="dashboard-report messages status">
    <h3>Resolved Problem/s</h3>
    <?php foreach ($previous_problems as $previous_problem): ?>
    <div>
      <label>Initially logged:</label> <?php print $previous_problem->problem_how_long_ago . ' on ' . $previous_problem->problem_timestamp; ?> by <?php print $previous_problem->user_name; ?><br>
      <label>Problem resolved:</label> <?php print $previous_problem->problem_resolved_how_long_ago . ' on ' . $previous_problem->problem_resolved_timestamp; ?><br>
      <label>Problem notes:</label><pre><?php print $previous_problem->problem_notes ?></pre>
    </div>
    <?php endforeach; ?>
  </div>
  <?php endif; ?>

  <?php if ($workflow_sequences[$batch_record->workflow_sequence_id]['max_timestamp'] > $max_timestamp_and_how_long_ago->max_timestamp): ?>
  <div class="dashboard-report messages info">
    <h3>Workflow Sequence updated</h3>
      <p>Workflow Sequence has been updated AFTER action/s on this Item.
      The sequence was modified <?php print $workflow_sequences[$batch_record->workflow_sequence_id]['how_long_ago']; ?> on
      <?php print $workflow_sequences[$batch_record->workflow_sequence_id]['max_timestamp']; ?>
      while the most recent transaction for this item happened
      <?php print $max_timestamp_and_how_long_ago->how_long_ago; ?> on
      <?php print $max_timestamp_and_how_long_ago->max_timestamp; ?>.</p>
  </div>
  <?php endif; ?>

  <?php if ($ingested_links): ?>
  <h3>Object has been ingested</h3>
    <?php print $ingested_links; ?>
  <?php elseif ($can_ingest): ?>
  <div class="good"><p>All requirements are completed.
      <b>Ingest this item into Islandora now: <a href="/islandora/islandora_digital_workflow/ingest_item/<?php print urlencode($item->identifier); ?>"><?php print $item->identifier; ?></a></b>
  </div>
  <?php endif; ?>

  <h3>Item Details</h3>
  <div class="lookup_result oddrow">
      <form action="" method="POST" enctype="multipart/form-data">
        <fieldset class="lookup_result_indent evenrow"<?php print ($can_update) ? '' : ' disabled' ?>>
            <label for="edit-title">Title: </label>
            <input id="edit-title" name="title" value="<?php print htmlspecialchars($item->title); ?>">
            <label for="edit-identifier">Identifier: </label>
            <input id="edit-identifier" name="identifier" value="<?php print $item->identifier; ?>">
            <?php if ($is_paged_content): ?>
            <label for="edit-filename">Filename: </label>
            <input id="edit-filename" name="filename" value="<?php print $item->master_filename_basename; ?>">
            <?php endif; ?>
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
            <th>User</th>
            <th>When</th>
            <th>Timestamp</th>
        </tr>
      <?php foreach ($item_record_transactions as $transaction_record) { ?>
          <?php
          $toggle = !$toggle;
          ?>
          <tr class="<?php print ($toggle) ? 'evenrow' : 'oddrow'; ?> <?php print ($transaction_record->action_id > 999) ? 'batch_action' : 'item_action'; ?>"
              <?php if (isset($transaction_record->problem_notes)) : ?>
                <?php print ' title="' . $transaction_record->problem_notes . '"'; ?>
              <?php endif; ?>>
              <td>
                <div class="<?php print $transaction_record->glyph_class; ?>">&nbsp;</div>
                <?php print $transaction_record->description; ?>

                <?php if ($transaction_record->admin_links <> ''): ?>
                  <div class="admin_links">
                      <?php if ($transaction_record->transaction_id > 0): ?>
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
              <td><?php print $transaction_record->user_name; ?></td>
              <td><?php print $transaction_record->how_long_ago; ?></td>
              <td><?php print $transaction_record->timestamp; ?></td>
          </tr>
      <?php } ?>
    </table>
    <?php else: ?>
    <em>There are no item transactions yet for this batch item.</em>
    <?php endif; ?>
    <hr>
    <?php print $workflow_sequence_text; ?>

    <?php if (count($found_files) > 0) : ?>
    <?php $toggle = FALSE; ?>
    <h3>Files</h3>
      <table>
        <tr>
          <th>Filename</th>
          <th class="numeric">Size</th>
        </tr>
        <?php foreach ($found_files as $filename => $file_info) { ?>
            <?php $toggle = !$toggle; ?>
        <tr class="<?php print (($toggle) ? 'evenrow' : 'oddrow') .
            (($file_info['class'] <> '') ? ' ' . $file_info['class'] : ''); ?>">
          <td><?php print $filename; ?></td>
          <td class="numeric"><?php print number_format($file_info['filesize']); ?></td>
        </tr>
        <?php } ?>
      </table>
    <?php endif; ?>

  </div><!-- /end report_table "Item Transactions" -->

</div><!-- /end no-sidebars -->
