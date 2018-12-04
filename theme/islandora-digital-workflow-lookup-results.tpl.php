<?php

/**
* @file
* islandora-digital-workflow-lookup-results.tpl display template.
*
* Variables available:
* - $searchterm = string
* - $records => array(),
* - $results_count => integer
* - $matched_csv_only => boolean
*
*/
?>
<div class="lookup_results">
    <h3>Searched for "<?php print $searchterm; ?>"</h3>
    <p>Found <?php print $results_count; ?> results<?php
    if ($matched_csv_only) : ?> <i>(found only in uploaded CSV file)</i><?php endif; ?></p>

    <?php $digital_request_heading_displayed = FALSE; ?>
    <?php
      $toggle = FALSE;
      $last_webform_title = '';
    ?>
    <?php foreach ($records as $record) { ?>
      <?php $is_digitization_request = (!(array_search('digitization requests', $record->reasons) === FALSE)); ?>
        <?php if (!$digital_request_heading_displayed && (!(array_search('digitization requests', $record->reasons) === FALSE))): ?>
          <h3>Digitization Requests</h3>
          <?php $digital_request_heading_displayed = TRUE; ?>
        <?php endif; ?>
        <?php // sorry for the inline PHP block using a slightly different syntax.
        if ($is_digitization_request && $last_webform_title <> $record->webform_title) {
          print "Submission/s to <b>" . $record->webform_title . "</b><br>";
          $last_webform_title = $record->webform_title;
        } ?>

    <div class="lookup_result <?php print ($toggle) ? 'evenrow' : 'oddrow'; ?>">
        <?php
        $toggle = !$toggle;
        if ($record->nid && (array_search('digitization requests', $record->reasons) === FALSE)) {
          $node = node_load($record->nid);
          $view = node_view($node, 'teaser');
          print drupal_render($view);
        }
        ?>
        <div class="lookup_result_indent">
        <?php if ($is_digitization_request) : ?>
          <b>Request data:</b> <?php print l($record->data, '/node/' . $record->nid . '/submission/' . $record->sid); ?>
        <?php else: ?>
          <?php if (!($record->nid)) : ?>
            <h2><?php print l($record->batch_name, '/islandora/islandora_digital_workflow/edit_batch/' . $record->batch_name); ?></h2>
          <?php endif; ?>
          <b>Description:</b> <?php print $record->batch_description; ?>
          <?php if (isset($record->identifiers)) { ?>
          <div><b>Matched Identifiers:</b> <?php print $record->identifiers; ?></div>
          <?php } ?>
          <?php endif; ?>
        </div>
        <span class="small_lt_text">Match found in: <?php print implode(', ', $record->reasons); ?></span>
    </div>
    <?php } ?>
<br style="clear:both" />
</div>
