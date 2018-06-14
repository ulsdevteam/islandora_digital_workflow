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
    <h3>Links</h3>
    <ul class="action-links"><li><a href="/islandora/islandora_digital_workflow/create_batch">Create Batch</a></li></ul>
    <ul>
    <?php foreach ($links as $link) { ?>
        <li><?php print $link; ?></li>
    <?php } ?>
    </ul>
  </div><!-- /end dashboard-report -->

</div><!-- /end no-sidebars -->
