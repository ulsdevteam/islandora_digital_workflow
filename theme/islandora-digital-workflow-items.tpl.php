<?php

/**
* @file
* islandora-digital-workflow-items.tpl display template.
*
* Variables available:
* - $batch_record => array(),
* - $transaction_records => array(),
* - $item_records => array(),
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
            <span class="<?php print (count($item_records) == $batch_record->object_count) ? 'good' : 'bad'; ?>">
            <b>How many physical objects:</b><?php print $batch_record->object_count; ?>
            <?php if (count($item_records) <> $batch_record->object_count) : ?>
            <em>(<a href="/node/<?php print $node->nid; ?>/batch"><?php print count($item_records); ?> objects expected</a>)</em>
            <?php endif; ?>
            </span>
        </div>
    </div>

    <h3>Batch Items</h3>
    <?php if (count($item_records) > 0) : ?>
    <table>
        <tr>
            <th>Identifier</th>
            <th>Title</th>
            <th>Filename</th>
            <th>Transactions</th>
        </tr>
      <?php foreach ($item_records as $item) { ?>
          <?php $toggle = !$toggle; ?>
          <tr class="<?php print ($toggle) ? 'evenrow' : 'oddrow'; ?>">
              <td><a href="/node/<?php print $node->nid; ?>/item/<?php print $item->batch_item_id; ?>"><?php print $item->identifier; ?></a></td>
              <td><?php print $item->title; ?></td>
              <td><?php print $item->filename_basename; ?></td>
              <td><?php print $item->transaction_actions; ?></td>
          </tr>
      <?php } ?>
    </table>
    <?php endif; ?>
  </div><!-- /end dashboard-report -->

</div><!-- /end no-sidebars -->