import json

evts = []
with open("dump", "r") as infile:
    for line in infile.readlines():
        evts.append(json.loads(line))

evt = evts.pop(0)
print("evt: " + str(evt))
assert evt["BaseMetadata"]["TsResolutionNs"]["ns_per_ts"] == 1

evt = evts.pop(0)
print("evt: " + str(evt))
assert evt["BaseMetadata"]["IsrName"]["isr_id"] == 0
assert evt["BaseMetadata"]["IsrName"]["name"] == "ISR0"

print("...")

evt = evts.pop(-1)
print("evt: " + str(evt))
assert evt["Base"]["ts"] == 40
assert evt["Base"]["kind"]["EvtmarkerEnd"]["evtmarker_id"] == 0

