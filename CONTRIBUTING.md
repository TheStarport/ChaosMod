# How to contribute

## Git

When making commits please follow [this guide](https://dev.to/chrissiemhrk/git-commit-message-5e21).
For a rough summary, all commit messages should be in this format: `$type: $subject (#ID)`.

The type should accurately inform what kind of change is being made: 
- feat for feature
- fix for bug fix
- style for anything visual
- cicd for anything relating to workflows
- chore for something of little consequence or hard to notice. QoL.
- etc, see previous commit messages for examples

The subject should be clear and concise and accurately describe what has been changed.
The subject should begin as lower case.
It should be no more than 50 characters unless absolutely necessary.

If you are referencing a pull request or issue, you should suffix your commit message with #id to link it within Git.

Example: `feat: added 'Double Down' effect that doubles the potency of the last patch (#79)`

More examples can be viewed quickly by looking at the releases.
If the commit style guidelines are not followed inside of pull requests, 
the commits will be squashed and renamed in order to conform.

## Coding conventions

We use ClangFormat to keep all the code to a consistent standard, and the tool should be run with the provided 
'.clang-format' file before any PRs are made.

However, here is a summary of the most common requirements: 

* All line endings are CRLF
* We use the '[Allman](https://en.wikipedia.org/wiki/Indentation_style#Allman_style)' indentation style
* We indent using four spaces (soft tabs)
* Class/Struct fields are camalCase
* All functions are PascalCase
* Macros and Constants are PascalCase
* Variables are camalCase