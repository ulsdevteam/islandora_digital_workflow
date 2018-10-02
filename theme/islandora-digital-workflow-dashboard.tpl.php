<?php

/**
* @file
* islandora-digital-workflow-dashboard.tpl display template.
*
* Variables available:
* - $dashboard_data:
*
*/
?>
<div id="no-sidebars">
  <div class="dashboard-report">
    <ul class="action-links"><li><?php print implode("</li><li>", $action_links); ?></li></ul>
    <?php foreach ($links as $section => $section_links): ?>
    <?php if ($section == "batch_stages"): ?><div class="batches-links"><?php endif; ?>
    <ul id="link_section_<?php print $section; ?>" class="<?php print $section; ?>-links">
    <?php if ($section == "batch_stages"): ?><li class="list_header">Filter by <b>Workflow Stage</b></li><?php endif; ?>
    <?php if ($section == "batch_priorities"): ?><li class="list_header">Batches by <b>Priority</b></li><?php endif; ?>
    <?php if ($section == "batch_content_types"): ?><li class="list_header">Batches by <b>Content Type</b></li><?php endif; ?>
      <?php foreach ($section_links as $link) { ?>
          <li><?php print $link; ?></li>
      <?php } ?>
    </ul>
    <?php if ($section == "batch_content_types"): ?></div><?php endif; ?>
    <?php endforeach; ?>
    <br style="clear: both">
  </div><!-- /end dashboard-report -->

</div><!-- /end no-sidebars -->
