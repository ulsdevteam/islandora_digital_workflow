# Digitization Requests
The Islandora Digital Workflow system is written to work in conjunction with Webforms and the Islandora Digitization Requests module.  If neither of those modules are installed, all options and interface prompts related to the Digitization Requests will not appear.

The major concept is that the Digital Workflow can be made to track Digitization Requests that ultimately may never be ingested into Islandora, but "could be".  These are usually faculty and sometimes staff requests.  These requests can be converted into `islandora_digital_workflow_batch` records (see the section on "Mapping" below).

### "Webforms" module
Webforms is a highly configurable module that allows forms to easily be set up to store submitted entries in the database.

### "Islandora Digitization Requests" module
The Islandora Digitization Requests module is a very lightweight module that only tracks whether or not any of the Webforms in any given Drupal site are to be considered as "Digitization Requests".  From the configuration screen, all of the webforms are displayed with a checkbox **"{name of webform}" is a Digitization Request?"**  What this means is that they will appear in several places 

The same configuration screen also allows for creating a new submission of each type by clicking on the **Create new "{name of webform}"** link.  Within the Islandora Digital Workflow code, any user who has the `ISLANDORA_DIGITAL_WORKFLOW_CREATE_DIGITIZATION_REQUEST` permission will be allowed to click a similar link from the user dashboard.  In this case, if there is only one type of webform configured as a Digitization Request, it will launch that submission page... if there are more than one "Digitization Request" forms, the link will render a page that displays a link for creating a submission to each of the configured Digitization Requests.

## Mapping
For those webforms that are set up to be Digitization Requests, each are able to be configured dynamically within the workflow settings screen `admin/islandora/islandora_digital_workflow` in the "Digitization Request Mappings and Search Fields" section.  For each webform, the field mapping selections are broken into two sub-sections **Text batch record fields** and **Numeric batch record fields**.  For each field, the left side for each field contains a select box that allows selection of any of the Webform's fields, while on the right side of the screen there is a text area that allows for supplying a default value.

#### What happens with those mapped fields that contained submission data?
Don't worry!  Whenever any submission is mapped into a batch record and there are any values from the submission that did not get mapped directly in to batch fields, the field name and the submitted value for each are stored in the batch_description field (if there is already a batch_description value from a submission mapping, the unmapped field info is appended to that description text).

## Search fields
For those webforms that are set up to be Digitization Requests, each are able to be configured dynamically within the workflow settings screen `admin/islandora/islandora_digital_workflow` in the "Digitization Request Mappings and Search Fields" section.  For each of the fields, simply check the checkbox beside any field that should be included in the dynamic search.
