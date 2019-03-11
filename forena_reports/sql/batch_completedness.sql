-- ACCESS=access content
-- REQUIRED ACTIONS BASED ON EACH BATCH
SELECT wa.description `required action_id`, b.batch_name, b.nid, ws.name `workflow sequence name`
FROM islandora_digital_workflow_batch b
JOIN islandora_digital_workflow_sequence ws ON (ws.workflow_sequence_id = b.workflow_sequence_id)
JOIN islandora_digital_workflow_sequence_actions wsa ON (wsa.workflow_sequence_id = ws.workflow_sequence_id)
JOIN islandora_digital_workflow_actions wa ON (wa.action_id = wsa.action_id)
WHERE 
 b.progress <> 'Completed' AND
 wsa.is_publish_prerequisite = 1 AND wsa.is_ingest_prerequisite = 0 AND wa.is_batch_action = 1 AND
 wa.action_id NOT IN (
            -- SET OF ACTUAL "BATCH" ACTIONS THAT HAVE BEEN COMPLETED FOR EACH BATCH
            SELECT tx.action_id
            FROM islandora_digital_workflow_transactions tx 
            WHERE tx.batch_id = b.batch_id
 )
UNION 
SELECT wa.description `required action_id`, b.batch_name, b.nid, ws.name `workflow sequence name`
  FROM islandora_digital_workflow_batch_items bi 
  JOIN islandora_digital_workflow_batch b ON (bi.batch_id = b.batch_id)
  JOIN islandora_digital_workflow_sequence ws ON (ws.workflow_sequence_id = b.workflow_sequence_id)  
  JOIN islandora_digital_workflow_sequence_actions wsa ON (wsa.workflow_sequence_id = ws.workflow_sequence_id)
  JOIN islandora_digital_workflow_actions wa ON (wa.action_id = wsa.action_id)
WHERE 
 b.progress <> 'Completed' AND 
 wsa.is_publish_prerequisite = 1 AND wsa.is_ingest_prerequisite = 0 AND wa.is_batch_action = 0 AND
 wa.action_id NOT IN (
            -- SET OF ACTUAL "BATCH_ITEM" ACTIONS THAT HAVE BEEN COMPLETED FOR EACH BATCH
            SELECT tx.action_id
            FROM islandora_digital_workflow_transactions tx
            WHERE tx.batch_item_id = bi.batch_item_id
        )
GROUP BY bi.batch_id, wsa.action_id
