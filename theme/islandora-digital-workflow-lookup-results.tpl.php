<?php

/**
* @file
* islandora-digital-workflow-lookup-results.tpl display template.
*
* Variables available:
* - $searchterm = string
* - $batch_records => array(),
* - $results_count => integer
* - $matched_csv_only => boolean
*
*/
?>
<div class="lookup_results">
    <h3>Searched for "<?php print $searchterm; ?>"</h3>
    <p>Found <?php print $results_count; ?> results<?php
    if ($matched_csv_only) : ?> <i>(found only in uploaded CSV file)</i><?php endif; ?></p>

    <?php $toggle = FALSE; ?>
    <?php foreach ($batch_records as $batch_record) { ?>
    <div class="lookup_result <?php print ($toggle) ? 'evenrow' : 'oddrow'; ?>">
        <span class="small_lt_text">Match found in: <?php print implode(', ', $batch_record->reasons); ?></span>
        <?php
        $toggle = !$toggle;
        if ($batch_record->nid) {
          $node = node_load($batch_record->nid);
          $view = node_view($node, 'teaser');
          print drupal_render($view);
        }
        ?>
        <div class="lookup_result_indent">
        <?php
        if (!($batch_record->nid)) : ?>
          <h2><?php print l($batch_record->batch_name, '/islandora/islandora_digital_workflow/edit_batch/' . $batch_record->batch_name); ?></h2>
        <?php endif; ?>
          <b>Description:</b> <?php print $batch_record->batch_description; ?>
          <?php if (isset($batch_record->identifiers)) { ?>
          <div><b>Matched Identifiers:</b> <?php print $batch_record->identifiers; ?></div>
          <?php } ?>
        </div>
    </div>
    <?php } ?>
<br style="clear:both" />
