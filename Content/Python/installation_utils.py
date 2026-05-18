import unreal
import sys
import re
from os import path


def get_requirements(requirements_dir: str) -> list[str]:
    with open(
        path.join(requirements_dir, "requirements.txt"), "r"
    ) as requirements_file:
        return [line.strip() for line in requirements_file.readlines()]


def is_dependency_satisfied(dependency: str) -> bool:

    py_version = sys.version_info

    if py_version >= (3, 8):
        return _is_dependency_satisfied(dependency)
    else:
        return _is_dependency_satisfied_pkgresources(dependency)


def _is_dependency_satisfied(dependency: str) -> bool:

    match = re.match(r"^([a-zA-Z0-9_\-]+)\s*(.*)", dependency.strip())
    if not match:
        return False

    pkg_name = match.group(1)
    pkg_version = match.group(2)

    try:
        # Check if the Package is Installed
        import importlib.metadata

        installed_version = importlib.metadata.version(pkg_name)

        unreal.log(
            f"|| Accession Python || Dependency '{dependency}' Installed Version: {installed_version} ||"
        )

        # Package is Installed, specific version not required early exit.
        if not pkg_version:
            return True

        # Validates the Version Specifier is Statisfied, if not it will
        has_required = _validate_version_packaging(installed_version, dependency)

        satisfied_str = "Satisfied" if has_required else "Not Satisfied"

        unreal.log(
            f"|| Accession Python || Dependency '{dependency}' is {satisfied_str} ||"
        )

        return has_required
    except:
        return False


def _validate_version_packaging(
    installed_version: str, dependency_specifier: str
) -> bool:
    try:
        # Use Vendored Package, incase not bundled with Standard Python (Will Possibly Change, similar to pkg_resources).
        from pip._vendor.packaging.requirements import Requirement

        req = Requirement(dependency_specifier)

        return installed_version in req.specifier
    except:
        return False


def _is_dependency_satisfied_pkgresources(dependency: str) -> bool:
    import pkg_resources

    try:
        unreal.log(
            f"|| Accession Python || Checking Dependency with pkg_resources: {dependency} ||"
        )

        pkg_resources.require(dependency)

        unreal.log(f"|| Accession Python || Dependency '{dependency}' is Satisfied ||")

        return True
    except:
        return False
