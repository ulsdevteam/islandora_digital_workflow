<?php

/**
* @file
* islandora-digital-workflow-item.tpl display template.
*
* Variables available:
* - $batch_record => array(),
* - $description_markup = '',
* - $item => stdObject,
* - $transaction_records => array(),
* - $drush_log_entries => array(),
* - $item_record_transactions => array(),
* - $found_files => array(),
* - $found_delivery_files => array(),
* - $resource_select_box - prerendered HTML for a <select ... > input box.
* - $islandora_model_select_box - prerendered HTML for a <select ... > input box.
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
    <p class="disabled_text">Aside from needing to satisfy all of the actions in
        the workflow sequence, problems must be cleared before the item or the
        batch can be ingested.  To clear a problem, place the repaired files in
        the <b>Incoming Delivery Directory</b> location;  these files can then be
        imported back into the system which will resolve the specific item's problem record.</p>
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

  <?php if ($max_timestamp_and_how_long_ago->max_timestamp &&
      ($workflow_sequences[$batch_record['workflow_sequence_id']]['max_timestamp'] > $max_timestamp_and_how_long_ago->max_timestamp)): ?>
  <div class="dashboard-report messages message_info">
    <h3>Workflow Sequence updated</h3>
      <p>Workflow Sequence has been updated AFTER action/s on this Item.
      The sequence was modified <?php print $workflow_sequences[$batch_record['workflow_sequence_id']]['how_long_ago']; ?> on
      <?php print $workflow_sequences[$batch_record['workflow_sequence_id']]['max_timestamp']; ?>
      while the most recent transaction for this item happened
      <?php print $max_timestamp_and_how_long_ago->how_long_ago; ?> on
      <?php print $max_timestamp_and_how_long_ago->max_timestamp; ?>.</p>
  </div>
  <?php endif; ?>

  <?php if ($ingested_links): ?>
  <div class="messages message_info">
    <h3>Object has been ingested</h3>
      <?php print $ingested_links; ?>
  </div>
  <?php endif; ?>

  <?php if ($can_ingest): ?>
    <?php if ($ingested_links) : ?>
    <div class="messages warning">
      <div class="bad"><p><b>All requirements are completed.  This object has already been ingested.</b></p>
        <p><i>You will need to manually delete the object and any islandora_batch queue records before this can ingest again.</i><br>
        Ingest this item into Islandora again: <a href="/islandora/islandora_digital_workflow/ingest_item/<?php print urlencode($item->batch_item_id); ?>"><?php print $item->identifier; ?></a>.
    <?php else: ?>
    <div class="messages message_info">
      <div class="good"><p><b>All requirements are completed.</b></p><p>Ingest this item into Islandora now:
        <a href="/islandora/islandora_digital_workflow/ingest_item/<?php print urlencode($item->batch_item_id); ?>"><?php print $item->identifier; ?></a>.
    <?php endif; ?>
    </div>
  </div>
  <?php endif; ?>

  <?php if ($description_markup): ?>
    <?php print $description_markup; ?>
  <?php endif; ?>

  <h3>Item Details</h3>
  <div class="lookup_result oddrow">
      <form action="" method="POST" enctype="multipart/form-data">
          <input name="batch_item_id" type="hidden" value="<?php print $item->batch_item_id; ?>">
        <fieldset class="lookup_result_indent evenrow"<?php print ($can_update) ? '' : ' disabled' ?>>
            <label for="edit-title">Title: </label>
            <input id="edit-title" name="title" value="<?php print htmlspecialchars($item->title); ?>">
            <label for="edit-identifier">Identifier: </label>
            <input id="edit-identifier" name="identifier" value="<?php print $item->identifier; ?>">
            <?php if (!$is_paged_content): ?>
            <label for="edit-filename">Filename: </label>
            <input id="edit-filename" name="filename" value="<?php print $item->master_filename_basename; ?>">
            <?php endif; ?>
            <label for="edit-assigned-pid">Assign Fedora PID: </label>
            <?php if (variable_get('islandora_digital_workflow_assign_PIDs', 0) == 1): ?>
            <input id="edit-assigned-pid" name="assigned_pid" value="<?php print (($item->assigned_pid) ? $item->assigned_pid : $pid); ?>">
            <?php else: ?>
            <input id="edit-assigned-pid" class="disabled_text" disabled readonly="readonly" name="disabled_assigned_pid" value="The option to assign PID values is disabled in the module configuration.">
            <?php endif; ?>
            <label for="edit-type-of-resource">Type of resource: </label>
            <?php print $resource_select_box; ?>
            <label for="edit-islandora-model">Islandora model: </label>
            <?php print $islandora_model_select_box; ?>
            <label for="edit-mods">MODS: </label>
            <textarea rows=5 class="<?php
            print (user_access(ISLANDORA_DIGITAL_WORKFLOW_EDIT_ITEM_META) ? '': 'disabled_text '); ?>short-text-area" id="edit-mods" name="mods"<?php
            print (user_access(ISLANDORA_DIGITAL_WORKFLOW_EDIT_ITEM_META) ? '': ' readonly="readonly"'); ?>><?php print $item->mods; ?></textarea>
            <span class="small_font disabled_text"><em>NOTE:</em> Editing the MODS here may not be the right thing to do depending on the source of the metadata.</span>
            <?php if ($is_paged_content) : ?>
            <div>
                <label for="edit-marc-file">MARC record:</label><br>
                <input type="file" id="edit-marc-file" name="marc_file" />
            </div>
            <textarea rows=5 class="<?php
            print (user_access(ISLANDORA_DIGITAL_WORKFLOW_EDIT_ITEM_META) ? '': 'disabled_text '); ?>short-text-area" id="edit-marc" name="marc"<?php
            print (user_access(ISLANDORA_DIGITAL_WORKFLOW_EDIT_ITEM_META) ? '': ' readonly="readonly"'); ?>><?php print $item->marc; ?></textarea>
            <?php endif; ?>
        </fieldset>
      <?php if ($can_update) : ?>
      <input type="submit" value="Update Batch Item">
      <?php endif; ?>
      <?php if (user_access(ISLANDORA_DIGITAL_WORKFLOW_DELETE_ITEMS)) : ?>
       &nbsp; <input type="submit" name="deletebatchitem" class="bad" value="Delete Batch Item">
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
            <th>Date</th>
        </tr>
      <?php foreach ($item_record_transactions as $transaction_record) { ?>
          <?php
          $toggle = !$toggle;
          ?>
          <tr class="<?php print ($toggle) ? 'evenrow' : 'oddrow'; ?> <?php print (isset($transaction_record->is_batch_action) && $transaction_record->is_batch_action) ? 'batch_action' : 'item_action'; ?>"
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

    <?php if ((count($found_files) > 0) || count($found_delivery_files) > 0) : ?>
    <?php $toggle = FALSE;  $total_size = 0; ?>
    <h3>Working Files</h3>
      <small class="small_lt_text"><b>Files location:</b> <?php print $working_directory; ?></small>
      <div class="text-report scroll-v-200">
        <table>
          <tr>
            <th>Filename</th>
            <th class="numeric">Size (bytes)</th>
          </tr>
          <?php foreach ($found_files as $filename => $file_info) { ?>
            <?php $toggle = !$toggle; ?>
            <?php $total_size += $file_info['filesize']; ?>
          <tr class="<?php print (($toggle) ? 'evenrow' : 'oddrow') .
              (($file_info['class'] <> '') ? ' ' . $file_info['class'] : ''); ?>">
            <td><?php print $filename; ?></td>
            <td class="numeric"><?php print number_format($file_info['filesize']); ?></td>
          </tr>
          <?php } ?>
        </table>
      </div>
      <div class="right-total"><span><b>Total: </b><?php print number_format($total_size); ?> bytes</span></div>

    <?php $toggle = FALSE;  $total_size = 0; ?>
    <h3>Delivery Files</h3>
      <small class="small_lt_text"><b>Files location:</b> <?php print $delivery_directory; ?></small>
      <div class="text-report scroll-v-200">
        <table>
          <tr>
            <th>Filename</th>
            <th class="numeric">Size (bytes)</th>
          </tr>
          <?php foreach ($found_delivery_files as $filename => $file_info) { ?>
            <?php $toggle = !$toggle; ?>
            <?php $total_size += $file_info['filesize']; ?>

          <tr <?php if (strstr($file_info['class'], 'bad_sequence')) {
            print 'title="Filename sequence bad and will be renamed when brought into the working files location" ';
          } ?>class="<?php print (($toggle) ? 'evenrow' : 'oddrow') .
              (($file_info['class'] <> '') ? ' ' . $file_info['class'] : ''); ?>">
            <td><?php print $filename; ?></td>
            <td class="numeric"><?php print number_format($file_info['filesize']); ?></td>
          </tr>
          <?php } ?>
        </table>
      </div>
      <div class="right-total"><span><b>Total: </b><?php print number_format($total_size); ?> bytes</span></div>

    <?php endif; ?>

  </div><!-- /end report_table "Item Transactions" -->

  <?php if (user_access(ISLANDORA_DIGITAL_WORKFLOW_VIEW_EXTRA_INFO) && (count($drush_log_entries) > 0)): ?>
    <?php $toggle = FALSE; ?>
    <h3>Ingest Commands</h3>
    <p>Return Code values that are non-zero are considered an error.  All other rows are assumed to have succeeded in being processed and handled by the shell.</p>
  <table class="report">
      <thead>
          <th>Drush Command</th>
          <th>User</th>
          <th>Date</th>
          <th>Return Code</th>
          <th width="300">Output</th>
      </thead>
      <tbody>
    <?php foreach ($drush_log_entries as $drush_log_entry): ?>
    <?php $toggle = !$toggle; ?>
      <tr class="<?php print (($drush_log_entry->return_val) ? 'bad' : 'good') . ' ' . (($toggle) ? 'evenrow' : 'oddrow'); ?>">
          <td><?php print $drush_log_entry->drush_command; ?></td>
          <td><?php print $drush_log_entry->user_link; ?></td>
          <td><?php print $drush_log_entry->timestamp; ?></td>
          <td><?php print $drush_log_entry->return_val; ?></td>
          <td><div class="as_terminal_output"><?php foreach (unserialize($drush_log_entry->output) as $output_line): ?><?php print $output_line . "\n"; ?><?php
          endforeach; ?></div><div><?php
            print l('View fullscreen', 'islandora/islandora_digital_workflow/debug_view/drush_log_id/' . $drush_log_entry->drush_log_id,
                    array('attributes' => array('class' => array('small_right_float'))));
            ?></div></td>
      </tr>
    <?php endforeach; ?>
      </tbody>
  </table>
  <?php endif; ?>

</div><!-- /end no-sidebars -->