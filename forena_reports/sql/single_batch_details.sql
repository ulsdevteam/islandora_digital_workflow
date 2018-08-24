-- ACCESS=access content
SELECT 
 bi.identifier, 
 bi.title,
 bi.file_count,
 bi.file_size,
 bi.filename `MASTER filename`
FROM islandora_digital_workflow_batch b
JOIN islandora_digital_workflow_batch_items bi ON (bi.batch_id = b.batch_id)
WHERE b.batch_name = :batch_name
ORDER BY bi.identifier