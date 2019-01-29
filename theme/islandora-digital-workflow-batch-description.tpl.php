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
    <label>Progress:</label> <span class="progress_<?php print strtolower(str_replace(" ", "_", $batch_record['progress'])); ?>"><?php print $batch_record['progress']; ?></span><br>
    <label>Batch description:</label> <?php print $batch_record['batch_description']; ?>
    <?php if ($batch_mapped_from_webformsubmission) : ?>
    <br><label>Submission: </label> <a target="_blank" href="/node/<?php print $batch_mapped_from_webformsubmission->nid; ?>/submission/<?php print $batch_mapped_from_webformsubmission->sid; ?>">"<?php print $batch_mapped_from_webformsubmission->title; ?>" #<?php print $batch_mapped_from_webformsubmission->sid; ?></a>
    <?php endif; ?>
</div>
<?php endif; ?>