import sys
import time

import argparse
import os
import random
import requests
import string
from datetime import datetime


def random_letters(length):
    letters = string.ascii_lowercase
    return ''.join(random.choice(letters) for _ in range(length))


def get_release_notes(date: str, token: str | None = None) -> (str, str):
    target_date = time.mktime(datetime.strptime(date, "%Y/%m/%d").timetuple())

    headers = {
        "Accept": "application/vnd.github+json"
    }

    if token is not None:
        headers["Authorization"] = "Bearer " + token

    r = requests.get('https://api.github.com/repos/TheStarport/ChaosMod/releases', headers=headers)

    print("Status Code: " + str(r.status_code))

    releases = r.json()

    body = ""
    url = ""

    # Iterate over releases in reverse order
    for release in releases[::-1]:
        created_at = release["created_at"]

        if time.mktime(time.strptime(created_at, '%Y-%m-%dT%H:%M:%SZ')) < target_date:
            continue

        url = release["html_url"]
        body = body + release["body"]

    if len(body) == 0:
        raise ValueError("Invalid date passed in or no releases have been created in the specified time frame.")

    env_file = os.getenv('GITHUB_ENV')
    if env_file is not None:
        with open(env_file, "a") as env:
            eof = random_letters(12)
            env.write(f"URL<<{eof}\n")
            env.write(f"{url}\n{eof}\n")

        with open(env_file, "a") as env:
            eof = random_letters(12)
            env.write(f"BODY<<{eof}\n")
            env.write(f"{body}\n{eof}\n")

    return url, body
