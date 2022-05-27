# Setup guide

## Qt Framework

1. Navigate to the [Qt download page](https://www.qt.io/download)
2. Locate and click on the `Go open source` button
3. Scroll down until you see the `Looking for Qt binaries?` header, just above FAQ and download the Online Installer
4. Before opening it, create a Qt account and confirm your email address
5. Run the installer and sign in with your account
6. Agree to the terms and conditions and confirm that you're an individual
7. Select Custom Installation
8. Check the following packages in the `Qt` category:
   - `Qt 6.2.3 > MinGW 11.2.0 64-bit`
   - `Qt 6.2.3 > MSVC 2019 64-bit` (optional for Visual Studio)
   - `Developer and Designer Tools > MinGW 11.2.0 64-bit` (skip that if you plan to use Visual Studio)
9. There is a chance that the project is still using `Qt 5.15.2`. If that is the case pick the same options for that version, but not the different MinGW version required
10. Proceed with the installation

## Git

Skip this part if you already have it.

1. Navigate to [Git website](https://git-scm.com/)
2. Download the standalone Windows installer
3. Install with default settings or adjust to your preferences

## Boost

Boost is a collection of C++ libraries that extend the standard library with many useful utility functions. Follow these steps to set it up:

1. Navigate to the [Boost download page](https://www.boost.org/users/download/)
2. Download the source code for Windows
3. Extract the archive and choose a location where you want to keep the source files, for example `C:\boost`

## Building the application

1. Navigate to a directory where you want your project files to be located
2. Hold `Ctrl+Shift`, right click and select `Git Bash here`
3. Run the command below:

```
git clone https://github.com/tkazimierczuk-fuw/pleview
```

4. Open Qt Creator, you can check out their tutorials and examples to get familiar with this IDE
5. Select the `Projects` tab and click on the `Open` button
6. Navigate to where you cloned the repository and select the `CMakeLists.txt` file
7. Select the appropriate Qt version and let the IDE configure the project
8. Inside the `CMakeLists.txt` file, provide the path to your Boost installation on line 30
9. In the bottom left, click on `Debug` and change it to `Release`
10. Verify that the project builds succesfully, by running it with `Ctrl+R`
