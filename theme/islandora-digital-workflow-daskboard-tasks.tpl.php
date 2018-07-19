<?php

/**
* @file
* islandora-digital-workflow-dashboard-tasks.tpl display template.
*
* Variables available:
* - $tasks_tables => array(),
*
*/
?>
<div class="dashboard-tasks">
    <h3>The issues with the following batches need to be resolved:</h3>
    <?php foreach ($tasks_tables as $task_table): ?>
    <div class="dashboard-task">
      <?php print $task_table; ?>
    </div>
    <?php endforeach; ?>
</div>
<br style="clear:both" />
