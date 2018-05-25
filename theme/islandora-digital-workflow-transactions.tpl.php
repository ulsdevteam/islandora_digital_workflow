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
            <th>Timestamp</th>
        </tr>
      <?php foreach ($transaction_records as $transaction_record) { ?>
          <?php
          $toggle = !$toggle;
          ?>
          <tr class="<?php print ($toggle) ? 'evenrow' : 'oddrow'; ?>">
              <td><?php print $transaction_record->description; ?></td>
              <td><?php print $transaction_record->how_long_ago; ?></td>
              <td><?php print $transaction_record->timestamp; ?></td>
          </tr>
      <?php } ?>
    </table>
    <?php endif; ?>
  </div><!-- /end dashboard-report -->

</div><!-- /end no-sidebars -->