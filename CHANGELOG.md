# Changelog for OTP-20 oBank
## [v4.0.0](https://github.com/SebastianOberschwendtner/OTP20_oBank/releases/tag/v4.0.0) *(2023-xx-xx)*

>Released by `SO`

### Release Notes:
#### :bookmark: `v3.1.0`
- Major update of IPC messaging within the tasks.
- Activates the *ALRT* pin of the BMS chip to prevent battery damage. Fixes #7.
- Led indicate the actual output state not just the assumed one.

### Fixed Issues:

- #6: Cell balancing not working *(Not a direct software issue)*
- #7: Add automatic shutoff when battery is depleted

## [v3.0.0](https://github.com/SebastianOberschwendtner/OTP20_oBank/releases/tag/v3.0.0) *(2023-05-07)*

>Released by `SO`

### Release Notes:
#### :bookmark: `v2.1.0`
- Adds **ETL** state machine to system task.
- Adds **ETL** state machine to display task.
- Adds further BMS parameters to the GUI.
#### :bookmark: `v3.0.0`
- Activates the PD controller.
- Refactors the interprocess communication and adds command queues using **ETL**.

### Fixed Issues:

- n/a

## [v2.0.0](https://github.com/SebastianOberschwendtner/OTP20_oBank/releases/tag/v2.0.0) *(2022-06-06)*

>Released by `SO`

### Release Notes:
- Update OTOS version to `v2.7.4`

### Fixed Issues:

- n/a

## [v1.0.0](https://github.com/SebastianOberschwendtner/OTP20_oBank/releases/tag/v1.0.0) *(2021-10-10)*

>Released by `SO`

### Release Notes:
- Initial release of *oBank*.

> This is more or less a pre-release to get started with the releases.

### Fixed Issues:

- n/a
