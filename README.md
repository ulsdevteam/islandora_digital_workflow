## Islandora Digital Workflow
To prepare and track digital content for possible ingest into Islandora.

### Considerations
Before implementing any workflow tool, the capabilities of that tool should be matched against your needs.  Some planning should be done ahead of time to:
1. whether or not the Islandora Batch ingest is a viable method for ingesting content to your repository.
2. which file systems could be accessible to the web server that is running Islandora Digital Workflow -- needed for the various Ingest paths.
3. identify the users and roles in order to set up permissions.
4. identify the types of objects (Islandora models) that you could track and whether or not CSV files could be created to drive the generation of objects within a batch.
5. what metadata should be used - whether it could be generated from CSV or the XML files exist on the file system.
6. what actions need to happen for each type of Islandora model before objects can be ingested.

### Core Workflow Abilities:
 The handling of batches of related objects through a workflow is important and the table below details a lot of the core functionality that should be supported when converting these objects into digital files with the intent of ultimately ingesting them into Islandora.  Below is the basic functionality of the **Islandora Digital Workflow**.
- **Work with digitization requests**
        - Requires islandora_digitization_requests and webform modules to be installed
        - integrate digitization requests into the workflow by marking the available webforms as digitization requests.  Workflow configuration would allow for individual mapping between the webform fields to workflow batch fields, which fields are searchable, and even specific mapping between each multi-select prompt that might be mapped to the islandora_model field so that multiple "islandora_model-specific" batches would be generated.
- **Custom Actions, Taxonomy Stages, and Workflow Sequences**
        - Custom actions may be defined to be added per item or at a batch level -- the action could be an "Ingest Prerequisite", or a "Publish Prerequisite" or both, it could trigger a batch stage change (for example to add "Scanned" or "In Processing" taxonomy tag to a batch when the "Item scanned" action is assigned to a single items or all items in the batch.  For more information, please refer to the ["Workflow Sequences and Workflow Actions" documentation](docs/WorkflowActionSequence.md).
- **Work with batches** - since batches of objects are carried through the workflow, they are assumed to be grouped together for a reason.  This module assumes that all items would represent the same islandora model, be related to the same collection/s and site/s (if using isMemberOfSite), and have similar batch default values (see "Collect batch level object metadata" below)
        - Create batches
        - Edit batch information
        - Delete batches
        - Hide completed batches (not implemented)
- **Collect batch metadata**
         - Description of batch
         - Sequence (sets the actions for batch)
         - Scanning specifications, including condition and handling information, resolution, page edge/blanks/editing, color targets, color, structural metadata requirements, etc.
         - Associated voyager bib id/ EAD id, as appropriate (used to link the objects with the appropriate metadata files (??)
- **Collect batch level object metadata** - Similar objects in batches, optional field values that would be used for the metadata of all items in the batch.  When items are generated from CSV the batch default field value is only used if a row's value for that field is not provided.  When items are generated from a MARC collection or EAD file (not implemented yet), the batch default field value will overwrite or insert that value in the item's metadata.
         - Rights, publication, rights holder, permission notes
         - Depositor
         - Default typeOfResource
         - Default genre
- **Collect collection/site information** - is used for all items in a batch
         - If the configuration is set to "Use review collections", objects that are ingested are first related to review collections which are clones of the intended collection.  After the objects have been reviewed, the user would "Publish the item" in order to add the objects to the intended collection.  If "Use review collections" is not configured, objects would be ingested directly to the public-facing web.
         - Objects can be related to sites.  The configuration options to "Assign PID values" and "Use the isMemberOfSite property?" require using the ulsdevteam forks of various *_batch modules (see "Islandora Batch Ingest Modules" above for more information)
- **Identify progress on batch**
        - Stage in process
                - physical objects are "in process" for scanning
                - Check-in complete
                - Scanning complete
                - QC/Structural metadata complete (as appropriate)
                - Ingest complete
                - Check out of physical objects signifying that they can be returned or stored because there is no further need of them
- **Test and/or assign identifiers**
        - Old system tested to make sure that an object or batch with matching id didn’t exist
        - If system could assign PIDS, that might be preferable.
        - Objects may have source ids (accession numbers, barcodes, etc.) to help the users who are scanning the content with identification.  They use a barcode reader for check-in and scanning processes, so that might be a challenge?
- **Entering object level metadata for serials (other use cases?)** (not implemented)
         - Empty batch is made and serial object metadata is created at the time of scanning.
- **Other Metadata? Do we want to continue tracking the following in this system?** (future considerations)
         - Property Owner
         - Priority information
         - All metadata related to requests
                - Requests not going online? Should they be part of this workflow or not?

## Installation
The Islandora Digital Workflow module relies on some other modules in order to be able to run.
**Required modules:**
 - [Islandora](https://github.com/Islandora/islandora) The Islandora core module.
 - [Islandora Solr Search](https://github.com/Islandora/islandora_solr_search.git) - Searches an Islandora Solr index.
 - [Islandora MARC Utility](https://github.com/ulsdevteam/islandora_marc_utility.git) - MARC utilities for parsing MARC mrc or MARCXML (collection) files.
 - [Forena](https://git.drupal.org/project/forena.git) | [ulsdevteam "Forena"](https://github.com/ulsdevteam/forena.git) *The ulsdevteam feature branch of Forena reports adds a Description value to the reports.  If the ulsdevteam branch of this module is installed or if the code is eventually merged into Forena, the descriptions of reports would be displayed -- else, only their Titles can be displayed.*
 - Taxonomy (Drupal core module must be enabled)
 - [Views](https://git.drupal.org/project/views.git)
 - see **Islandora Batch Ingest Modules** section below as well in order to be able to prepare batches for Drupal ingest via drush commands.


 **Optional modules:**
 - [Rules](https://git.drupal.org/project/rules.git)
 - [Islandora METS Editor](https://github.com/ulsdevteam/islandora_mets_editor.git) To provide a way to create, edit, manage METS files in Islandora.
 - [Islandora MARC XML](https://github.com/Islandora/islandora_marcxml) An Islandora module which performs transformations between MODS and MARCXML.  When present, the configuration will add the transforms to the "MARC to MODS Transform" selection choices.
 - [Islandora Digitization Requests](https://github.com/ulsdevteam/islandora_digitization_requests.git) A module that allow for the creation of Webforms and the other to allow use of these to handle Digitization Request submissions.  For more information on how this works with the Islandora Digital Workflow, please see the ["Digitization Requests" documentation](docs/DigitizationRequests.md).

**Islandora Batch Ingest Modules**
In order to batch ingest Islandora models, they each require a specific module to be installed.  Additionally, the ability to assign PID value for objects as they ingest as well as to add the "isMemberOfSite" relationship would require the ulsdevteam instance of each module.  The configuration page provides links to download and install each of the required modules.  These are:
 - [Basic Image](https://github.com/Islandora/islandora_batch.git) | [ulsdevteam "Basic Image"](https://github.com/ulsdevteam/islandora_batch.git)
 - [Finding Aid](//) (not developed)
 - [Internet Archive Book](https://github.com/Islandora/islandora_book_batch.git) | [ulsdevteam "Internet Archive Book"](https://github.com/ulsdevteam/islandora_book_batch.git)
 - [Large Image]() | [ulsdevteam "Large Image"](https://github.com/ulsdevteam/islandora_batch.git)
 - [Manuscript](https://github.com/ulsdevteam/islandora_manuscript_batch.git) *ulsdevteam version is the only instance.*
 - [Newspaper](https://github.com/Islandora/islandora_newspaper_batch.git) | [ulsdevteam "Newspaper"](https://github.com/ulsdevteam/islandora_newspaper_batch.git)
 - [Newspaper Issue](https://github.com/Islandora/islandora_newspaper_batch.git) | [ulsdevteam "Newspaper Issue"](https://github.com/ulsdevteam/islandora_newspaper_batch.git)
 - [Web ARChive](//) (not developed)

Installing this module will also set up the following:
 - Drupal node content type `workflow_batch` with CCK fields for each batch property
 - Drupal taxonomy and several vocabularies to track batch "Stage", and "Content Type" (correlates to Islandora object models).
 - create the tables: islandora_digital_workflow_batch, islandora_digital_workflow_batch_items, islandora_digital_workflow_actions, islandora_digital_workflow_transactions, islandora_digital_workflow_problem_items, islandora_digital_workflow_sequence, islandora_digital_workflow_model_sequence, islandora_digital_workflow_sequence_actions.  It will also insert the initial set of records into the islandora_digital_workflow_actions table which will set up the default set of actions.
 - Deploy the Forena reports SQL and FRX files to the configured Forena locations.  This requires that the Drupal "**Private file system path**" is configured and editable by the web server (this path is configured at /admin/config/media/file-system).
 - custom Drupal views to display the workflow_batch nodes

*This module also uses two free image libraries for the various action glyphs.  These images allow for usage but not distribution.  They are  https://www.icondeposit.com/theicondeposit:159 and http://brankic1979.com/icons/.*

## Permissions
Each islandora model is exposed as an "{MODEL_NAME} Islandora Digital Workflow" permission under the Drupal permissions table `/admin/people/permissions`.  User roles and individual users will need to be configured for access to the various models.

[Permissions.md](docs/Permissions.md) contains more detailed information on setting up Permissions for the workflow.

One important note about the reports.  Since this uses the Forena reporting system, any users of the workflow who are granted access to the "Run reports - User can run and view reports on batches and items" permission will also need to be granted separate permissions "Access Islandora Workflow Reports Data" and "List reports" for the Forena reports.


***NOTE:**  even though permissions exist for **all islandora models**, the "Supported Models" settings within the Islandora Digital Workflow limit the total set of object models that could ever appearing within the various parts of the interface before consideration of the user permissions.*

# Configuration
### Configuring "Workflow Sequences" and "Workflow Actions"
For information on configuring custom Workflow Actions or defining Workflow Sequences, please refer to the ["Workflow Sequences and Workflow Actions" documentation](docs/WorkflowSequences.md).

# Usage
## Batch tracking, stats, and Stages explained
This section needs to be written.

### Islandora Digital Workflow Dashboard
<img src="https://user-images.githubusercontent.com/19391126/53822705-420a3c00-3f3e-11e9-8a65-3b14f09890a3.png" width="400" />

### Islandora Digital Workflow Dashboard - status boxes for items needing actions
The various boxes that appear are based on the state of the underlying workflow data as well as the permissions of the currently logged-in user.

<img src="https://user-images.githubusercontent.com/19391126/53822867-92819980-3f3e-11e9-9a1a-d10a5068901c.png" width="400" />

### Search interface
The search will look in all batch and batch item string fields as well as any webform submission data from all configured Digitization Requests that match the string search.

If the code only finds one result, the code will redirect the user to the Batch View for the perfect match, else true Batch Records and items appear first in search results -- and if configured, any matches on Digitization Requests are displayed.

<img src="https://user-images.githubusercontent.com/19391126/53823906-b5ad4880-3f40-11e9-92ae-104417976488.png" width="400" />

### Batch node View
<img src="https://user-images.githubusercontent.com/19391126/53822996-d70d3500-3f3e-11e9-8ca3-b8580673df92.png" width="400" />

### Batch node Edit
Access depends on the the currently logged-in user's permissions.

<img src="https://user-images.githubusercontent.com/19391126/53823081-0a4fc400-3f3f-11e9-95ba-e32edacecea6.png" width="400" />

### "Batch Files" tab (top)
<img src="https://user-images.githubusercontent.com/19391126/53823260-661a4d00-3f3f-11e9-88c6-f5ba510befc5.png" width="400" />

### "Batch Files" tab (bottom)
<img src="https://user-images.githubusercontent.com/19391126/53823277-716d7880-3f3f-11e9-8d2a-507bb9498cb3.png" width="400" />

### "Batch Items" tab
This page may be the most useful page for every batch.  From here, (and depending on the user's permissions) items can individually (manually) be added, multiple items may be merged (ONLY in the case of CSV files), Transactions can be added / removed / edited (based on permission), items can be deleted, and overall progress can be easily assessed.

<img src="https://user-images.githubusercontent.com/19391126/53823338-9366fb00-3f3f-11e9-9b37-fedce3ac6af0.png" width="400" />

### "Batch Items" - add/remove transaction to multiple items)
<img src="https://user-images.githubusercontent.com/19391126/53823485-e2ad2b80-3f3f-11e9-8d7d-57fb80b3eb65.png" width="400" />

### "Batch Transactions"
<img src="https://user-images.githubusercontent.com/19391126/53823604-2011b900-3f40-11e9-8473-c31e67f9d592.png" width="400" />

### Batch Item - details (top)
<img src="https://user-images.githubusercontent.com/19391126/53824174-5a2f8a80-3f41-11e9-94b2-18264ecbc067.png" width="400" />

### Batch Item - transactions (middle)
<img src="https://user-images.githubusercontent.com/19391126/53824279-92cf6400-3f41-11e9-96d9-6ba81a194692.png" width="400" />

### Batch Item - files & drush debug (bottom)
<img src="https://user-images.githubusercontent.com/19391126/53824365-c9a57a00-3f41-11e9-915d-59ade0a6c816.png" width="400" />


## Author / License
Written by [Willow Gillingham](https://github.com/bgilling) for the [University of Pittsburgh](http://www.pitt.edu).  Copyright (c) University of Pittsburgh.

Released under a license of GPL v2 or later.
