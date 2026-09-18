# ResourcePack Reference (RPR)

> **This is the first release version. The standalone toolset is not yet complete; related tools will be added in future updates, and this sentence will be removed.**

- [README 中文](README.md)
- [README English](README_EN.md)
- [README TokiPona](README_TP.md)

![icon](resource/icon.png) A static analysis toolset for Minecraft: Bedrock Edition resource packs.

`rpr.exe` is the main program (the actual execution backend). The other programs are standalone tools that translate arguments and invoke `rpr.exe`.

Some tools depend on `.rpi` (ResourcePack Index) files. It is recommended to run `rpi_index` first to generate the index file.

## Download and Usage

- **Direct Download**: [rpref.zip](https://github.com/SusumaarT/RP-Reference/releases/latest/download/rpref.zip)
- **Usage**:

    ```batch
    rpr -mcp              :: Start the MCP server
    rpr -tool=<tool_name> :: Run the specified tool
    rpr -info=<tool_name> :: Get information about the specified tool
    rpr -tools            :: List all available tools
    ```

## Tool List

> The tool names in this list are MCP server tool names and are not necessarily the same as the standalone tool names.

- `ac_to_mmd`: Converts **animation controllers** into Mermaid state diagrams or flowcharts, making it easier to inspect state machine logic.
- `rpi_index`: Scans a resource pack directory and generates a **.rpi** index file.
- `rpi_find`: Searches **.rpi** files for components or files by keyword.
- `sn_map`: Builds short-name mappings for entities or resources, helping to understand naming and reference relationships.
- `bone_coverage`: Checks the bone-set relationship between geometries and animation controllers.
- `geo_tree`: Displays the hierarchy and parent-child relationships of geometry.
- `br_extract`: Extracts **.brarchive** archive files and pulls out the resource contents.
- `br_zip`: Compresses resource contents into **.brarchive** archive files.
- `br_merge`: Merges multiple **.brarchive** files into a new archive.
- `br_auto`: Automatically performs resource pack extraction or archiving operations.
- `dir_compo_rename`: Batch-renames component directories according to JSON key-value conditions within the directory, with support for custom naming formats.
- `rpi_compo_rename`: Batch-renames components based on the resource pack index, suitable for unifying naming conventions.
- `unmerge`: Splits merged JSON objects or arrays back into multiple component fragments.
- `merged`: Merges multiple component JSON files into a single overall structure according to specified paths.
- `mat_forest`: Analyzes material tree structures and dependencies.

> Run a standalone program without arguments to get usage instructions.

## Building from Source

> The project has only been tested in a Windows environment (invoking `mingw32-make` via CMD).

1. Clone this repository.
2. Make sure you have the GNU GCC compiler suite installed.
    > (Preferably with the Tiny C Compiler (TCC); you can also change it to gcc in `Makefile.win`.)
3. Open a CMD command prompt and enter the project root directory.
4. Run the existing scripts in the root directory:
    1. `clean.bat`: Clean the build directory.
    2. `debug.bat`: Compile in debug mode.
    3. `release.bat`: Compile in release mode.
5. Get the executable files in the `bin` directory.

## License

This project is open source under the GNU AGPL v3.0 license. For details, please refer to the [LICENSE](LICENSE) file.
