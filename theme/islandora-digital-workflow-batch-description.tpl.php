<?php

/**
 * @file
 * islandora-digital-workflow-batch-description.tpl display template.
 *
 * This will only create some output if there is a value for $description.
 *
 * Variables available:
 * - $description
 */
?>
<?php if ($description): ?>
<div class="batch-description">
<?php print $description; ?>
</div>
<?php endif; ?>