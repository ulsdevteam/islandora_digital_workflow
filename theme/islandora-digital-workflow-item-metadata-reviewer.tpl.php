<?php

/**
 * @file
 * islandora-digital-workflow-item-metadata-reviewer.tpl display template.
 *
 * Variables available:
 * - $metadata_reviewer HTML
 * - $timestamp string
 * - $long_ago string
 */
?>
<?php if ($metadata_reviewer): ?>
<br class="break_float" />
<label><?php print t('Metadata Completed'); ?></label>
<div><?php print ($metadata_reviewer); ?> <span class="small_lt_text"><?php print $long_ago . ' at ' . $timestamp; ?></span></div>
<?php endif; ?>