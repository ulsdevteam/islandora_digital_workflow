<?php

/**
 * @file
 * islandora-digital-workflow-batch-description.tpl display template.
 *
 * This will only create some output if there is a value for $batch_record and that
 * array has a value for $batch_record['nid'].
 *
 * Variables available:
 * - $batch_record => array(),
 * - $collection_review_links = array(),
 * - can_ingest_all => whether or not the entire batch can be ingested,
 * - can_publish_all => whether or not the entire batch can be published,
 * - batch_mapped_from_webformsubmission => webform submission object
 * - metadata_reviewer => HTML of the user who made the metadata and the
 * timestamp, but only appears until that item has passed metadata QC.
 */
?>
<?php if (array_key_exists('an_item_exists_in_other_batch', $_SESSION) && $_SESSION['an_item_exists_in_other_batch'] > 0): ?>
<div class="messages warning">
<p>At least one item in this batch exists in at least one other batch.  If those
item/s are ingested and published, they would replace the previous objects and
any workflow item/s records related to the identifier/s would be out of sync
with the state of the object/s.</div>
<?php endif; ?>
<?php if (is_array($batch_record)): ?>
<?php
  if ($batch_record['workflow_sequence_id'] < 1) : ?>
  <div class="bad messages error">This batch does not have a <b>
    <a href="/node/<?php print $batch_record['nid'] . '/batch';?>">Workflow Sequence</b>
    associated with it.</div>
<?php endif; ?>
<div class="lookup_result batch-description">
    <label>Batch name:</label> <a href="<?php print ($batch_record['nid']) ? '/node/' . $batch_record['nid'] . '/items' : '/islandora/islandora_digital_workflow/edit_batch/' . $batch_record['batch_name']; ?>"><?php print $batch_record['batch_name']; ?></a><br >
    <label>Batch progress:</label>
    <div class="progress_div">
          <?php foreach (array('New', 'In Progress', 'Prepared', 'Ingested', 'Completed') as $progress_step) : ?>
          <span class="progress_<?php print (($batch_record['progress'] == $progress_step) ?
            strtolower(str_replace(" ", "_", $batch_record['progress'])) : "na"); ?>"><?php print $progress_step; ?></span>
          <?php print (($progress_step <> 'Completed') ? ' &rsaquo; ' : ''); ?>
      <?php endforeach; ?>
    <?php if ($can_ingest_all && (arg(2) <> 'ingest_all')): ?> |
      <a href="/islandora/islandora_digital_workflow/ingest_all/<?php print $batch_record['batch_id'];?>">INGEST ALL ITEMS</a>
    <?php endif; ?>
    <?php if ($can_publish_all && (arg(2) <> 'publish_all')): ?> |
      <a href="/islandora/islandora_digital_workflow/publish_all/<?php print $batch_record['batch_id'];?>">PUBLISH ALL ITEMS</a>
    <?php endif; ?>
    </div>
    <br class="break_float">
    <?php if ($collection_review_links && is_array($collection_review_links) && count($collection_review_links) > 0) : ?>
    <div class="progress_div">
        <b><?php print (variable_get('islandora_digital_workflow_use_reviewCollections', 0) ? 'Review ' : ''); ?>Collection/s:</b>
        <ul><li><?php print implode("</li><li>", $collection_review_links); ?></li><ul>
    </div>
    <br class="break_float">
    <?php endif; ?>
    <label>Batch description:</label> <div><?php print $batch_record['batch_description']; ?></div>
    <?php if ($batch_mapped_from_webformsubmission) : ?>
    <br><label>Submission: </label> <a class="link_open_new_tab" title="link opens in separate tab" target="_blank" href="/node/<?php print $batch_mapped_from_webformsubmission->nid; ?>/submission/<?php print $batch_mapped_from_webformsubmission->sid; ?>">"<?php print $batch_mapped_from_webformsubmission->title; ?>" #<?php print $batch_mapped_from_webformsubmission->sid; ?></a>
    <?php endif; ?>
    <label>Scanning specifications:</label> <div><?php print ($batch_record['scan_specs'] ? $batch_record['scan_specs'] : '&nbsp;'); ?></div>
    <?php print $metadata_reviewer; ?>
</div>
<?php endif; ?>