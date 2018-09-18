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
<table class="action_glyph_previews<?php print (($normal_size) ? '' : ' larger_glyph'); ?>">
    <?php $toggle = TRUE; ?>
    <?php foreach ($glyph_filenames as $glyph_index => $glyph_filename): ?>
    <?php if ($toggle): ?>
    <tr>
    <?php endif; ?>
    <?php if ($glyph_filename): ?>
        <td><img src="/<?php print $module_path; ?>/<?php print $glyph_filename; ?>" width="<?php print (($normal_size) ? '20' : '40');?>" /> &nbsp;</td>
    <?php else: ?>
        <td>N/A</td>
    <?php endif; ?>
    <?php if ($normal_size): ?>
        <td>
            &nbsp;<input type="radio" id="glyph_<?php print $glyph_index; ?>" name="glyph_selector" value="<?php print $glyph_filename;?>"<?php print ($selected <> $glyph_filename) ? '' : ' checked'; ?>>
            <label for="glyph_<?php print $glyph_index; ?>"><?php print $glyph_index;?></label>
        </td>
    <?php else: ?>
        <td><?php print $glyph_filename;?></td>
    <?php endif; ?>
    <?php if (!$toggle): ?>
    </tr>
    <?php endif; ?>
    <?php $toggle = !$toggle; ?>
    <?php endforeach; ?>
    <?php if (!$toggle): ?>
    </tr>
    <?php endif; ?>
</table>
