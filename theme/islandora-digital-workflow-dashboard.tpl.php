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
    <ul id="link_section_<?php print $section; ?>" class="<?php print $section; ?>-links">
      <?php foreach ($section_links as $link) { ?>
          <li><?php print $link; ?></li>
      <?php } ?>
    </ul>
    <?php endforeach; ?>
    <br style="clear: both">
  </div><!-- /end dashboard-report -->

</div><!-- /end no-sidebars -->
