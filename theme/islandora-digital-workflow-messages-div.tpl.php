<?php

/**
* @file
* islandora-digital-workflow-messages-div.tpl display template.
*
* Variables available:
* - $message_type string the status class for the div
* - $title string (optional)
* - $message string, the HTML contents for the box
*/
?>
<div class="messages <?php print $message_type; ?>"><?php if ($title) : ?><h3><?php print $title; ?></h3><?php endif; ?>
    <?php print $message; ?>
</div>