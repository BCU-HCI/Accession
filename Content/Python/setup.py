from setuptools import setup, find_packages

packages = find_packages(include=["openaccessibility.*"])

setup(
    name="OpenAccessibilityPy",
    description="The Python Component of the OpenAccessibility Plugin For Python.",
    version="0.1.0",
    packages=packages,
    install_requires=[
        "unreal",
        "numpy",
        "faster-whisper",
        "pyzmq",
    ],
)
