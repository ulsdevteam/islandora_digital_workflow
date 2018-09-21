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
    <?php $toggle = 0; ?>
    <?php foreach ($glyph_filenames as $glyph_index => $glyph_filename): ?>
    <?php if ($toggle < 1): ?>
    <tr>
    <?php endif; ?>
    <?php if ($glyph_filename): ?>
        <td class="padleft"><img src="/<?php print $module_path; ?>/<?php print $glyph_filename; ?>" width="<?php print (($normal_size) ? '20' : '40');?>" /> &nbsp;</td>
    <?php else: ?>
        <td class="padleft">N/A</td>
    <?php endif; ?>
    <?php if ($normal_size): ?>
        <td class="padright">
            &nbsp;<input type="radio" id="glyph_<?php print $glyph_index; ?>" name="glyph_selector" value="<?php print $glyph_filename;?>"<?php print ($selected <> $glyph_filename) ? '' : ' checked'; ?>>
            <label for="glyph_<?php print $glyph_index; ?>"><?php print $glyph_index;?></label>
        </td>
    <?php else: ?>
        <td class="padright"><?php print $glyph_filename;?></td>
    <?php endif; ?>
    <?php if ($toggle == 4): ?>
    </tr>
    <?php endif; ?>
    <?php
    $toggle++;
    if ($toggle == 4) { $toggle = 0; }
    ?>
    <?php endforeach; ?>
    <?php if ($toggle): ?>
    </tr>
    <?php endif; ?>
</table>
