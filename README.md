![AudioOrbs](images/DismalThoughtfulKoi.gif)

View the <a href="https://vimeo.com/181585320" target="_blank">demo</a> shown above or watch a <a href="https://vimeo.com/117489883" target="_blank">live performance</a>.

# Graphical Music Editor

*Logan Barnes, Luke Fraker (Graphics), and Jake Shields (Performance)*

## Overview

This Graphical Music Editor is a combined final project for Brown University's ***CSCI 1230 - Intro to Computer Graphics*** and ***MUSC 1210 - Seminar in Electronic Music: Real-Time Systems***. It is a multi-platform, spatially-based, sound visualizer/modifier created with a custom C++/OpenGL graphics engine and MaxMSP.

## Technical Features
**Multiplatform** - The game utilizes the Qt Framework allowing it to be run on OS X and Linux.

**Real-Time Sound Visualization** - Each audio track is analyzed using a Fast Forier Transform and visualized in the form of a smooth orb through the use of Bézier curves.

**Networking** - The graphical program communicates with the MaxMSP patch through a UDP connection.

**OpenGL Core Profile** - Using the newer OpenGL API's allowed for increased speed and shader control. Transparent objects were created by using a geometry shader to render the scene multiple times from differing perspectives.

**Original Music** - The music for the demo was written by Scott McGowan and Logan Barnes and the guitar music for the performance was written by Jake Shields.

*Note: This project is not actively mantained*
