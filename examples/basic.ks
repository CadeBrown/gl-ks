#!/usr/bin/env ks
""" basic.ks - basic rendering of 2D objects


@author: Cade Brown <cade@kscript.org>
"""

# OpenGL bindings
import gl

# NumeriX, for specific datatypes
import nx

# audio/video loaders
import av

# Create a basic window from GLFW (part of the 'gl' package)
window = gl.glfw.Window("basic", (640, 480))

# Create a 'gl.Shader' object. This is how we tell the computer to render 
#   objects. The constructor takes the vertex shader source, then the fragment
#   shader source.
#
# SEE: GLSL (OpenGL Shading Language) https://www.khronos.org/opengl/wiki/OpenGL_Shading_Language
shader = gl.Shader("""#version 330 core
layout(location = 0) in vec3 vXYZ;
layout(location = 1) in vec2 vUV;

out vec2 fUV;

void main() {
    gl_Position = vec4(vXYZ.x, vXYZ.y, vXYZ.z, 1.0);
    fUV = vUV;
}
""", """#version 330 core
layout(location = 0) out vec4 oDiffuse;

in vec2 fUV;

// Texture to sample
uniform sampler2D uTex0;

void main() {
    oDiffuse = texture(uTex0, fUV);
}
""")


# Create a 'gl.VAO' object. This is a state binding, which will capture the
#   buffers and atttributes we create within it
tri = gl.VAO()
tri.bind()

# Create a 'gl.VBO' object. This is a buffer on the GPU, which can hold any
#   data. We make sure to make it an array of 'nx.float', since we use 'gl.FLOAT'
#   when creating the attribute. You can upload any arbitrary bytes, though
#
# This data is an array of [x, y, z, u, v] coordinates for each vertex
#
# Due to OpenGL's global state, it is important that we create it within the 
#   'tri.bind()' and 'tri.unbind()' calls
tri_vbo = gl.VBO(nx.float([
    [-0.5, -0.5, 0.0, 0.0, 0.0],
    [0.5, -0.5, 0.0, 0.0, 1.0],
    [0.0, 0.5, 0.0, 1.0, 0.0],
]))

# Create a 'gl.EBO' object. This is also a buffer on the GPU, but it represents
#   indexes (in this case, the faces of triangles) within the VBO created above.
#   We make sure to use 'nx.u32', since we use 'gl.UNSIGNED_INT' when rendering
#
# Due to OpenGL's global state, it is important that we create it within the 
#   'tri.bind()' and 'tri.unbind()' calls
tri_ebo = gl.EBO(nx.u32([
    [0, 1, 2],
]))

# vec3 xyz;
tri.attrib(0, 3, gl.FLOAT, gl.FALSE, 5 * nx.float.size, 0)
tri.enableAttrib(0)

# vec2 uv;
tri.attrib(1, 2, gl.FLOAT, gl.FALSE, 5 * nx.float.size, 3 * nx.float.size)
tri.enableAttrib(1)

# We are done, so unbind it. When we re-bind it, the VBO and EBO are loaded as the current
#   state
tri.unbind()

func make_tex2d(data) {
    # Now, create a texture
    data = nx.fpcast(data, nx.u8)

    # decide what format
    (h, w, d) = data.shape
    ret gl.Texture2D(data, w, h, gl.RGB if d == 3 else gl.RGBA)
}
# Helper function to make a texture from a file
func make_tex2df(fname) {
    ret make_tex2d(av.imread(fname))
}

# Create a texture
tex0 = make_tex2df('assets/wall.jpg')

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

    # Set background color
    gl.clearColor(0.1, 0.1, 0.1)


    # Wireframe mode
    #gl.polygonMode(gl.FRONT_AND_BACK, gl.LINE)


    ## Render Scene ##

    # Use the shader program
    shader.use()

    # Bind and set the shader uniform
    tex0.bind(0)
    shader.uniform('uTex0', 0)

    # Bind the VAO 
    tri.bind()
    # Now, draw all the elements of the VAO
    # '3' is the number of datapoints, and each triangle takes 3
    gl.drawElements(gl.TRIANGLES, 3, gl.UNSIGNED_INT, 0)

    # Done with the VAO
    tri.unbind()


    ## Finalize frame ##

    # Poll events
    gl.glfw.poll()

    # Swap buffers
    window.swap()
}

