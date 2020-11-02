all : \
    ../dist/proto.pnct \
    ../dist/proto.scene \

../dist/proto.scene : proto.blend export-scene.py
    "C:\Program Files\Blender Foundation\Blender 2.90\blender.exe" --background --python export-scene.py -- "proto.blend:Collection" "../dist/proto.scene"

../dist/proto.pnct : proto.blend export-meshes.py
    "C:\Program Files\Blender Foundation\Blender 2.90\blender.exe" --background --python export-meshes.py -- "proto.blend:Collection" "../dist/proto.pnct" 