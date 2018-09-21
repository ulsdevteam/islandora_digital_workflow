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

function stage_selection(control) {
  if (control.value > 0) {
    jQuery('#edit-is-triggered-by-single-item').removeAttr("disabled");
    jQuery('.form-item-is-triggered-by-single-item label').addClass("enabled_text");
    jQuery('.form-item-is-triggered-by-single-item label').removeClass("disabled_text");
  }
  else {
    jQuery('#edit-is-triggered-by-single-item').attr('disabled', 'disabled');
    jQuery('.form-item-is-triggered-by-single-item label').removeClass("enabled_text");
    jQuery('.form-item-is-triggered-by-single-item label').addClass("disabled_text");
  }
}
