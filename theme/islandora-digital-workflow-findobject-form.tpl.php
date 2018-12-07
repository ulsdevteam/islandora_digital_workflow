<?php
/**
* @file
* islandora-digital-workflow-findobject-form.tpl display template.
*
* Variables available:
* - $searchterm
*
*/
?>
<?php $current_url = $_SERVER["REQUEST_URI"]; ?>
<form id="lookup_item_form" name="form" method="POST" action="/islandora/islandora_digital_workflow/lookup">
  Search by batch name or identifier: <input type="text" size="20" name="q" autofocus>
  <input name="request_uri" type="hidden" value="<?php print (isset($current_url) ? ltrim($current_url, '/') : 'islandora/islandora_digital_workflow'); ?>" />
  <input type="submit" value="go" /></form>
<br style="clear:both" />
