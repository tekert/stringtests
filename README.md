# Basic Split String tests

### Prerequisites: 

* Google Benchmark: https://github.com/google/benchmark
* Vcpkg: https://github.com/microsoft/vcpkg

### Install Prerequisites:
Windows
```cmd
vcpkg install benchmark --triplet x64-windows-static-md
```
(Optional)
```cmd
vcpkg install boost-tokenizer --triplet x64-windows-static-md
vcpkg install boost-utility --triplet x64-windows-static-md
```

Linux
```cmd
vcpkg install benchmark
```
(Optional)
```cmd
vcpkg install boost-tokenizer
vcpkg install boost-utility
```
### Compile

Point VCPKG_ROOT env variable to where vcpkg is installed (root) (Windows and Linux)

Alternative method is to set it inside `CMakePresets.json`
          
```json
    "environment":
    {
        "VCPKG_ROOT": "PATH"
    }
```
replace `PATH`
one for each OS base preset

