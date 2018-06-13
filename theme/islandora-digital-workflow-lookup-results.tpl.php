<div class="lookup_results">
    <h3>Searched for "<?php print $searchterm; ?>"</h3>
    <p>Found <?php print $results_count; ?> results
      <?php if ($matched_csv_only) : ?> <i>(found only in uploaded CSV file)</i><?php endif; ?>.
    </p>

    <?php $toggle = FALSE; ?>
    <?php foreach ($batch_records as $batch_record) { ?>
    <div class="lookup_result <?php print ($toggle) ? 'evenrow' : 'oddrow'; ?>">
        <div class="lookup_result_indent">
          <b>Description:</b> <?php print $batch_record->batch_description; ?>
          <?php if (isset($batch_record->identifiers)) { ?>
          <div><b>Matched Identifiers:</b> <?php print $batch_record->identifiers; ?></div>
          <?php } ?>
        </div>
        <?php
        $toggle = !$toggle;
        if ($batch_record->nid) {
          $node = node_load($batch_record->nid);
          $view = node_view($node, 'teaser');
          print drupal_render($view);
        }
        ?>
    </div>
    <?php } ?>
<br style="clear:both" />