<?php

/**
* @file
* islandora-digital-workflow-dynamic-css.tpl display template.
*
* Variables available:
* - $batch_record => array(),
* - $item_file_records => array(),
*
*/
?>
<?php foreach ($all_actions as $action_arr): ?>
<?php if ($action_arr['glyph']) : ?>
.<?php print $action_arr['class_name']; ?> {
  background: url("/<?php print $module_path; ?>/<?php print $action_arr['glyph']; ?>") 0px 0px no-repeat !important;
}
<?php endif; ?>
<?php endforeach; ?>

<?php $div_classnames = array(); ?>
<?php foreach ($all_actions as $action_arr): ?>
<?php if ($action_arr['glyph']) {
  $div_classnames[] = 'div.' . $action_arr['class_name'];
} ?>
<?php endforeach; ?>

<?php print implode(", ", $div_classnames); ?> {
  width: 20px;
  height: 20px;
  float: left;
  padding: 0px 1px;
  background-position: top left;
}