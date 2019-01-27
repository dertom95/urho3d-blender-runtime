#/bin/bash

if [[ -z "${URHO3D_SOURCE}" ]]; then
        echo "You need to set the env-variable URHO3D_SOURCE to point to URHO3D-Source-Folder"
else
        echo "WIRE SYMBOLIC-Links to base: ${URHO3D_SOURCE}"
        rm CMake
        rm Rakefile
        rm script
        ln -s $URHO3D_SOURCE/CMake
        ln -s $URHO3D_SOURCE/Rakefile
        ln -s $URHO3D_SOURCE/script
fi
