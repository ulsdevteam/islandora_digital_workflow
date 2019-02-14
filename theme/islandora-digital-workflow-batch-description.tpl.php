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
<?php if (is_array($batch_record)): ?>
<div class="lookup_result batch-description">
    <label>Batch name:</label> <a href="<?php print ($batch_record['nid']) ? '/node/' . $batch_record['nid'] . '/batch' : '/islandora/islandora_digital_workflow/edit_batch/' . $batch_record['batch_name']; ?>"><?php print $batch_record['batch_name']; ?></a><br >
    <label>Progress:</label>
          <?php foreach (array('New', 'In Progress', 'Prepared', 'Completed') as $progress_step) : ?>
          <span class="progress_<?php print (($batch_record['progress'] == $progress_step) ?
            strtolower(str_replace(" ", "_", $batch_record['progress'])) : "na"); ?>"><?php print $progress_step; ?></span>
          <?php print (($progress_step <> 'Completed') ? ' | ' : ''); ?>
      <?php endforeach; ?>
    <br>
    <label>Batch description:</label> <?php print $batch_record['batch_description']; ?>
    <?php if ($batch_mapped_from_webformsubmission) : ?>
    <br><label>Submission: </label> <a class="link_open_new_tab" title="link opens in separate tab" target="_blank" href="/node/<?php print $batch_mapped_from_webformsubmission->nid; ?>/submission/<?php print $batch_mapped_from_webformsubmission->sid; ?>">"<?php print $batch_mapped_from_webformsubmission->title; ?>" #<?php print $batch_mapped_from_webformsubmission->sid; ?></a>
    <?php endif; ?>
</div>
<?php endif; ?>