<?php

/**
 * @file
 * islandora-digital-workflow-batch-description.tpl display template.
 *
 * This will only create some output if there is a value for $batch_record and that
 * array has a value for $batch_record['nid'].
 *
 * Variables available:
 * - $batch_record
 */
?>
<?php if ($batch_record && array_key_exists('nid', $batch_record) && $batch_record['nid']): ?>
<div class="lookup_result batch-description">
    <label>Batch name:</label> <a href="/node/<?php print $batch_record['nid']; ?>/batch"><?php print $batch_record['batch_name']; ?></a><br >
    <label>Batch description:</label> <?php print $batch_record['batch_description']; ?>
</div>
<?php endif; ?>