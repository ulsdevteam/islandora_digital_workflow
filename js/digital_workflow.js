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