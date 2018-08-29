-- ACCESS=access content
SELECT t.transaction_id, a.action_id `action_id`, 
 a.description `Action`, 
 IFNULL(CONCAT(b.batch_name, ' (batch action)'), bi.title) `Item`, 
 CONCAT(IFNULL(b.batch_name, ''), IFNULL(b_by_bi.batch_name, '')) `batch_name`, 
 CONCAT(IFNULL(b.nid, ''), IFNULL(b_by_bi.nid, '')) `nid`,
 u.`name` `User`, 
 t.`timestamp` `timestamp`,
 bi.batch_item_id
FROM islandora_digital_workflow_transactions t
LEFT OUTER JOIN islandora_digital_workflow_actions a ON (a.action_id = t.action_id)
LEFT OUTER JOIN islandora_digital_workflow_batch b ON (b.batch_id = t.batch_id)
LEFT OUTER JOIN islandora_digital_workflow_batch_items bi ON (bi.batch_item_id = t.batch_item_id)
LEFT OUTER JOIN islandora_digital_workflow_batch b_by_bi ON (bi.batch_id = b_by_bi.batch_id)
LEFT OUTER JOIN users u ON (u.uid = t.drupal_uid)
WHERE (b.batch_name = :batch_name OR b_by_bi.batch_name = :batch_name)
ORDER BY t.timestamp DESC