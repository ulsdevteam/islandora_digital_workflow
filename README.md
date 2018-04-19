

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
	- Objects may have source ids (accessionnumbers, barcodes, etc.) to help DRL with identification.	DRL uses barcode reader for check-in and scanning processes, so that might be a challenge?
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
This module does require one manual step in order to be able to update the dynamic CSS file.  The configuration will provide these instructions if an attempt is made to update the settings before performing this step to change the ownership of this file.

    $ cd islandora_digital_workflow/css
    $ sudo chown apache islandora_digital_workflow_dynamic.css
Installing this module will also set up the following:
 - workflow_batch Drupal node content type with CCK fields for each batch property
 - Drupal taxonomy and several vocabularies to track batch "Stage", and "Content Type" (correlates to Islandora object models).
 - custom Drupal views to display the workflow_batch nodes

## Permissions
There are three basic roles defined under `/admin/people/permissions/roles` Curator, DRL, and MAD.  These roles also have Drupal permissions defined at `/admin/people/permissions` under the **Islandora Digital Workflow** section that can be configured to allow access to various parts of the system.  Users who use this system will need to be added to the 
- DRL
- MAD
- Curator
- Workflow User - is the considered the lowest access role.

Additionally, the roles' access to specific Islandora models can be controlled with the module's configuration `/admin/islandora/islandora_digital_workflow`.

## Author / License

Written by Brian Gillingham for the [University of Pittsburgh](http://www.pitt.edu).  Copyright (c) University of Pittsburgh.

Released under a license of GPL v2 or later.

