#!/usr/bin/env ks
""" basic.ks - Simple example showing the kscript OpenGL bindings


@author: Cade Brown <cade@kscript.org>
"""

# OpenGL, GLFW, etc. bindings
import gl

# NumeriX, used for efficient data transfer
import nx


# Create a basic window
window = gl.glfw.Window("basic", (640, 480))


# While it should stay rendering
while window {

    ## Logic



    ## Render
    
    # Query the size of the window
    (w, h) = window.size

    # Draw over the entire window
    gl.viewport(0, 0, w, h)

    # Clear the screen
    gl.clear(gl.DEPTH_BUFFER_BIT | gl.COLOR_BUFFER_BIT)

    # Set background color
    gl.clearColor(0.1, 0.1, 0.1)


    ## Finalize frame

    # Poll events
    gl.glfw.poll()

    # Swap buffers
    window.swap()
}



