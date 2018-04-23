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
<div id="two-col-left-main"> 
  <div id="main-content"> 
    <div class="dashboard-report">
      <h3>TESTING</h3>
        <a href="/islandora/islandora_digital_workflow/create_batch">Create Batch</a>
      <ul>
          <li><a href="/islandora/islandora_digital_workflow/batches/all">All Batches</a></li>
          <li><a href="/islandora/islandora_digital_workflow/batches/problem">Problems</a></li>
          <li><a href="/islandora/islandora_digital_workflow/batches/new">New</a></li>
          <li><a href="/islandora/islandora_digital_workflow/batches/scanned">Scanned</a></li>
          <li><a href="/islandora/islandora_digital_workflow/batches/reviewed">Reviewed</a></li>
          <li><a href="/islandora/islandora_digital_workflow/batches/done">Done</a></li>
      </ul>
    </div>

  </div><!-- /end main-content -->
  <?php if ((isset($page['sidebar'])) || (isset($metadata))): ?>
  <div id="sidebar"> 
    <?php if (isset($page['sidebar'])) : ?>
      <?php print render($page['sidebar']); ?>
    <?php endif; ?>

    <?php if (isset($metadata)) : ?>
      <?php print $metadata; ?>
    <?php endif; ?>
  </div><!-- /end sidebar -->
  <?php endif; ?>

</div><!-- /end two-col-left-main -->
