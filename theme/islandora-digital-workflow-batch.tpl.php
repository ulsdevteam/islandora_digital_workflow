<?php
/**
        * @file
        * ######################### display template.
        *
        * Variables available:
        *
        * @see template_preprocess_###########################
        */
?>

<div id="islandora-digital-workflow-batch">
  <h3>Batch Defaults</h3>
  <?php if (is_array($islandora_digital_workflow_batch)) {
    foreach ($islandora_digital_workflow_batch as $field => $value) {
      $span_id = "islandora-digital-workflow-" . $field;
      print "<div><label for='" . $span_id . "'>" . $field . "</label> <span id='" . $span_id . "'>" . $value . "</span></div>";
    }
  } ?>
</div><!-- /end islandora-digital-workflow-batch -->
