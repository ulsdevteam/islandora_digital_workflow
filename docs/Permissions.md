## Setting up permission roles

The Islandora Digital Workflow system is written with a lot of different permissions.  This document is meant to provide a good understanding of the various permissions needed.  Since Drupal handles user permission upon role definitions and a user can have more than one role, it may be possible to assign some existing roles with these permissions, yet additional roles could be created which control access to various parts of the workflow.

## Permission-based display elements

Certain permissions are going to see some additional or slightly modified output on some of the screens within the Workflow.

### Dashboard

This page is the main hub for users of the workflow.  Much of the display depends on the users&#39; permission.

In the main block of links, some of them only appear if the user has permission.

The **Create collection** link displays to users with the Islandora module &quot;Create child collection&quot; permission.

The **Create batch** link displays to users with &quot;Create new digital workflow batches&quot; permission.

The **Reports** link is only visible to users who have the &quot;Run reports&quot; permission.

The **Utilities** link is only visible to users who have the &quot;Use batch and items utilities forms&quot; permission.

The link to **Islandora Digital Workflow Settings** is only visible to users who have &quot;Manage Workflow&quot; permission.  Additionally, the link to &quot;User Permissions&quot; displays to users with the Drupal core &quot;Administer permissions&quot; permission.

In addition to the main block and the links that are based on permissions, several other blocks may display that are based on permission.

A block that displays incomplete batches is visible to users with the &quot;Update Batch Items&quot; permission.

A block that displays any rescanned delivery files is visible to users with &quot;User can &quot;Sync&quot; files from Delivery to workflow&quot; permission.

### Batch create form

Under the &quot;**Destination Islandora Content Model**&quot; select box, users with &quot;Administer Site Configuration&quot; permission will see a link to the Islandora Digital Workflow configuration to select the available models.

Furthermore, the actual set of models available that appear to any user for &quot;**Destination Islandora Content Model**&quot; depends on their permission to each content type as is listed in the Drupal /admin/people/permissions such as &quot;Create/Edit/Update Newspaper batches&quot; permission.

Under the &quot;**Workflow Sequence**&quot; selection box, users with &quot;Manage Workflow Sequences&quot; permission will see a link to configure the available workflow sequences.

Under the &quot;**Collections**&quot; selection box, users with &quot;Create child collection&quot; permission will see a link to create an Islandora Collection.

### Items and Item pages

In the form that appears in the &quot;**Item Details**&quot; section of this page&#39;s output, if a user has &quot;Edit Item MODS / MARC directly&quot; permission, the MODS text area and MARC (if applicable) will be editable – otherwise this would be grayed-out.

For items that have had a drush command issued on them for ingest, users with the &quot;View extra debug info&quot; permission will see an &quot; **Ingest Commands**&quot; section that will output the actual commands issued, the username, what time, and their output results.

On any Item page as well as on the Items page, underneath the table of transactions the batch&#39;s workflow sequence name is rendered as a link if the current user has &quot;Manage Workflow Sequences&quot; permission.

## Transactions page

The ability to edit and delete transactions requires the &quot;Edit / Delete Batch Transactions&quot; permission.  For users who have this permission, an additional dropdown box appears that allows deleting the selected actions as well as individual &quot;edit&quot; and &quot;delete&quot; button glyphs for the transactions tables.