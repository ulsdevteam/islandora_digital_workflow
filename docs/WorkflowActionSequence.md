# Workflow Sequences and Workflow Actions
This section needs to be written.

## Workflow Actions
This section needs to be written.

## Workflow Sequences 
This section needs to be written.

## Understanding how batches are special Drupal node objects
Since the Islandora Digital Workflow batch is built upon a Drupal node, many of the core-Drupal features could be used to work with these batches.

## Understanding and customizing "Islandora Digital Workflow Stage Vocabulary"
The standard Drupal taxonomy for this can be found /admin/structure/taxonomy/workflow_stage_vocab.  The user must have or be granted the "Administer vocabularies and terms" permission in order to customize Taxonomy terms within this vocabulary.  Detailing how Drupal taxonomy works is outside of the scope of this document since that is core-Drupal functionality and it is already very well documented.

An example of using Taxonomy Stages with workflow batches may be to want a specific role to work on all batches that have a "Problem" stage.