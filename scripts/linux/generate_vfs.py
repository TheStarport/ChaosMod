import os
import yaml


def _build_vfs(root_real_path: str, root_vfs_path: str) -> dict:
    def build_vfs_recurse(real_dir_path: str, vfs_dir_obj: dict):
        for file_name in os.listdir(real_dir_path):
            file_path = os.path.join(real_dir_path, file_name)

            if os.path.isdir(file_path):
                file_vfs = {
                    'type': 'directory',
                    'name': file_name,
                    'contents': []
                }

                build_vfs_recurse(file_path, file_vfs)
            else:
                file_vfs = {
                    'type': 'file',
                    'name': file_name,
                    'external-contents': file_path
                }

            vfs_dir_obj['contents'].append(file_vfs)

    root_vfs_dir = {
        'type': 'directory',
        'name': root_vfs_path,
        'contents': []
    }

    build_vfs_recurse(root_real_path, root_vfs_dir)

    return root_vfs_dir

def build_vfs(msvc: str, kit: str, out_file: str):
    vfs = {
        'version': 0,
        'case-sensitive': False,
        'roots': [
            _build_vfs(msvc, '/winsys/msvc'),
            _build_vfs(kit, '/winsys/sdk'),
        ]
    }

    with open(out_file, 'w', encoding='utf-8') as f:
        yaml.dump(vfs, f)
