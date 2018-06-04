<?php

/**
* @file
* islandora-digital-workflow-transactions-glyphs.tpl display template.
*
* Variables available:
* - $transaction_actions => array(),
*
*/
?>
<div class="transactions_glyphs">
    <?php foreach ($transaction_actions as $transaction_action) {
      $class_name = ($transaction_action) ? strtolower(str_replace(" ", "_", $transaction_action)) : 'spacer'; ?>
    <div class="transaction_action_<?php print $class_name;?>" title="<?php print $transaction_action; ?>">&nbsp;</div>
    <?php } ?>
</div>