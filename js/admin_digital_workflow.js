/**
 * @file
 * Javascript file for the settings interface islandora_digital_workflow
 */


jQuery( document ).ready(function() {
    jQuery(".odd_child").parents('div .form-item').addClass("odd_element");
    jQuery(".even_child").parents('div .form-item').addClass("even_element");        
});

function model_mapping_selected(control, form_variables) {
    if (control.value) {
        for (var key in form_variables) {
            var obj = form_variables[key];
            fieldset_id = 'edit-webformfield-fieldset-' + form_variables[control.value]['nid'] + '-' + key;
            fieldset = document.getElementById(fieldset_id);
            if (fieldset && fieldset !== 'null' && fieldset !== 'undefined') {
                if (key == control.value) {
                    fieldset.style.display = "block";
                }
                else {
                    fieldset.style.display = "none";
                }
            }
        }
    }
}