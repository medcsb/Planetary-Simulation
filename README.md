# C-OpenGL-simple-orbit-IGR200
Simple sphere orbit in openGL, for my 3D graphics class at Télécom Paris


# 🌌 Solar System Simulator – IGR200 Edition 🌍☀️

This is a simple solar system simulator created for the IGR200 3D Graphics course at Télécom Paris. It's not that good, for now.

---

## 📦 Installation

I've tried to make this as smooth as possible. All the libraries you’ll need are packed in here, so just grab `cmake` and you’re good to go. Here’s the step-by-step, because who doesn’t love detailed instructions?

### For Linux:
1. Open your terminal.
2. `cd` into the project directory.
3. Run `cmake ./` (yes, directly in this folder!).
4. Follow up with `make`.

⚠️ **WARNING:** Do NOT build this in a separate directory (like a `build/` folder). The shaders and textures are sensitive little things and will only cooperate if they’re exactly where the code expects them to be. Or just move them along with the media folder wherever the executable is.

### For Windows:
You’ve got options!
- **With Visual Studio or VSCode Or anyother IDE:** Open the project, let `cmake` work its magic, and build.
- **With MinGW (or any other compiler):** Use `cmake` to generate a Makefile, then use either `make` or `mingw32-make` to build it.
    ```bash
    cmake -G "MinGW Makefiles" .   # Generate the Makefile for MinGW
    make       # Or "mingw32-make" for MinGW
    ```
    
### For macOS:
I have no clue. Good luck!

---

## 🌞 What’s Happening?

There’s a Sun at the center (holding it down), with the Earth orbiting around it. To complete the trio, we have the Moon doing its best to keep up, orbiting the Earth. And yes, they even spin!
![20](https://github.com/user-attachments/assets/2578a8fa-49c9-430a-bcfa-2a6b8079c943)

Don't let the picture decieve you the spheres have Phong lighting... I think.
![13](https://github.com/user-attachments/assets/21b876ad-87f0-4052-9671-8fb8fe396446)

---

## 🎮 How to Use This Thing

The console will greet you with a string of commands, but here’s the rundown:

- **H**: Show help message (in case you forget these hotkeys)
- **P**: Pause/unpause the simulation
- **F**: Toggle FPS mode (basicaly camera movement with mouse)
- **T**: Enable wireframe mode (see the mesh triangles)
- **Y**: Disable wireframe mode (back to normal spheres)
- **Z/S**: Move forward/backward
- **Q/D**: Move left/right
- **Space/Left Control**: Move up/down
- **UP-Key/DOWN-Key**: Move up/down
- **Left Shift/Left Alt**: Increase/decrease speed
- **ESC**: Bail out of the simulation

---

## 🛠 Code Highlights

The simulation uses OpenGL with textures and shaders, we've got three meshes the sun, the earth and the moon. A `Camera` class handles your the perspective, while the `Mesh` class is in charge of the planetary forms.
Also everything moves.
![Untitled video - Made with Clipchamp](https://github.com/user-attachments/assets/b517b928-ec2d-40b3-a73c-48bd10dbd54e)

---

## 📝 TO DO :

- Clean up the code
- Fix camera gittering
- Improve camera movement
- Add texture for the sky (starts)
- Add more meshes
- Implement real life Equations for the simulation instead of made up numbers (Put everything in the correct place, correct size, correct animation... etc)

Enjoy! 🚀
