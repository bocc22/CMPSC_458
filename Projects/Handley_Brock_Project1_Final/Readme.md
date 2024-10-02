# CMPSC 458 Project 1

Please fill out the README's content in your intermediate submission 2 and final submission.

Student Name: Brock Handley
Student ID: bdh5437
Email: bdh5437@psu.edu

## How to run the Project

To run this project, please use Project_1.exe located in the Project_1 folder.

## Project description

This project creates a series of boxes and allows the user to apply transformations to them using keyboard inputs. The inputs are as follows:
U: + X Transformation | J: - X Transformation
I: + Y Transformation | K: - Y Transformation
O: + Z Transformation | L: - Z Transformation

Pressing the keys alone will cause a rotation about the axis of the key.
Pressing the keys combined with the SHIFT key will cause a scale in the direction of the key.
Pressing the keys combined with the CTRL key will cause a translation in the direction of the key.
Pressing the keys combined with the ALT key will cause a shear in the direction of the key.

Additionally, G can be used to stop rotation and reset the scale, translations, and shearing to their default settings. If you hold SHIFT, CTRL, or ALT while pressing G, it will stop rotation and reset all transformations except the transformation of the additional key. (i.e. SHIFT+G resets all transformations except for scaling.)
If you would like to reset the orientation of the cube, please press H.

**NOTE - Resetting orientation and stopping rotation are different. Stopping rotation means the cubes will not continue to rotate, but remain at whatever angle they were stopped at. Resetting orientation will both stop rotation and return each face to the direction it was facing in the beginning. Resetting Orientation will not reset any other transformations.

WASD can also be used to move the camera around, and you may use your mouse input to look with the camera.

In the distance, the user will see a skybox surrounding the area. This skybox moves along with the camera, so it is impossible to reach.

Below the the transformation boxes, the user will find terrain drawn using a heightmap. 

## Extra credit attempt

For extra credit, I implemented shearing and additional reset keys to my project. 

The shearing was implemented by creating shearing values for each direction and combining them with an identity matrix to create a shearing matrix that was then applied to the overall transformation matrix. This transformation is applied between the scaling and rotation transformations.

The additional resets were implemented by only selectively resetting certain values. In the regular G press, it simply sets each direction's rotation rate, translations, and shearing to 0 and its scaling to 1. For the additional resets, I reset each other value to its default, leaving only the corresponding transformation. For the orientation reset, I simply set the rotation rates and rotation values of each direction back to 0.
