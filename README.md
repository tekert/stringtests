# Basic Split String tests

Prerequisites: 

* Google Benchmark: https://github.com/google/benchmark
* Vcpkg: https://github.com/microsoft/vcpkg

## Install:
```cmd
vcpkg install benchmark --clean-after-build --triplet x64-windows-static-md
```
# Quick Start

Point VCPKG_ROOT env variable to where vcpkg is installed (root)

Can be done inside `CMakePresets.json`
          
```json
    "environment":
    {
        "VCPKG_ROOT": "PATH"
    }
```
replace `PATH`

