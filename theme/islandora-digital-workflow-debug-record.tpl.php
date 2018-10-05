<?php

/**
 * @file
 * islandora-digital-workflow-markup.tpl display template.
 *
 * Variables available:
 * - $debug_record array()
 */
?>
<?php $output_saved_for_bottom = ''; ?>
<?php foreach ($debug_record as $fieldname => $value): ?>
<?php if ($fieldname == 'output') :
  $output_saved_for_bottom = $value;
else: ?>
<div>
    <label class="fixed_label_width"><?php print $fieldname; ?></label><?php print $value; ?>
</div>
<?php endif; ?>
<?php endforeach; ?>
<?php if ($output_saved_for_bottom): ?>
  <div><label class="fixed_label_width"><?php print $fieldname; ?></label></div>
  <textarea readonly class="as_terminal_output textarea_terminal"><?php print unserialize($output_saved_for_bottom); ?></textarea>
<?php endif; ?>
