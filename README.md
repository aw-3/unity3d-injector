## Unity3D-inject

### Overview

Unity3d-inject is a Mono C# assembly injector designed for use on Unity3D games and applications.

### Features
- **Completely undetectable** from within the mono application
- Supported on GNU/Linux & ~~Windows~~ (Coming soon)
- Can inject into any mono embedded application
- Stable

### Compiling

#### Linux

```
git clone --recursive https://github.com/aw-3/unity3d-injector.git
cd unity3d-injector-master
cmake .
make
```

#### Windows

TODO

### Basic usage

```c
// With PID
$ ./inject --pid 123 --namespace my_namespace my_assembly.dll

// With process name
$ ./inject --process my_process_name --namespace my_namespace my_assembly.dll
```

### Notes

By default, unity3d-inject will assume the default class and method as
"Loader" and "Load" respectively. If needed, you can change it in the main header file
and recompile as desired.

It is required to change the assembly version each build if you wish to reinject multiple times per session.
You may do this by adding `[assembly: AssemblyVersion("1.0.*")]` to your AssemblyInfo.cs

### Building your own assembly

TODO

### Sample loader class

```csharp
using System;
using UnityEngine;

namespace SampleAssembly
{
    public class Loader
    {
        private static GameObject _mainClass;

        public static void Load()
        {
            _mainClass = new GameObject();
            _mainClass.AddComponent<MainClass>();

            UnityEngine.Object.DontDestroyOnLoad(_mainClass);
        }
    }

    // .. Different file

    class MainClass : MonoBehavior
    {
        // Implement as desired
    }
}
```

### License

This project is licensed under BSD-2. For more information see LICENSE file.