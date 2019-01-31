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
        var fieldset_id = 'edit-webformfield-fieldset-' + form_variables[control.value]['nid'] + '-' + control.value;
        var fieldset = document.getElementById(fieldset_id);
        var element_name = 'model_mappings_' + form_variables[control.value]['nid'];
        var element = document.getElementById(element_name);
        // If the item is a select element
        if (form_variables[control.value]['type'] == 'select') {
            fieldset.style.display = "block";
//            var extra = JSON.parse(JSON.stringify(form_variables[control.value].extra));
//            console.log(extra);
//            console.log(extra.items);
//            var items_obj = extra.items;
//            console.log(items_obj);
        }
        else {
            fieldset.style.display = "none";
        }
    }
}