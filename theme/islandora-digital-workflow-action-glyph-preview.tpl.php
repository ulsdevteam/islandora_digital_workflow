<?php

/**
* @file
* islandora-digital-workflow-action-glyph-preview.tpl display template.
*
* Variables available:
* - $glyph_filenames => array(),
* - $module_path => string the path to the current module.
*/
?>
<div class="action_glyph_previews<?php print (($normal_size) ? '' : ' larger_glyph'); ?>">
    <?php foreach ($glyph_filenames as $glyph_filename): ?>
    <?php if ($glyph_filename): ?>
    <img src="/<?php print $module_path; ?>/<?php print $glyph_filename; ?>" width="<?php print (($normal_size) ? '20' : '20');?>" />
    <?php else: ?>
    N/A
    <?php endif; ?>
    <br />
    <?php endforeach; ?>
</div>
<br class="clearfix" />