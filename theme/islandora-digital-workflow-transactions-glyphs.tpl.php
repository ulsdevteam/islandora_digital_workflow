<?php

/**
* @file
* islandora-digital-workflow-transactions-glyphs.tpl display template.
*
* Variables available:
* - $transaction_actions => array(),
* - $sequence_name => string,
* - $display_as_requirements => boolean,
*
*/
?>
<div class="transactions_glyphs" style="display:inline-block;float:left">
    <?php if ($display_as_requirements): ?>
    <div class="actions_required">
        <small><?php print $sequence_name; ?> item actions: </small>
    </div>
    <?php endif; ?>
    <?php foreach ($transaction_actions as $transaction_action) {
      // Since this set of actions can be used for a batch to represent required
      // actions AS WELL AS to display just the descriptions of the actions that 
      // are related to the batch / batch_item, this needs to inspect the variable
      $transaction_action_description = (is_array($transaction_action) &&
          array_key_exists('batch_action_description', $transaction_action)) ?
              $transaction_action['batch_action_description'] : $transaction_action;
      $required_class = (is_array($transaction_action) &&
          array_key_exists('is_required', $transaction_action)) ?
              ($transaction_action['is_required'] == 1 ? ' required_action' : ' optional_action') : '';
      $class_name = ($transaction_action_description) ? strtolower(str_replace(" ", "_", $transaction_action_description)) : 'spacer'; ?>
    <div class="transaction_action_<?php print $class_name . $required_class;?>" title="<?php print $transaction_action_description; ?>">&nbsp;</div>
    <?php } ?>
</div><br>