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
    <?php if (count($tasks_tables) > 0): ?>
    <?php foreach ($tasks_tables as $task_table): ?>
    <div class="dashboard-task">
      <?php print $task_table; ?>
    </div>
    <?php endforeach; ?>
    <?php else: ?>
    <em class="good">There are no batches that currently have any batches needing attention.  If there were, they would be listed here.</em>
    <?php endif; ?>
</div>
<br style="clear:both" />
