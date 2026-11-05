---
name: execution-environment-recovery
description: Recover from transient execution-environment, network or server disconnections with bounded increasing-delay retries and verified resumption.
---

# Execution environment recovery

Applies to managed execution environments on any host. This is independent of
Android and iOS exporter workflows; it does not change their platform procedures.

## Bounded backoff

When an execution tool disconnects, its transport fails, or an environment
unexpectedly becomes unavailable during authorized work:

1. Briefly tell the user that execution was interrupted and recovery is being
   attempted. Keep the task active; a first transient failure is not the final
   failure report.
2. After the initial failure, allow up to **three retries** with increasing
   delays. Approximately **10, 50 and 100 seconds** is the default example, not
   a rigid timing requirement. Adapt to explicit recovery status or retry hints;
   small jitter is appropriate to avoid synchronized retries.
3. Use orchestration-side timers when executor-side waiting is unavailable.
   Keep each blocking wait at most 60 seconds: split a longer delay, such as
   100 seconds, into two 50-second waits. Continue concise progress updates at
   least every 60 seconds. Useful independent work can occupy the delay; do
   not add another full delay if that work already consumed it.
4. Probe availability with a harmless read before resuming. Use the dedicated
   environment-start wait tool only when an environment is actually marked
   `starting` and its tool contract permits it; it is not a general wait for an
   unavailable environment. Connector availability alone does not establish
   executor recovery.
5. On recovery, inspect the workspace, branch/HEAD, pending changes and relevant
   outputs. Resume from the interrupted step, preserving prior authorization,
   task scope and validated results.

## Verify effects before retrying writes

A lost response does not establish that an operation failed. Before retrying a
commit, push, upload, deployment or external write, check its actual result.
Read the remote ref or object/status as appropriate; resume only the missing
step. Use compare-and-swap or an exact force-with-lease when updating a branch.
Do not blindly replay non-idempotent operations or duplicate already completed
work. Never describe a local result as published until the remote is verified.

## Stop conditions and reporting

After all three retries fail, stop dependent execution and report the remaining
blocker. State what completed, what was verified, what remains unpublished or
unfinished, and the relevant local/remote revision when available. Do not claim
success from a prepared result alone. Mark remote work as pending only after
recovery attempts are exhausted, unless an immediate status correction is needed
to prevent a misleading publication claim.

Backoff is for plausibly transient infrastructure failures. Do not retry clear
permission/policy rejections, invalid requests, deterministic build failures or
an explicitly destroyed environment as though they were network collisions.
Honor an explicit user stop or retry limit. Do not request renewed permission
for work already authorized merely because its environment disconnected.
