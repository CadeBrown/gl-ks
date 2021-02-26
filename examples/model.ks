#!/usr/bin/env ks
""" model.ks - basic model loading and displaying

This example uses modern OpenGL (specifically, 3.3), and so it will be
  fairly fast on all hardware. OpenGL 3.3 is a good standard to program for,
  as the more efficient routines are available yet it is widely supported

This tutorial also uses:
  * gl.glfw: GLFW bindings, for window creation and input management
  * gl.assimp

OpenGL uses a global state, which is admittedly not-so-great design. So,
  this code can get a bit hard to understand and it may seem like magic.
  This is how OpenGL is. Typically, you will write your own renderer on top
  of OpenGL, which handles all this state management.

@author: Cade Brown <cade@kscript.org>
"""

# OpenGL bindings
import gl

# NumeriX, for specific datatypes
# This is part of the standard library
import nx

# Math, for some basic functions
# This is part of the standard library
import m

# Time, for some automation
# This is part of the standard library
import time

# Create a basic window from GLFW (part of the 'gl' package)
window = gl.glfw.Window("basic", (640, 480))

# Create a 'gl.Shader' object. This is how we tell the computer to render 
#   objects. The constructor takes the vertex shader source, then the fragment
#   shader source.
#
# SEE: GLSL (OpenGL Shading Language) https://www.khronos.org/opengl/wiki/OpenGL_Shading_Language
shader = gl.Shader("""#version 330 core
layout (location = 0) in vec3 vXYZ;
layout (location = 1) in vec3 vN;
layout (location = 2) in vec2 vUV;

out vec4 fXYZ;
out vec3 fN;
out vec2 fUV;

uniform mat4 uPV;
uniform mat4 uM;

void main() {
    fXYZ = uM * vec4(vXYZ, 1.0);
    fN = vN;
    fUV = vUV;

    gl_Position = uPV * fXYZ;
}
""", """#version 330 core
layout(location = 0) out vec4 oDiffuse;

in vec4 fXYZ;
in vec3 fN;
in vec2 fUV;

void main() {

    vec3 ldir = normalize(vec3(-1, -1, -1));
    float diff = max(0, -dot(ldir, fN));

    oDiffuse = vec4(1.0, 0.5, 0.2, 1.0) * (0.5 + 0.5 * diff);
}
""")

# Use the Assimp bindings (gl.ai) to load 3D model
obj = gl.ai.load('assets/models/suzanne.obj')

# List of VAOs for each mesh in the object
vaos = []

# Iterate over meshes in the loaded file
for mesh in obj.meshes {
    v = gl.VAO()
    v.bind()

    # Create a VBO describing the vertex data
    data = nx.zeros((mesh.nvert, 8), nx.float)
    data[..., 0:3] = mesh.pos
    data[..., 3:6] = mesh.normals
    data[..., 6:8] = mesh.uv
    vbo = gl.VBO(data)
    
    # Create an EBO describing the triangles
    ebo = gl.EBO(mesh.idx as nx.u32)

    # Describe the 3 vertex attributes
    v.attrib(0, 3, gl.FLOAT, gl.FALSE, 8 * nx.float.size, 0)
    v.attrib(1, 3, gl.FLOAT, gl.FALSE, 8 * nx.float.size, 3 * nx.float.size)
    v.attrib(2, 2, gl.FLOAT, gl.FALSE, 8 * nx.float.size, 6 * nx.float.size)

    # Unbind the vertex
    v.unbind()

    vaos.push(v)
}

pos = nx.float([0, 0, 3])


# While it should stay rendering
while window {

    ## Logic ##


    ## Render Setup ##
    
    # Query the size of the window
    (w, h) = window.size

    # Draw over the entire window
    gl.viewport(0, 0, w, h)

    # Clear the screen
    gl.clear(gl.DEPTH_BUFFER_BIT | gl.COLOR_BUFFER_BIT)

    # Enable depth testing
    gl.enable(gl.DEPTH_TEST)


    # Set background color
    gl.clear_color(0.1, 0.1, 0.1)

    # Wireframe mode
    #gl.polygon_mode(gl.FRONT_AND_BACK, gl.LINE)

    pos[0] = m.sin(time.time())
    uP = gl.perspective(m.rad(60), w / h)
    uV = gl.lookat(pos, (0, 0, 0))


    ## Render Scene ##

    # Use the shader program
    shader.use()

    # Go ahead and set the uniform
    shader.uniform('uPV', uP @ uV)

    # Renders a single node
    func render(node, T=none) {
        # First, set the transformation matrix
        T = node.transform @ T if T else node.transform
        shader.uniform('uM', T)

        # List of meshes to render
        for i in node.meshes {
            # Bind the VAO for the corresponding mesh
            vaos[i].bind()

            # Draw the correct number of datapoints
            gl.draw_elements(gl.TRIANGLES, obj.meshes[i].ntri * 3, gl.UNSIGNED_INT)

            # Done with the VAO
            vaos[i].unbind()
        }

        # Render children
        for sub in node.sub {
            render(sub, T)
        }
    }
    
    # Render, starting at the root
    render(obj.root)

    ## Finalize frame ##

    # Poll events
    gl.glfw.poll()

    # Swap buffers
    window.swap()
}
