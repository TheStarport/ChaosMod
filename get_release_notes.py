import argparse
import os
import sys
import time
import requests
from datetime import datetime
import random, string


def random_letters(length):
    letters = string.ascii_lowercase
    return ''.join(random.choice(letters) for _ in range(length))


parser = argparse.ArgumentParser()
parser.add_argument("-d", "--date", action="store", required=True)
parser.add_argument("-t", "--token", action="store", required=True)
args = parser.parse_args()

target_date = time.mktime(datetime.strptime(args.date, "%Y/%m/%d").timetuple())

r = requests.get('https://api.github.com/repos/TheStarport/ChaosMod/releases',
                 headers={
                     "Accept": "application/vnd.github+json",
                     "Authorization": "Bearer " + args.token
                 })

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
    print("Invalid date passed in or no releases have been created in the specified time frame.")
    sys.exit(1)

env_file = os.getenv('GITHUB_ENV')
with open(env_file, "a") as env:
    eof = random_letters(12)
    env.write(f"URL<<{eof}\n")
    env.write(f"{url}\n{eof}\n")

with open(env_file, "a") as env:
    eof = random_letters(12)
    env.write(f"BODY<<{eof}\n")
    env.write(f"{body}\n{eof}\n")

