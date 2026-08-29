# Successor receipt V1 — shard000 pair binder CI

predecessor_path: `receipts/2026-08-29/conversation-shard000-pair-binding.receipt.v1.json`  
predecessor_commit: `7b968a389b9ae420ec26e41f44053f8c97cafff0`  
observed_at: `2026-08-29`  
mode: `APPEND_ONLY_SUCCESSOR`  
claim_allowed: `false`

## Binder implementation

```yaml
repository: rafaelmeloreisnovo/RafPolimata
binder_commit: 4bea3b0e40ed155da0b3cd6860743ad7e2c51f3c
test_commit: 50ebf659b911025c034b066252a69368899ff47a
workflow_commit: 97648a153f1ba541ebc4b19e20a7ab5df04a571f
workflow: Conversation Pair Binder
run_id: 33235947546
job_id: 99056844937
status: completed
conclusion: success
```

Observed steps:

```text
Set up job                    SUCCESS
checkout@v4                  SUCCESS
setup-python@v5              SUCCESS
Run binder tests             SUCCESS
post steps                   SUCCESS
Complete job                 SUCCESS
```

Promotion:

```yaml
binder_remote_ci: PASS_OBSERVED
binder_fixture_execution: VERIFIED_LIMITED_CI
```

## Physical source already observed

```yaml
source: conversations-000.json
drive_file_id: 1VfP3u2BUBJ7aByrHh-PUfo4cW1jj3SBZ
bytes: 28152659
sha256: 27b3ebe900dcf811e648f0bcb8da02cc5e1e1230049567cdd1a83a6df317ee06
conversation_count: 100
message_objects: 9850
user_messages: 4761
assistant_messages: 5089
user_to_assistant_pairs: 4906
```

Pair rule:

```text
for each textual assistant node
→ follow mapping.parent ancestry
→ nearest textual user ancestor
→ preserve branch lineage
```

## Feedback boundary

`message_feedback.json` was observed with 50 records across 49 conversations: 32 `thumbs_up`, 18 `thumbs_down`.

The feedback records expose `conversation_id` but no response `message_id` in the observed file. Therefore:

```text
conversation feedback != response-level feedback
thumbs_up/down != evidence/gap/risk/urgency
```

No operational score is derived from those ratings.

## Corpus inventory state

The durable NOVOexport source root is observed. The nominal conversation shard range is `000..050`. The current Drive inventory exposes 50 shard objects and does not expose `conversations-018.json`.

```yaml
source_root: SOURCE_ROOT_OBSERVED
nominal_shard_slots: 51
observable_shards: 50
missing_shard_018: TOKEN_VAZIO_PROVIDER_OBJECT
full_pairwise_binding_executed: false
```

## Still-open gates

```yaml
all_observable_shards_pair_binding: TOKEN_VAZIO_NOT_EXECUTED
missing_shard_018: TOKEN_VAZIO_PROVIDER_OBJECT
operational_metrics_per_interaction: TOKEN_VAZIO_UNDERIVED
full_longitudinal_coverage: false
claim_allowed: false
```

## Anti-regression

```text
CI PASS on binder fixtures != 50-shard corpus execution
shard000 execution != complete corpus coverage
conversation-level thumbs != response-level evaluation
missing shard != empty shard
```

Only a later successor with the corresponding physical source and execution receipt may promote these gates.
