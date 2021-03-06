-- ACCESS=access content
SELECT b.batch_name, 
 b.batch_description, b.nid, `nid`,
 COUNT(bi.batch_item_id) `item_count`,
 bi.batch_item_id,
 SUM(bi.file_count) `file_count`,
 SUM(bi.file_size) `file_size`,
 (SELECT COUNT(batch_item_id) FROM islandora_digital_workflow_batch_items ibi WHERE ibi.batch_id = b.batch_id AND ibi.progress = 'New') as `progress_new_count`,
 (SELECT COUNT(batch_item_id) FROM islandora_digital_workflow_batch_items ibi WHERE ibi.batch_id = b.batch_id AND ibi.progress = 'In Progress') as `progress_in_progress_count`,
 (SELECT COUNT(batch_item_id) FROM islandora_digital_workflow_batch_items ibi WHERE ibi.batch_id = b.batch_id AND ibi.progress = 'Prepared') as `progress_prepared_count`,
 (SELECT COUNT(batch_item_id) FROM islandora_digital_workflow_batch_items ibi WHERE ibi.batch_id = b.batch_id AND ibi.progress = 'Ingested') as `progress_ingested_count`,
 (SELECT COUNT(batch_item_id) FROM islandora_digital_workflow_batch_items ibi WHERE ibi.batch_id = b.batch_id AND ibi.progress = 'Completed') as `progress_completed_count`,
 b.lastmod,
 b.created,
 b.nid
FROM islandora_digital_workflow_batch b
JOIN islandora_digital_workflow_batch_items bi ON (bi.batch_id = b.batch_id)
WHERE b.batch_name = :batch_name
