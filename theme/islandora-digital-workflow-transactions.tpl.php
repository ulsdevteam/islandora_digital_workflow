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
          <b>Description:</b> <?php print $batch_record->batch_description; ?>
        </div>
    </div>

    <?php if (count($transaction_records) > 0) : ?>
    <table>
        <tr>
            <th>Description</th>
            <th>When</th>
            <th>User</th>
            <th>Timestamp</th>
        </tr>
      <?php foreach ($transaction_records as $transaction_record) { ?>
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
              <td><?php print $transaction_record->user_name; ?></td>
              <td><?php print $transaction_record->timestamp; ?></td>
          </tr>
      <?php } ?>
    </table>
    <?php endif; ?>
  </div><!-- /end dashboard-report -->

</div><!-- /end no-sidebars -->