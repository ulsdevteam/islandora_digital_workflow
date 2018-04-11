/**
 * @file
 * Javascript file for islandora_digital_workflow admin_form
 * 
 */

(function ($) {

    $(document).ready(function() {
        // New
        $('#edit-stage-new-background-color').change(function() {
            $('#color_New').css({backgroundColor:  $('#edit-stage-new-background-color').val()});
        });
        $('#edit-stage-new-color').change(function() {
            $('#color_New').css({color:  $('#edit-stage-new-color').val()});
        });
        // Scanned
        $('#edit-stage-scanned-background-color').change(function() {
            $('#color_Scanned').css({backgroundColor:  $('#edit-stage-scanned-background-color').val()});
        });
        $('#edit-stage-scanned-color').change(function() {
            $('#color_Scanned').css({color:  $('#edit-stage-scanned-color').val()});
        });
        // Problem
        $('#edit-stage-problem-background-color').change(function() {
            $('#color_Problem').css({backgroundColor:  $('#edit-stage-problem-background-color').val()});
        });
        $('#edit-stage-problem-color').change(function() {
            $('#color_Problem').css({color:  $('#edit-stage-problem-color').val()});
        });
        // Reviewed
        $('#edit-stage-reviewed-background-color').change(function() {
            $('#color_Reviewed').css({backgroundColor:  $('#edit-stage-reviewed-background-color').val()});
        });
        $('#edit-stage-reviewed-color').change(function() {
            $('#color_Reviewed').css({color:  $('#edit-stage-reviewed-color').val()});
        });
        // Done
        $('#edit-stage-done-background-color').change(function() {
            $('#color_Done').css({backgroundColor:  $('#edit-stage-done-background-color').val()});
        });
        $('#edit-stage-done-color').change(function() {
            $('#color_Done').css({color:  $('#edit-stage-done-color').val()});
        });       
    });
  
})(jQuery);
