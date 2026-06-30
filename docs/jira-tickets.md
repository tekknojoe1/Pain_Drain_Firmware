# PainDrain — Jira Epics & Tickets

Working copy of the epic and ticket write-ups. Descriptions are forward-looking
(the task/goal, not "what we did" — that goes in status updates). Each ticket has
**Description, Acceptance Criteria, Definition of Done, Test Cases, Dependencies**.
Each epic uses the broader epic sections. Battery level is reported over the BLE
**Battery Service (BAS)**; firmware version/slot over the **DIS Firmware Revision**.

---

# Epics

## OTA Updates (Firmware + Mobile App)

**Summary**
Let users update their PainDrain device's firmware wirelessly from the mobile app —
safely, with no cable or clinic visit.

**Background**
Today, firmware can only be changed by physically connecting a hardware programmer.
Deployed devices need a way to receive fixes and features in the field. This epic
delivers end-to-end over-the-air (OTA) updates over BLE — spanning both the device
firmware and the mobile app — with enough safety that a failed update can never
brick a device.

**Goals**
- A user can update their device from the app in a couple of minutes.
- An update is safe: an interruption, corruption, or wrong/older image never bricks
  the device — it keeps running the current firmware.
- User presets/settings are preserved across updates.

**Scope**
- *Firmware:* dual-app A/B layout (two slots + a small bootloader that boots the
  newest valid slot); the app receives the image over BLE (Cypress DFU + Bootloader
  service), writes it to the inactive slot, integrity-checks it (CRC), and reboots
  into the new version; reports its running version/slot via the DIS; preserves
  presets; raises the BLE MTU for speed.
- *Mobile App:* connect (no pairing), read the running slot, upload the correct
  image from the release bundle via the Cypress DFU protocol, show progress, and
  report success/failure.

**Out of Scope**
- Updating the bootloader itself over the air (needs a hardware programmer).
- Updating while charging (deferred — BLE is off while charging).

**Acceptance Criteria**
- A user completes a firmware update entirely from the app, end-to-end.
- A failed/interrupted/corrupt/older update leaves the device on its working
  firmware (no brick, no accidental downgrade).
- Presets survive an update.
- A full update completes in roughly 1–2 minutes.

**Success Metrics**
- Update success rate (near 100% on a stable connection).
- Time per update.
- Zero bricked devices from a failed update.

**Dependencies, Risks & Assumptions**
- BLE connection stability and negotiated MTU drive speed/reliability.
- Charging turns BLE off, so updates happen on battery.
- Future changes to the saved-data format need schema migration so presets aren't lost.

**Child Stories**
- Firmware: BDPD-1, BDPD-5 (bootloader), BDPD-6 (app DFU service + integrity + presets).
- Mobile App: BDPD-2 (app OTA flow).

---

## Battery Level (Firmware + Mobile App)

**Summary**
Show users their device's battery level in the mobile app, sourced from the
device's fuel gauge over BLE.

**Background**
Users need to know how much charge their device has so they can recharge before it
runs out mid-use. The device has a fuel gauge; this epic reads it, exposes it over
the standard BLE Battery Service, and displays it in the app — spanning both
firmware and app.

**Goals**
- The app shows an accurate battery level for the connected device.
- The level updates live, not just on connect.

**Scope**
- *Firmware:* read state-of-charge (%), voltage (mV), and charging status from the
  bq28Z610 fuel gauge; report the level over the standard BLE **Battery Service**
  (BAS, `0x180F` / Battery Level `0x2A19`), updated regularly with notifications.
- *Mobile App:* read the Battery Service, display the battery %, and subscribe for
  live updates.

**Out of Scope**
- Battery health / cycle count / charge-time estimates (future).

**Acceptance Criteria**
- The app displays the device's real battery %, matching the device within tolerance.
- The value updates live as the charge changes (via notifications).

**Success Metrics**
- Displayed value matches the actual charge within an agreed tolerance.
- Updates appear within a few seconds of a real change.

**Dependencies, Risks & Assumptions**
- Fuel-gauge accuracy/calibration affects the reported %.
- Relies on BLE notification reliability for live updates.

**Child Stories**
- Firmware: BDPD-4, BDPD-7 (read the fuel gauge), BDPD-8 (report over the Battery Service).
- Mobile App: BDPD-3 (battery indicator).

---

# Stories / Tasks

## BDPD-1 — Device Firmware: OTA Updates

**Description**
Enable over-the-air firmware updates on the device. Use a dual-app (A/B) layout —
two app slots plus a small bootloader: the app receives a new image over BLE, writes
it to the *inactive* slot, then resets, and the bootloader boots the newer valid
slot. Scope includes the bootloader, the BLE update service in the app, integrity
checks so a bad or interrupted update can't brick the device, and keeping user
presets through an update.

**Acceptance Criteria**
- Device accepts a firmware image over BLE and writes it to the inactive slot; the
  running slot is never overwritten.
- After a successful update the device reboots into the new version.
- An interrupted or corrupted update never bricks the device — it keeps running the
  current firmware.
- A downgrade/old image does not silently take over.
- User presets/settings survive an update.
- Works with both CySmart and the mobile app.

**Definition of Done**
- Bootloader + app changes merged and tagged as a release.
- Verified end-to-end on hardware (full update, interrupt, corrupt, downgrade,
  preset-survival).
- SETUP/architecture docs updated.

**Test Cases**
1. Flash a device, note its version (P0.5 log / DIS).
2. Run an OTA from CySmart → confirm it reboots into the new version.
3. Start an update and **disconnect mid-transfer** → device should still boot the
   old slot (no brick).
4. Save a preset, run an update → preset is still there afterward.
5. Try sending an older/wrong image → device stays on current firmware.

**Dependencies:** BDPD-5 (bootloader), BDPD-6 (app DFU). Pairs with BDPD-2 (mobile app).

---

## BDPD-5 — DF: Add bootloader

**Description**
Add a small CM0+ bootloader that runs on every reset, checks both app slots'
metadata (magic + version + CRC), and boots the newest valid one — falling back to
the other if one is bad. Keep it minimal with no BLE (the app handles the actual
update). Define the A/B memory map and slot-selection logic, with debug output on P0.5.

**Acceptance Criteria**
- Runs on every reset and hands off to an app slot.
- Picks the valid slot with the highest version; falls back to the other valid slot.
- Rejects a slot with bad magic or failing CRC; if neither slot is valid, behaves safely.
- Vector-table/stack handoff into the chosen app works (app runs normally, BLE comes up).
- Prints its decision on the P0.5 debug UART.

**Definition of Done**
- Bootloader builds and programs via MiniProg/KitProg.
- Boots App0 and the app runs normally.
- Slot selection + fallback verified; committed.

**Test Cases**
1. Program bootloader + App0; on P0.5 watch for `select App0` → `jump -> 0x10010000`.
2. Program both slots with different versions → confirm it boots the higher one
   (`select App1 (newer)`).
3. Corrupt one slot's metadata → confirm it falls back to the other slot.
4. Confirm the app's BLE advertises after the jump.

**Dependencies:** part of BDPD-1.

---

## BDPD-6 — DF: Configure PainDrain app for DFU updates

**Description**
Add the DFU engine and the Bootloader (BTS) BLE service to the PainDrain app so it
can receive firmware over BLE from CySmart and the mobile app. It should write the
incoming image to the inactive slot (protecting the running slot), report which slot
it's running from via the DIS firmware-revision string so the app knows which file
to send, and reset into the bootloader when done. Also raise the BLE MTU to keep
update time reasonable, and add a CRC integrity check plus preset preservation.

**Acceptance Criteria**
- App advertises the Bootloader (BTS) service; a DFU host can connect and transfer.
- Writes only the inactive slot; refuses writes to the running slot.
- DIS Firmware Revision reads `version/slot` for the running image.
- Device resets into the bootloader on completion and boots the new slot.
- BLE MTU negotiated high enough that a full update is ~1–2 min, not ~20.
- Image CRC is stored and checked; presets (work-flash) are not overwritten by an update.

**Definition of Done**
- App builds with the DFU service + engine; OTA works via CySmart end-to-end.
- DIS slot string verified on both slots; committed/tagged.

**Test Cases**
1. In CySmart, connect (no pairing) → confirm the Bootloader service is present.
2. Send the inactive slot's `.cyacd2` → device writes it, reboots, runs the new slot.
3. Read DIS Firmware Revision → matches the running version/slot.
4. Send the *wrong* (running) slot's file → device rejects it early, stays on current firmware.
5. Confirm the negotiated MTU is the high value and the transfer is fast.

**Dependencies:** BDPD-5.

---

## BDPD-2 — Mobile App: OTA Updates

**Description**
On the app side: connect to the device (no pairing), read the running slot from the
DIS firmware-revision string (`version/slot`), and upload the *inactive* slot's
`.cyacd2` from the release bundle using the Cypress DFU protocol — with progress and
success/failure handling. Firmware side is BDPD-1.

**Acceptance Criteria**
- App connects without requiring pairing/bonding.
- Reads the running slot from DIS and selects the correct (inactive) `.cyacd2` from
  the bundle.
- Streams the update via the Cypress DFU protocol with a progress indicator.
- Reports success (and the new version) or a clear failure; lets the user retry.
- Won't push an equal/older version (no-op) — checks version before updating.

**Definition of Done**
- App updates a real device end-to-end; progress + success/failure states
  implemented; tested against firmware.

**Test Cases**
1. Run an update from the app on a real device → device updates and reboots to the
   new version.
2. Confirm the app shows progress and a success state with the new version (re-read DIS).
3. Test a dropped connection / failure → app shows a clear, retryable error; device
   stays safe.
4. Try "update" with the same version already installed → app treats it as up-to-date.

**Dependencies:** BDPD-1 (firmware OTA).

---

## BDPD-4 — Device Firmware: Battery Level

**Description**
Read the real charge level from the fuel gauge and report it over the standard BLE
**Battery Service (BAS)** — the `0x180F` service / Battery Level (`0x2A19`)
characteristic — so the mobile app (or any BLE client) can read and display the
device's battery. This is the standard, app-readable way the device surfaces its
battery state.

**Acceptance Criteria**
- Device reads an accurate battery level from the fuel gauge.
- The level is reported over the standard BLE **Battery Service** (Battery Level
  characteristic), updated regularly, with notifications to subscribed clients.
- A phone / BLE client can read and subscribe to the real value.

**Definition of Done**
- Fuel-gauge read + BLE **Battery Service** reporting the real level (with
  notifications), verified on hardware against the actual battery.

**Test Cases**
- In CySmart, read **Battery Service → Battery Level (`0x2A19`)** and confirm it
  matches the device's actual charge; subscribe and confirm it updates.

**Dependencies:** BDPD-7 (read the gauge), BDPD-8 (report it over the Battery
Service). Pairs with BDPD-3 (mobile app).

---

## BDPD-7 — DF: Calculate battery level (%, mV) from fuel gauge

**Description**
Read state-of-charge (%) and voltage (mV) from the bq28Z610 fuel gauge over I2C,
along with charging status, to feed the debug log and the BLE battery service.

**Acceptance Criteria**
- Reads SoC (%), voltage (mV), and charging status from the bq28Z610.
- Values are sane and track the real battery (charging/discharging).

**Definition of Done**
- Fuel-gauge reads implemented and shown in the debug log; values verified against reality.

**Test Cases**
1. On P0.5, confirm a periodic `Battery: N% Xmv <status>` line.
2. Compare the % and mV to a known-good reference (multimeter / charger state).
3. Plug/unplug the charger → status flips correctly.

**Dependencies:** feeds BDPD-8.

---

## BDPD-8 — DF: Configure Bluetooth battery level service

**Description**
Push the real fuel-gauge percentage into the standard BLE Battery Service so a phone
reads the actual level. Update the characteristic regularly and notify subscribed
clients.

**Acceptance Criteria**
- Battery Level characteristic returns the real fuel-gauge % (not a placeholder/demo value).
- Value updates on a regular cadence and matches the debug-log reading.
- Subscribed clients get notifications when it changes.

**Definition of Done**
- Battery Service reports the real SoC, updates periodically, and notifies; verified
  on hardware.

**Test Cases**
1. In CySmart, read Battery Level (`0x2A19`) → matches the P0.5 `Battery:` value and
   the actual charge.
2. Read it several times → it should be **stable** (not change/decrement just from reading).
3. Subscribe to notifications → confirm periodic updates.

**Dependencies:** BDPD-7.

---

## BDPD-3 — Mobile App: Battery Level Indicator

**Description**
Show the device's battery level in the app by reading the standard BLE Battery
Service and subscribing for live updates. Firmware side is BDPD-4/BDPD-8.

**Acceptance Criteria**
- App reads and displays the device's battery % from the Battery Service.
- Subscribes for notifications so the displayed value updates live.
- Displayed value matches the device's actual level.

**Definition of Done**
- Battery indicator shows in the app, matches the device, and updates live; tested
  against firmware.

**Test Cases**
1. Connect the app to a device → battery % shows and matches the device/CySmart reading.
2. Let it run / change charge → the indicator updates without reconnecting.

**Dependencies:** BDPD-4 / BDPD-8 (firmware).
