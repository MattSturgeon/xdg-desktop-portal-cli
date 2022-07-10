# Try to install the git pre-commit hook
# Setup pre-commit hook for ensuring coding style is always consistent
message('Installing git hooks...')
result = run_command(find_program('python3'), files('install-hooks.py'), check: false)
code = result.returncode()
stdout = result.stdout().strip()
stderr = result.stderr().strip()
if stdout != ''
    message('\n@0@\n'.format(stdout))
endif
if stderr != ''
    warning('\n@0@\n'.format(stderr))
endif
if code != 0
    warning('Error installing git hooks (@0@)'.format(code))
endif