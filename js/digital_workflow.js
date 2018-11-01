/**
 * @file
 * Javascript file for islandora_digital_workflow
 */


/*
 * This will hide / show the "notes" form element depending on whether or not 
 * the current selection is an action that requires notes.  Currently, this is
 * only for IDW_ACTION_PROBLEM.
 * 
 * @param {type} control
 * @returns {undefined}
 */
function add_action_selection(control) {
  // IDW_ACTION_PROBLEM == 9
  if (control.value == 9) {
    jQuery('#edit-notes-wrapper').show();
    jQuery('#edit-notes-wrapper').removeClass("display_none");
  }
  else {
    jQuery('#edit-notes-wrapper').hide();
    jQuery('#edit-notes-wrapper').addClass("display_none");
  }
}

function stage_selection(control, element_name) {
  if (control.value > 0) {
    jQuery('#edit-' + element_name).removeAttr("disabled");
    jQuery('.form-item-' + element_name + ' label').addClass("enabled_text");
    jQuery('.form-item-' + element_name + ' label').removeClass("disabled_text");
  }
  else {
    jQuery('#edit-' + element_name).attr('disabled', 'disabled');
    jQuery('.form-item-' + element_name + ' label').removeClass("enabled_text");
    jQuery('.form-item-' + element_name + ' label').addClass("disabled_text");
  }
}

function processing_mode_selected(control) {
    if ((control.value === 'gen_notimelimit_OCR') || (control.value === 'gen_OCR')) {
        jQuery('#edit-language-options').show();
        jQuery('#edit-language-options').removeClass("display_none");
    }
    else {
        jQuery('#edit-language-options').hide();
        jQuery('#edit-language-options').addClass("display_none");
    }
}