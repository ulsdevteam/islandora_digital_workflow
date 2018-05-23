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
  <div class="transactions-report">
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

    <?php foreach ($transaction_records as $transaction_record) { ?>
    <div class="lookup_result <?php print ($toggle) ? 'evenrow' : 'oddrow'; ?>">
        <?php
        $toggle = !$toggle;
        ?>
        <div class="lookup_result_indent">
          <pre><?php echo print_r($transaction_record, true); ?></pre>
        </div>
    </div>
    <?php } ?>
  </div><!-- /end dashboard-report -->

</div><!-- /end no-sidebars -->