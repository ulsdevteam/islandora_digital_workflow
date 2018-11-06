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
- **Work with batches**
	- Create batches
	- Edit batch information
	- Delete batches
	- Hide completed batches
- **Identify progress on batch**
	- Stage in process		
		- In DRL for scanning		
		- Check-in complete		
		- Scanning complete		
		- QC/Structural metadata complete (as appropriate)		
		- Ingest complete		
		- Check out of DRL
- **Test and/or assign identifiers**
	- Old system tested to make sure that an object or batch with matching id didn’t exist
	- If system could assign PIDS, that might be preferable. 
	- Objects may have source ids (accession numbers, barcodes, etc.) to help DRL with identification.  DRL uses barcode reader for check-in and scanning processes, so that might be a challenge?
- **Collect batch metadata**
	 - Description of batch
	 - Sequence (sets the actions for batch)
	 - Scanning specifications, including condition and handling information, resolution, page edge/blanks/editing, color targets, color, structural metadata requirements, etc. 
	 - Associated voyager bib id/ EAD id, as appropriate (used to link the objects with the appropriate metadata files (??)
- **Collect batch level object metadata**
	 - Similar objects in batches, should be optional
	 - Rights, publication, rights holder, permission notes
	 - Depositor
	 - Default typeOfResource
	 - Default genre
- **Collect collection/site information**
	 - Should be the same for all items in a batch.
- **Entering object level metadata for serials (other use cases?)**
	 - Empty batch is made and serial object metadata is created at the time of scanning. 
- **Other Metadata? Do we want to continue tracking the following in this system?**
	 - Property Owner
	 - Priority information
	 - All metadata related to requests		
		- Requests not going online? Should they be part of this workflow or not?

## Installation
The Islandora Digital Workflow module relies on some other modules in order to be able to run.  
**Required modules:**
 - [Forena](https://git.drupal.org/project/forena.git) | [ulsdevteam "Forena"](https://github.com/ulsdevteam/forena.git) *The ulsdevteam feature branch of Forena reports adds a Description value to the reports.  If the ulsdevteam branch of this module is installed or if the code is eventually merged into Forena, the descriptions of reports would be displayed -- else, only their Titles can be displayed.*
 - [Rules](https://git.drupal.org/project/rules.git)
 - Taxonomy (Drupal core module must be enabled)
 - [Views](https://git.drupal.org/project/views.git)
 - see **Islandora Batch Ingest Modules** section below as well in order to be able to prepare batches for Drupal ingest via drush commands.

 **Optional modules:**
 - [Islandora METS Editor](https://github.com/ulsdevteam/islandora_mets_editor.git) To provide a way to create, edit, manage METS files in Islandora.
 - [Islandora CSV to MODS](https://github.com/ulsdevteam/islandora_csv_to_mods.git) Islandora utility to create MODS from CSV upload and potentially update the related Islandora objects.

**Islandora Batch Ingest Modules** 
In order to batch ingest Islandora models, they each require a specific module to be installed.  Additionally, the ability to assign PID value for objects as they ingest as well as to add the "isMemberOfSite" relationship would require the ulsdevteam instance of each module.  The configuration page provides links to download and install each of the required modules.  These are:
 - [Basic Image](https://github.com/Islandora/islandora_batch.git) | [ulsdevteam "Basic Image"](https://github.com/ulsdevteam/islandora_batch.git)
 - [Finding Aid](//) (not developed)
 - [Internet Archive Book](https://github.com/Islandora/islandora_book_batch.git) | [ulsdevteam "Internet Archive Book"](https://github.com/ulsdevteam/islandora_book_batch.git)
 - [Large Image]() [ulsdevteam "Large Image"](https://github.com/ulsdevteam/islandora_batch.git)
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
### Actions
This section needs to be written.
### Workflow Sequences
This section needs to be written.

# Usage
## Batch tracking, stats, and Stages explained
This section needs to be written.

## Author / License
Written by [Willow Gillingham](https://github.com/bgilling) for the [University of Pittsburgh](http://www.pitt.edu).  Copyright (c) University of Pittsburgh.

Released under a license of GPL v2 or later.

