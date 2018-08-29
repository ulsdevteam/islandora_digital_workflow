-- ACCESS=access content
SELECT b.batch_name, 
 b.batch_description,
 COUNT(bi.batch_item_id) `batch_item_id`,
 SUM(bi.file_count) `file_count`,
 SUM(bi.file_size) `file_size`,
 b.lastmod,
 b.nid
FROM islandora_digital_workflow_batch b
JOIN islandora_digital_workflow_batch_items bi ON (bi.batch_id = b.batch_id)
WHERE b.batch_name = :batch_name