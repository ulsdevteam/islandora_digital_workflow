<?php

/**
* @file
* islandora-digital-workflow-process-set-display.tpl display template.
*
* Variables available:
* - $process_set_id
* - $items array of identifiers
* - $title
*/
?>
<?php if (count($items) > 0) : ?>
    <div class="process_set">
        <h3><?php print $title; ?> (<?php print number_format(count($items)); ?>)</h3>
        <?php print l('Process this set', 'islandora/islandora_digital_workflow/utilities/process_set/' .
    $process_set_id, array('attributes'=>array(
      'title' => 'link opens in separate tab',
      'class' => array('link_open_new_tab_tiny'),
      'target' => '_blank'))); ?>
        <textarea rows="6" readonly class="short-text-report"><?php print implode("\n", $items); ?></textarea>
    </div>
<?php endif; ?>