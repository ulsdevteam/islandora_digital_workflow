## Islandora Digital Workflow
To prepare and track digital content for possible ingest into Islandora.

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
	- Objects may have source ids (accessionnumbers, barcodes, etc.) to help DRL with identification.  DRL uses barcode reader for check-in and scanning processes, so that might be a challenge?
- **Collect batch metadata**
	 - Description of batch
	 - Sequence (sets the actions for batch)
	 - Scanning specifications,including condition and handling information, resolution, page edge/blanks/editing, color targets, color, structural metadata requirements, etc. 
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
Each Islandora model requires a specific module to be installed.  Additionally, the ability to assign PID value for objects as they ingest as well as to add the "isMemberOfSite" relationship would require the ulsdevteam instance of each module.  The configuration page provides links to each of the required modules so that they can be installed.  These are:
 - [Basic Image](https://github.com/Islandora/islandora_batch.git), [ulsdevteam](https://github.com/ulsdevteam/islandora_batch.git)
 - [Finding Aid](//) (not developed)
 - [Internet Archive Book](https://github.com/Islandora/islandora_book_batch.git), [ulsdevteam](https://github.com/ulsdevteam/islandora_book_batch.git)
 - [Large Image]() [ulsdevteam](https://github.com/ulsdevteam/islandora_batch.git)
 - [Manuscript](https://github.com/ulsdevteam/islandora_manuscript_batch.git) *ulsdevteam version is the only instance.*
 - [Newspaper](https://github.com/Islandora/islandora_newspaper_batch.git), [ulsdevteam](https://github.com/ulsdevteam/islandora_newspaper_batch.git)
 - [Newspaper Issue](https://github.com/Islandora/islandora_newspaper_batch.git), [ulsdevteam](https://github.com/ulsdevteam/islandora_newspaper_batch.git)
 - [Web ARChive](//) (not developed)

Installing this module will also set up the following:
 - workflow_batch Drupal node content type with CCK fields for each batch property
 - Drupal taxonomy and several vocabularies to track batch "Stage", and "Content Type" (correlates to Islandora object models).
 - custom Drupal views to display the workflow_batch nodes

## Permissions
Each islandora model is exposed as an "{MODEL_NAME} Islandora Digital Workflow" permission under the Drupal permissions table `/admin/people/permissions`.  User roles and individual users will need to be configured for accesss to the various models.  

Additionally, the roles' access to specific Islandora models can be controlled with the module's configuration `/admin/islandora/islandora_digital_workflow`.

***NOTE:**  even though permissions exist for **all islandora models**, the "Supported Models" settings within the Islandora Digital Workflow limit the total set of object models that could ever appearing within the various parts of the interface before consideration of the user permissions.*

## Author / License

Written by Brian Gillingham for the [University of Pittsburgh](http://www.pitt.edu).  Copyright (c) University of Pittsburgh.

Released under a license of GPL v2 or later.
