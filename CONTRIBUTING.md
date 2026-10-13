# Contributing to instr-driver-advantest-pa-r3263-cvi

Contributions to the Advantest PA series LabWindows/CVI driver are welcome from all!

instr-driver-advantest-pa-r3263-cvi is managed via [git](https://git-scm.com), with the canonical upstream
repository hosted on [GitHub](https://github.com/ASingh-NI/instr-driver-advantest-pa-r3263-cvi/).

instr-driver-advantest-pa-r3263-cvi follows a pull-request model for development. To contribute, create a
GitHub account, fork this project, push a branch with your changes to your fork, and
then submit a pull request.

Please sign off your commits (e.g. `git commit -s`). This adds a line of the form
`Signed-off-by: Name Lastname <name.lastname@example.com>` to the commit message,
indicating that you have read and agree to the Developer Certificate of Origin (see
below). Include a `Signed-off-by` line for every author of a commit.

See [GitHub's documentation](https://docs.github.com/pull-requests) for more details.

# Getting Started

1. Install the prerequisites:
   - LabWindows/CVI.
   - [NI-VISA](https://www.ni.com/en/support/downloads/drivers/download.ni-visa.html)
     and NI MAX.
   - [Uncrustify](https://github.com/uncrustify/uncrustify) for source formatting.
2. Fork this repository and clone your fork.
3. Create a topic branch, e.g. `git checkout -b users/<name>/avpa-<change>`.
4. Open `avpa/avpa.prj` in LabWindows/CVI and build it for 32-bit and 64-bit to confirm a
   clean starting point.

# Making Changes

- Follow the VXIplug&play (VPP-3.x) instrument driver conventions.
- Keep the existing `AVPA_` prefix and naming style for new exported functions
  (`AVPA_Set_*` to configure, `AVPA_Read_*` to query).
- Add instrument commands to the command arrays at the top of `avpa.c` rather than
  embedding strings in function bodies.
- Use VISA data types, declare private helpers and globals `static`, validate every input
  parameter, use bounded string operations, and check every VISA return status.
- Keep RS-232 in mind: if a new function cannot work over the serial link (as with
  trace readback), return a clear error and document it under Known Issues.
- Update `avpa.fp` for every new or changed function, documenting valid values, ranges,
  and defaults, and update `avpa.doc`.
- Update the `avpau` example application if you add user-visible functionality.
- Preserve backwards compatibility: do not rename functions or parameters and do not
  reorder parameter lists.
- Update `AVPA_REVISION`, the copyright, and the modification history at the top of
  `avpa.c`, and the README (supported models, interfaces, Known Issues).
- Run Uncrustify on modified `.c`/`.h` files.

# Testing

Summarize your results in the pull request: analyzer model (R3263, R3272, R3463, or
R3465), firmware revision, interfaces tested, and CVI version.

- **Build:** Build `avpa.prj` for 32-bit and 64-bit with no errors or warnings.
- **Function panels:** Run `AVPA_init` from its function panel with the analyzer's VISA
  resource name, then run each new and modified panel and confirm `VI_SUCCESS`.
- **Hardware:** Test new and existing functions against a real PA-series analyzer.
- **Interfaces:** Test over GPIB and over RS-232. Confirm functions that are not
  available over RS-232 fail cleanly.
- **Error handling:** Confirm invalid and out-of-range values return the right error
  and that instrument errors are reported by `AVPA_errorQuery`.
- **Example:** Build and run the `avpau` application against the analyzer.

# Developer Certificate of Origin (DCO)

   Developer's Certificate of Origin 1.1

   By making a contribution to this project, I certify that:

   (a) The contribution was created in whole or in part by me and I
       have the right to submit it under the open source license
       indicated in the file; or

   (b) The contribution is based upon previous work that, to the best
       of my knowledge, is covered under an appropriate open source
       license and I have the right under that license to submit that
       work with modifications, whether created in whole or in part
       by me, under the same open source license (unless I am
       permitted to submit under a different license), as indicated
       in the file; or

   (c) The contribution was provided directly to me by some other
       person who certified (a), (b) or (c) and I have not modified
       it.

   (d) I understand and agree that this project and the contribution
       are public and that a record of the contribution (including all
       personal information I submit with it, including my sign-off) is
       maintained indefinitely and may be redistributed consistent with
       this project or the open source license(s) involved.

(taken from [developercertificate.org](https://developercertificate.org/))

See [LICENSE](https://github.com/ASingh-NI/instr-driver-advantest-pa-r3263-cvi/blob/main/LICENSE.md) for details about how instr-driver-advantest-pa-r3263-cvi is
licensed.
