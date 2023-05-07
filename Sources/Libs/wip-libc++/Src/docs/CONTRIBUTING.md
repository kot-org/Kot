# Contributing Guide

Welcome! We love receiving contributions from the community, so thanks for stopping by! There are many ways to contribute, including submitting bug reports, improving documentation, submitting feature requests, reviewing new submissions, or contributing code that can be incorporated into the project.

This document describes the Embedded Artistry development process. Following these guidelines shows that you respect the time and effort of the developers managing this project. In return, you will be shown respect in addressing your issue, reviewing your changes, and incorporating your contributions.

**Table of Contents:**

1. [Code of Conduct](#code-of-conduct)
2. [Important Resources](#important-resources)
3. [Questions](#questions)
7. [Contributing to the Project](#contributing-to-the-project)
    2. [Dependencies](#dependencies)
    3. [Development Process](#development-process)
    4. [Building the Project](#building-the-project)
    5. [Adding New Implementations](#adding-new-implementatinos)
    5. [Adding New Tests](#adding-new-tests)
9. [Community](#community)


## Code of Conduct

By participating in this project, you agree to abide by the Embedded Artistry [Code of Conduct](CODE_OF_CONDUCT.md). We expect all contributors to follow the [Code of Conduct](CODE_OF_CONDUCT.md) and to treat fellow humans with respect.

## Important Resources

This project is managed on GitHub:

* [GitHub Project Page](https://github.com/embeddedartistry/libcpp/)
* [GitHub Issue Tracker](https://github.com/embeddedartistry/libcpp/issues)

Questions can also be submitted [on the Embedded Artistry website](https://embeddedartistry.com/contact) and in the [Embedded Artistry Forum](https://embeddedartistry.com/community/embedded-artistry-oss/).

Useful Documentation:

* [Library Documentation][9]
* [cppreference (API notes)](http://en.cppreference.com)

## Questions

Please submit your questions in the following ways:

* Filing a new [GitHub Issue](https://github.com/embeddedartistry/libcpp/issues)
* [Submitting a question on the Embedded Artistry website](https://embeddedartistry.com/contact)
* [Submitting a question on the Embedded Artistry forum](https://embeddedartistry.com/community/embedded-artistry-oss/)
* Asking a question [on Twitter: (mbeddedartistry)](https://twitter.com/mbeddedartistry/).

## New to Open Source?

If you've never worked with an open source project before, please check out our [Open Source Contribution Guide](https://embeddedartistry.com/fieldatlas/open-source-contribution-guide/), which contains information on:

* Resources for people new to open-source development
* How to find something to work on
* How to report issues
* Branching and PR guidelines for our projects
* Style guidelines for our projects

## Contributing to the Project

If you're interested in contributing = to this project, read on! Don't know where to start? Take a look at the [Open Source Contribution Guide](https://embeddedartistry.com/fieldatlas/open-source-contribution-guide/) section for information on finding beginner-friendly issues on our projects.

Please review the following guides to learn about our development process:

* [Open Source Contribution Guide](https://embeddedartistry.com/fieldatlas/open-source-contribution-guide/)
    * Resources for people new to open-source development
    - How to find something to work on
    - How to report issues
    - Branching and PR guidelines for our projects
    - Style guidelines for our projects
* [Embedded Artistry's GitHub Process](https://embeddedartistry.com/fieldatlas/embedded-artistrys-github-process/), which discusses how we handle PRs and integrate changes
* [Source Control Commit Guidelines](https://embeddedartistry.com/fieldatlas/source-control-commit-guidelines/), which describes our guidelines for commit messages

**If you have any questions about this process, please [ask us for help!](#questions)**

### Dependencies

The dependencies for [Embedded Artistry's standardized Meson build system](https://embeddedartistry.com/fieldatlas/embedded-artistrys-standardized-meson-build-system/) are described [on our website](https://embeddedartistry.com/fieldatlas/embedded-artistrys-standardized-meson-build-system/).

In addition to those dependencies, you will need the following for this repository:

* [`git-lfs`](https://git-lfs.github.com) is used to store binary files

### Development Process

`master` contains the latest code for this project, and new versions are tagged nightly.

Please branch from `master` for any new changes. Once you are ready to merge changes, open a pull request. The build server will test and analyze the branch to ensure it can be safely merged.

### Building the Project

This project uses [Embedded Artistry's standardized Meson build system](https://embeddedartistry.com/fieldatlas/embedded-artistrys-standardized-meson-build-system/). Complete instructions can be found [on our website](https://embeddedartistry.com/fieldatlas/embedded-artistrys-standardized-meson-build-system/).

## Community

Anyone interested in active conversation regarding this project should [join the Embedded Artistry community]https://embeddedartistry.com/community/embedded-artistry-oss/).

You can also [reach out on Twitter: mbeddedartistry](https://twitter.com/mbeddedartistry/).
