<?php

/**
 * @file
 * islandora-digital-workflow-batch-defaults.tpl display template.
 *
 * This template will render all of the fields that are in a given batch record
 * in a way that can be human readable.  This is only used to set the nodes'
 * body value for workflow_batch type nodes.
 *
 * Variables available:
 * - $batch_record array
 * - $schema array
 */
?>

<?php
  if (!$batch_record['workflow_sequence_id']) : ?>
  <div class="messages error">This batch does not have a <b>
    <a href="node/<?php print $batch_record['nid'] . '/batch';?>">Workflow Sequence</b>
    associated with it.  Any actions for the items below would not display until
    that sequence is configured.</div>
<?php endif; ?>
<div id="islandora-digital-workflow-batch-defaults">
  <h3>Batch Defaults</h3>
  <?php if (is_array($batch_record)) { ?>
  Edit Batch: "<?php print l($batch_record['batch_name'],
        'islandora/islandora_digital_workflow/edit_batch/' . $batch_record['batch_name']); ?>"<hr>
  <?php
    $string_fields = array('varchar' => 'varchar', 'text' => 'text');
    foreach ($batch_record as $field => $value) {
      $quote_char = (array_key_exists('type', $schema['islandora_digital_workflow_batch']['fields'][$field]) ?
        (array_key_exists($schema['islandora_digital_workflow_batch']['fields'][$field]['type'], $string_fields) ?
              '"' : '') : '');
      $description = (array_key_exists('description', $schema['islandora_digital_workflow_batch']['fields'][$field]) ?
        $schema['islandora_digital_workflow_batch']['fields'][$field]['description']: $field);
      $type = (array_key_exists('type', $schema['islandora_digital_workflow_batch']['fields'][$field]) ?
        $schema['islandora_digital_workflow_batch']['fields'][$field]['type']: 'varchar');
      $span_id = "islandora-digital-workflow-" . $field; ?>
        <div><label title="field name &quot;<?php print $field; ?>&quot;" for='<?php print $span_id; ?>'><?php print $description; ?></label><span class="idw_field_type_<?php print  $type; ?>" id='<?php print $span_id; ?>'><?php print $quote_char; ?><?php
      $truncated_ellipsis = (strlen($value) > 180) ? ' ... <i>(continued)</i> ' : '';
      print substr($value, 0, 180) . $truncated_ellipsis;
    ?><?php print $quote_char; ?></span></div>
    <?php }
  } ?>
</div><!-- /end islandora-digital-workflow-batch-defaults -->
