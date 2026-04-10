#!/usr/bin/env python
import os

from scripts.utils import cli
# noinspection PyUnresolvedReferences
import scripts.configure
# noinspection PyUnresolvedReferences
import scripts.dependencies
# noinspection PyUnresolvedReferences
import scripts.workflow

if os.path.exists('./scripts/post_build.py'):
    # noinspection PyUnresolvedReferences
    import scripts.post_build

if __name__ == '__main__':
    cli()
