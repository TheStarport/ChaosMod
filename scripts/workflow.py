import requests
import json
import click

from .utils import cli, log


@cli.command(name="release-cleanup", short_help="Deletes all releases starting with the string 'nightly-' when there are more than the amount specified")
@click.option("--token", type=str, help="A valid ForgeJo token with repo write access")
@click.option("--repo-uri", type=str, help="The URI for the repo")
@click.option("--num_releases", type=int, help="The number of releases to keep", default=10)
def cleanup_old_releases(token: str, repo_uri: str, num_releases: int):
    request = requests.get(f'https://codeberg.org/api/v1/repos/{repo_uri}/releases?pre-release=true')
    if request.status_code != 200:
        log(f"Unable to fetch resource with response {request.status_code}")

    release_list = []
    for release in json.loads(request.content):
        if release.get("name").startswith("nightly-"):
            release_list.append(release)

    count = 0
    for release in release_list:
        count += 1
        
        if count > num_releases:
            log(f'Deleting release {release.get("id")}')
            delete_url = f'https://codeberg.org/api/v1/repos/{repo_uri}/releases/{release.get("id")}'
            auth_header = {'Authorization': f'{token}'}
            request = requests.delete(delete_url, headers=auth_header)
            if request.status_code != 204:
                log(f"Unable to delete resource with response {request.status_code}")