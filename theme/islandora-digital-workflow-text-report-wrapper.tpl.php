<?php

/**
* @file
* islandora-digital-workflow-text-report-wrapper.tpl display template.
*
* Variables available:
* - $inner_title => string,
* - $contents => string,
* - $footer => string,
* - $timestamps (array with 'start' and 'done' key values)
*/
?>
<div class="text-report">
  <?php if ($inner_title): ?>
    <h3><?php print $inner_title; ?></h3>
  <?php endif; ?>
  <?php if (array_key_exists('start', $timestamps) && (array_key_exists('done', $timestamps))): ?>
    <b class="disabled_text">
        Started <?php print $timestamps['start']; ?> --
        Finished <?php print $timestamps['done']; ?>
    </b>
  <?php endif; ?>

  <?php print $contents; ?>

  <?php if ($footer): ?>
    <?php print $footer; ?>
  <?php endif; ?>
</div>
