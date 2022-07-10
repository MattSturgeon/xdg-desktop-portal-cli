import os
import shutil
from pathlib import Path

# Gets the directory this script file is located in
# (NOT the current working directory)


def get_script_dir():
    p = Path(__file__)
    while (not p.is_dir()):
        p = p.parent
    return p.absolute()

# Installs source into dest, using the given file permission mode


def install(source: Path, dest: Path, mode: int):
    if dest.exists():
        if dest.is_file():
            # If dest is a file (or a symlink to a file), set it's permissions first
            dest.chmod(mode)
        else:
            # If dest exists but isn't a file (or symlink to a file), we can't
            # safely copy to it...
            raise RuntimeError(f'{dest.absolute()} already exists and is not a file.')
    # Open dest and source, then use shutil to copy from source to dest.
    # The with keyword will close the files for us
    with open(os.open(dest, os.O_CREAT | os.O_WRONLY, mode), 'w') as fdest:
        with open(source, 'r') as fsource:
            shutil.copyfileobj(fsource, fdest)


def main():
    # Find relevent directories
    hooks_dir = get_script_dir()
    sources_root = Path(os.environ.get(
        'MESON_SOURCE_ROOT',
        hooks_dir.parent.absolute())
    )
    installed_hooks = sources_root.joinpath('.git', 'hooks')

    # iterate over every .hook file and install it
    count = 0
    for hook in hooks_dir.glob('*.hook'):
        if hook.is_file():
            dest = installed_hooks.joinpath(hook.name.removesuffix('.hook'))
            try:
                install(hook, dest, 0o755)
            except BaseException as e:
                print(f'Failed to install {hook.name} to {dest}')
                print(e)
                exit(1)
            else:
                print(f'Installed {hook.name} to {dest}')
                count += 1

    # Print a summary of what was installed
    if count > 0:
        s = 's' if count > 1 else ''
        print(f'Successfully installed {count} hook{s}')
    else:
        print('No hooks found')


if __name__ == "__main__":
    main()
