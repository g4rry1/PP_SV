# PP_SV - SystemVerilog Pretty Printer

PP_SV is a pretty printer designed to format SystemVerilog code according to a unified styling standard. It operates through syntax analysis using the Slang library. By constructing a Concrete Syntax Tree (CST), PP_SV completely reconstructs the code, ensuring readability and consistent style throughout the entire codebase.

## 🚀 Installation and Build

### 📥 Cloning the Project

The project uses git submodules for dependencies. To clone it correctly, run:

# Cloning with automatic submodule initialization
```bash
git clone --recurse-submodules https://github.com/g4rry1/PP_SV.git
```

If you've already cloned the project without submodules:

# Initializing and updating submodules
```bash
git submodule update --init --recursive
```

## 🔨 Building the Project

### Creating a Build Directory
```bash
mkdir build && cd build
cmake ..
cmake --build . (or make)
```

### ✅ Verification

After successful build, run:
```bash
./my_project [path to SystemVerilog file]
```

## Testing
### The project uses the "Google Test" framework for testing.
If you don't have it installed, run:
```bash
sudo apt-get install libgtest-dev
```

### Running Tests
```bash
cd build
make check
```

## 📚 Used Submodules

The project uses the following libraries as submodules:

| Library | Purpose |
|---------|---------|
| [slang](https://github.com/MikePopoloski/slang.git) | SystemVerilog processing library |

#### 🔄 Updating Submodules

To update dependencies to the latest versions:
```bash
git submodule update --remote --recursive
```

## Troubleshooting

If you encounter build errors related to dependencies:

### Complete Submodule Reinitialization:
```bash
git submodule deinit --all
git submodule update --init --recursive
```
