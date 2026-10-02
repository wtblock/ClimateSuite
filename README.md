# Climate Suite

Climate Suite transforms more than **3,655 raw USHCN station files** into a fast, portable relational database that makes **125 years of climate history** instantly accessible. A single ZIP download is all it takes to begin exploring national, state, and local climate trends through **Climate Explorer**, a visual tool designed to make climate history easy to understand, analyze, and share.

Climate Explorer includes a rich set of tools that make climate history easy to analyze and present. You can graph temperature trends at national, state, and local levels, explore interactive maps showing station locations, import images and Markdown files into your documents, and export your work as high‑resolution images or Kindle‑ready PDFs.

## Quick Start

Getting started is simple: download the Climate Suite ZIP file, extract it anywhere, and launch Climate Explorer (`ce.cmd`). The database is included, so you can begin exploring climate history immediately—no setup, no configuration, and no additional downloads required.

[![Video Thumbnail](https://img.youtube.com/vi/7SYEbimxDUs/0.jpg)](https://youtu.be/7SYEbimxDUs)

### Running Unsigned Builds

The Climate Suite executables are currently **not code‑signed**.  
When you launch the application for the first time, Windows may show a SmartScreen warning.

This is normal for independent open‑source software. I am giving away three months of my time, and it does not make sense to pay one to two hundred dollars a year for signing certificate. As long as you download the software from this site, you know it came from me.

To continue:
1. Click **More info**
2. Click **Run anyway**

After this, the application will start normally.

## What You Can Do With Climate Explorer

Climate Explorer lets you graph temperature trends, view interactive maps of station locations, create documents that combine images, graphs, and Markdown, and export your work as high‑resolution images or Kindle‑ready PDFs. Whether you want to study long‑term climate behavior or publish your own findings, the tools you need are built directly into the application.

## Video Demonstration

A short YouTube video provides a quick walkthrough of Climate Explorer, showing how to navigate the database, view maps, generate graphs, and create documents. It’s the fastest way to see the core features in action before exploring on your own.

[![Video Thumbnail](https://img.youtube.com/vi/AdCsyUhw8uc/0.jpg)](https://youtu.be/AdCsyUhw8uc)

## Learn More

The sections below provide a brief overview of the workspace layout, optional command‑line tools, and the structure of the included database. These details are available for users who want to explore the internals of Climate Suite or rebuild the dataset, but they are not required for normal use.

## Workspace Model

Climate Suite uses a self‑contained workspace design: everything you need is included in the ZIP file, and all tools run directly from the extracted folder. There is no installation process, no registry changes, and no system configuration required. This makes the suite fully portable—easy to move, back up, or place on any drive.

## Folder Structure

After extracting the ZIP file, the Climate Suite workspace looks like this:

```text
Climate/
    bin/
    Books/
    cache/
    Images/
    Markdown/
    USHCN/
    ClimateUSHCN.db
    ushcn-v2.5-stations.txt
    ce.cmd
    ImportUSHCN.cmd
    QueryUSHCN.cmd
    lc.cmd
    OpenCE.cmd
    SynchClimate.cmd
    UpdateBIN.cmd
```

This layout keeps everything self‑contained and portable, with executables, documents, images, and the database organized in a simple, predictable structure. 

Portability comes from the fact that Climate Suite never relies on absolute paths. All images, documents, and database files are referenced using simple relative paths inside the workspace (for example, `.\Images\Climate.png`). This means the entire folder can be moved to any drive or directory without breaking links, configuration, or functionality. As long as the workspace stays together, everything works. 

Climate Explorer is launched from the root folder using `ce.cmd` rather than by running `ClimateExplorer.exe` directly from the `bin` folder. This ensures all relative paths resolve correctly. The application expects its workspace to be the current directory, so images, documents, and database files are located using paths like `.\Images\Climate.png` instead of absolute locations. Launching through `ce.cmd` guarantees the correct working directory and preserves full portability.

## Optional Tools

For users who want to explore the internals of the dataset or automate parts of their workflow, Climate Suite includes several optional command‑line tools. These utilities allow you to rebuild the database, query raw USHCN files, synchronize workspaces, and perform other advanced operations. They are entirely optional—Climate Explorer works fully without them.

## License

Climate Suite is released under the MIT License, giving you broad freedom to use, modify, and distribute the software. This permissive license is intended to support open access to climate data and encourage further research, analysis, and tool development.

## Open‑Source Acknowledgments

Climate Suite relies on a small number of high‑quality open‑source libraries. Markdown support is provided by **md4c**, a fast and compliant CommonMark parser. ZIP compression and extraction use **miniz**, a lightweight single‑file library. All climate data is stored in **SQLite**, a compact and reliable embedded database engine. Mapping is implemented using custom rendering code built on top of **OpenStreetMap** tiles. PDF generation and document rendering are implemented entirely within Climate Suite using custom code. We gratefully acknowledge the open‑source projects that make this work possible.

